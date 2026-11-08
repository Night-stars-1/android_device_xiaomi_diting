// SPDX-License-Identifier: Apache-2.0
// Read-only prerequisite observations. No credentials, key files or dm tables.
#include <android-base/properties.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <fstab/fstab.h>
#include <libdm/dm.h>
#include <selinux/selinux.h>

#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
using android::base::GetProperty;
using android::fs_mgr::Fstab;
using android::fs_mgr::FstabEntry;

void Emit(const std::string& key, std::string value) {
    // Single-line, fixed-key report; do not allow a property to inject report rows.
    for (char& byte : value) {
        if (static_cast<unsigned char>(byte) < 0x20 || byte == 0x7f) byte = '?';
    }
    std::cout << key << '=' << value << '\n';
}

bool HasOption(const std::string& options, const std::string& wanted) {
    std::istringstream stream(options);
    std::string token;
    while (std::getline(stream, token, ',')) {
        if (token == wanted) return true;
    }
    return false;
}

bool IsDiting() {
    bool found = false;
    for (const char* name : {"ro.product.device", "ro.product.vendor.device", "ro.product.odm.device"}) {
        const auto value = GetProperty(name, "");
        if (!value.empty()) {
            found = true;
            if (value != "diting") return false;
        }
    }
    return found;
}

void ObserveFstab() {
    for (const char* file : {"/etc/recovery.fstab", "/system/etc/recovery.fstab"}) {
        if (access(file, R_OK) != 0) continue;
        Emit("fstab.source", file);
        Fstab fstab;
        if (!android::fs_mgr::ReadFstabFromFile(file, &fstab)) {
            Emit("fstab.profile", "parse_failed");
            return;
        }
        std::vector<const FstabEntry*> data;
        std::vector<const FstabEntry*> metadata;
        for (const auto& entry : fstab) {
            if (entry.mount_point == "/data") data.push_back(&entry);
            if (entry.mount_point == "/metadata") metadata.push_back(&entry);
        }
        if (data.size() != 1 || metadata.size() != 1) {
            Emit("fstab.profile", "ambiguous_or_missing_entries");
            return;
        }
        const auto& d = *data.front();
        const auto& m = *metadata.front();
        Emit("fstab.data.filesystem", d.fs_type);
        Emit("fstab.data.encryption", d.encryption_options);
        Emit("fstab.data.metadata_encryption", d.metadata_encryption_options);
        Emit("fstab.data.inlinecrypt", HasOption(d.fs_options, "inlinecrypt") ? "present" : "not_observed");
        bool aligned = d.blk_device == "/dev/block/bootdevice/by-name/userdata" &&
                       d.fs_type == "f2fs" && HasOption(d.fs_options, "inlinecrypt") &&
                       d.encryption_options == "aes-256-xts:aes-256-cts:v2+inlinecrypt_optimized+wrappedkey_v0" &&
                       d.metadata_encryption_options == "aes-256-xts:wrappedkey_v0" &&
                       d.metadata_key_dir == "/metadata/vold/metadata_encryption" &&
                       !d.fs_mgr_flags.wrapped_key && m.fs_type == "ext4" &&
                       (m.blk_device == "/dev/block/bootdevice/by-name/metadata" ||
                        m.blk_device == "/dev/block/by-name/metadata") &&
                       !m.fs_mgr_flags.wrapped_key && m.encryption_options.empty() &&
                       m.metadata_encryption_options.empty();
        Emit("fstab.profile", aligned ? "audited_diting_profile" : "different_from_audited_profile");
        return;
    }
    Emit("fstab.profile", "unavailable");
}

void ObserveMounts() {
    Fstab mounts;
    if (!android::fs_mgr::ReadFstabFromProcMounts(&mounts)) {
        Emit("mounts.query", "unavailable");
        return;
    }
    Emit("mounts.query", "complete");
    for (const char* point : {"/metadata", "/data"}) {
        const auto* entry = android::fs_mgr::GetEntryForMountPoint(&mounts, std::string(point));
        const auto prefix = std::string("mounts.") + (std::string(point) == "/data" ? "data" : "metadata");
        Emit(prefix + ".state", entry ? "mounted" : "not_observed");
        if (entry) {
            Emit(prefix + ".source", entry->blk_device);
            Emit(prefix + ".filesystem", entry->fs_type);
        }
    }
}

