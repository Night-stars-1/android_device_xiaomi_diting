// SPDX-License-Identifier: Apache-2.0
#define LOG_TAG "DitingTorchStrength"

#include "common/CameraProviderExtension.h"
#include "TorchStrength.h"

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <android/log.h>
#include <mutex>
#include <string>
#include <unistd.h>

namespace {
constexpr const char* kBrightness = "/sys/class/leds/led:torch_0/brightness";
constexpr const char* kSwitch = "/sys/class/leds/led:switch_0/brightness";
std::mutex gMutex;

class Fd {
public:
    explicit Fd(int value) : value_(value) {}
    ~Fd() { if (value_ >= 0) close(value_); }
    Fd(const Fd&) = delete;
    Fd& operator=(const Fd&) = delete;
    int get() const { return value_; }
private:
    int value_;
};

int readInteger(const char* path, int* value) {
    const Fd fd(open(path, O_RDONLY | O_CLOEXEC));
    if (fd.get() < 0) return -errno;
    char buffer[32] = {};
    ssize_t count;
    do { count = read(fd.get(), buffer, sizeof(buffer) - 1); } while (count < 0 && errno == EINTR);
    if (count < 0) return -errno;
    if (count == 0 || count == static_cast<ssize_t>(sizeof(buffer) - 1)) return -EIO;
    char* end = nullptr;
    errno = 0;
    const long parsed = strtol(buffer, &end, 10);
    if (errno != 0 || end == buffer || parsed < 0 || parsed > 10000) return -EINVAL;
    while (*end == ' ' || *end == '\n' || *end == '\r' || *end == '\t') ++end;
    if (*end != '\0') return -EINVAL;
    *value = static_cast<int>(parsed);
    return 0;
}

class SysfsIo final : public diting::torch::Io {
public:
    int readCurrent(int* value) override { return readInteger(kBrightness, value); }
    int readSwitch(int* value) override { return readInteger(kSwitch, value); }
    int writeCurrent(int value) override {
        // No switch or trigger writes: ordinary HAL on/off must already work.
        const Fd fd(open(kBrightness, O_WRONLY | O_CLOEXEC));
        if (fd.get() < 0) return -errno;
        const std::string text = std::to_string(value) + "\n";
        ssize_t count;
        do { count = write(fd.get(), text.data(), text.size()); } while (count < 0 && errno == EINTR);
        if (count < 0) return -errno;
        return count == static_cast<ssize_t>(text.size()) ? 0 : -EIO;
    }
};

SysfsIo gIo;
diting::torch::Controller gController(gIo);
}  // namespace

bool supportsTorchStrengthControlExt() {
    const Fd brightness(open(kBrightness, O_RDWR | O_CLOEXEC));
    const Fd enabled(open(kSwitch, O_RDONLY | O_CLOEXEC));
    return brightness.get() >= 0 && enabled.get() >= 0;
}

int32_t getTorchDefaultStrengthLevelExt() {
    return supportsTorchStrengthControlExt() ? diting::torch::kDefaultLevel : 1;
}
int32_t getTorchMaxStrengthLevelExt() {
    return supportsTorchStrengthControlExt() ? diting::torch::kMaxLevel : 1;
}

int32_t getTorchStrengthLevelExt() {
    std::lock_guard<std::mutex> lock(gMutex);
    return gController.getLevel();
}

void setTorchStrengthLevelExt(int32_t strength, bool enabled) {
    std::lock_guard<std::mutex> lock(gMutex);
    const int status = gController.setLevel(strength, enabled);
    if (status != 0) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,
                            "Torch strength request failed: level=%d enabled=%d status=%d",
                            strength, enabled, status);
    }
}
