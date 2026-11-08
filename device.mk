#
# Copyright (C) 2024 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

include device/xiaomi/diting/config/features.mk

# Inherit from xiaomi sm8450-common
TARGET_HAS_UDFPS := true
$(call inherit-product, device/xiaomi/sm8450-common/common.mk)

# Inherit from the proprietary version
$(call inherit-product, vendor/xiaomi/diting/diting-vendor.mk)

# Audio
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/audio/mixer_paths_waipio_mtp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_cape/mixer_paths_waipio_mtp.xml \
    $(LOCAL_PATH)/audio/resourcemanager_waipio_mtp.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio/sku_cape/resourcemanager_waipio_mtp.xml \
    $(LOCAL_PATH)/audio/usecaseKvManager.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usecaseKvManager.xml

# Overlay
PRODUCT_PACKAGES += \
    ApertureResDiting \
    FrameworksResDiting \
    NfcResDiting \
    SettingsProviderResDiting \
    SettingsProviderResDitingCN \
    SettingsResDiting \
    SystemUIResDiting \
    WifiResDiting \
    WifiResDitingCN

# Soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    $(LOCAL_PATH)

# System properties
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/properties/build_CN.prop:$(TARGET_COPY_OUT_ODM)/etc/build_CN.prop \
    $(LOCAL_PATH)/properties/build_GL.prop:$(TARGET_COPY_OUT_ODM)/etc/build_GL.prop

PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/properties/build_CN.prop:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/odm/etc/build_CN.prop \
    $(LOCAL_PATH)/properties/build_GL.prop:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/odm/etc/build_GL.prop

# Vibrator
$(call soong_config_set,qti_vibrator,effect_lib,libqtivibratoreffect.xiaomi)
$(call soong_config_set_bool,qti_vibrator,use_effect_stream,true)

# BEGIN DITING LEGACY FCM 6
PRODUCT_PACKAGES += diting_legacy_framework_compatibility_matrix.6.xml
# END DITING LEGACY FCM 6

# Optional source features.
ifeq ($(DITING_ISLAND),true)
PRODUCT_PACKAGE_OVERLAYS += device/xiaomi/diting/overlay-uwu-island
PRODUCT_SYSTEM_EXT_PROPERTIES += ro.uwu.feature.island=true persist.sys.feature.island=true ro.mi.os.version.code?=2
endif
ifeq ($(DITING_ROM_UPDATER),true)
PRODUCT_PACKAGES += uwuUpdater
PRODUCT_SYSTEM_PROPERTIES += ro.uwu.ota.base_url=$(DITING_OTA_BASE_URL)
endif
ifeq ($(DITING_RECOVERY_CRYPTO),true)
$(call inherit-product, device/xiaomi/diting/recovery-crypto/diting.mk)
endif
ifeq ($(DITING_RECOVERY_MTP),true)
$(call inherit-product, device/xiaomi/diting/recovery-mtp/diting.mk)
endif
ifeq ($(DITING_RECOVERY_CRYPTO_PROBE),true)
PRODUCT_PACKAGES += diting_recovery_crypto_probe
endif

ifeq ($(DITING_TORCH_STRENGTH),true)
$(call soong_config_set,libcameraservice,ext_lib,libcameraservice_diting_torch)
$(call soong_config_set_bool,camera,disable_torch_control,false)
PRODUCT_COPY_FILES += \
    device/xiaomi/diting/torch/ueventd.diting-torch.rc:$(TARGET_COPY_OUT_VENDOR)/etc/ueventd.diting-torch.rc
endif

ifeq ($(UWU_SOONG_ONLY_BUILD),true)
PRODUCT_PACKAGES += \
    diting_vendor_manifest_taro \
    diting_vendor_manifest_diwali \
    diting_vendor_manifest_cape \
    diting_vendor_manifest_ukee
endif