void ObserveDeviceMapper() {
    std::vector<android::dm::DmTargetTypeInfo> targets;
    // DM_LIST_VERSIONS only. Never ask for DM_TABLE_STATUS or any mapping keys.
    if (!android::dm::DeviceMapper::Instance().GetAvailableTargets(&targets)) {
        Emit("dm.query", "unavailable");
        Emit("dm.default_key", "unknown");
        return;
    }
    Emit("dm.query", "complete");
    const bool listed = std::any_of(targets.begin(), targets.end(), [](const auto& target) {
        return target.name() == "default-key";
    });
    Emit("dm.default_key", listed ? "listed" : "not_listed");
}

void ObserveBinder() {
    const bool manager_running = GetProperty("init.svc.servicemanager", "") == "running";
    Emit("binder.manager", manager_running ? "init_reports_running" : "not_observed");
    const char* names[] = {
        "android.hardware.security.keymint.IKeyMintDevice/default",
        "android.hardware.security.sharedsecret.ISharedSecret/default",
        "android.hardware.security.secureclock.ISecureClock/default",
        "android.system.keystore2.IKeystoreService/default",
        "android.security.maintenance",
        "android.security.authorization",
    };
    for (const char* name : names) {
        if (!manager_running) {
            Emit(std::string("binder.") + name, "not_queried");
            continue;
        }
        // Non-waiting lookup only; no crypto RPC and no lazy-service startup request.
        AIBinder* service = AServiceManager_checkService(name);
        Emit(std::string("binder.") + name, service ? "registered" : "not_observed_or_denied");
        if (service) AIBinder_decStrong(service);
    }
    Emit("gatekeeper.hidl_interface", "not_queried");
}
}  // namespace

int main(int argc, char** argv) {
    if (argc != 2 || std::string(argv[1]) != "--check") {
        std::cerr << "Usage: diting_recovery_crypto_probe --check (inside diting Recovery)\n";
        return 2;
    }
    std::cout << std::unitbuf;
    Emit("probe.schema", "1");
    Emit("probe.complete", "false");
    Emit("recovery.decryption", "not_implemented");
    if (!IsDiting()) {
        Emit("probe.error", "wrong_or_unknown_device");
        return 3;
    }
    if (GetProperty("init.svc.recovery", "") != "running" &&
        GetProperty("ro.bootmode", "") != "recovery") {
        Emit("probe.error", "recovery_environment_required");
        return 3;
    }
    // Terminate a stalled Binder/driver query; an interrupted report remains incomplete.
    alarm(20);
    Emit("probe.environment", "recovery");
    Emit("probe.euid", std::to_string(geteuid()));
    char* context = nullptr;
    if (getcon(&context) == 0) {
        Emit("probe.selinux_context", context);
        freecon(context);
    } else {
        Emit("probe.selinux_context", "unavailable");
    }
    const int enforcing = security_getenforce();
    Emit("selinux.mode", enforcing == 1 ? "enforcing" : enforcing == 0 ? "permissive" : "unknown");
    for (const char* name : {"vendor.keymint-qti", "vendor.keymaster-4-1", "gatekeeper-1-0",
                             "vendor.qseecomd", "qseecom-service", "keystore2", "hwservicemanager"}) {
        Emit(std::string("init.") + name, GetProperty(std::string("init.svc.") + name, "unknown"));
    }
    ObserveFstab();
    ObserveMounts();
    ObserveDeviceMapper();
    ObserveBinder();
    Emit("metadata.unlock", "not_attempted");
    Emit("de.unlock", "not_attempted");
    Emit("ce.unlock", "not_attempted");
    alarm(0);
    Emit("probe.complete", "true");
    return 0;  // Report completed, NOT a readiness/decryption success code.
}
