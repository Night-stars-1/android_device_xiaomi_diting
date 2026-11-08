#
# Copyright (C) 2024 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

include device/xiaomi/diting/config/features.mk

# Security patch level
VENDOR_SECURITY_PATCH := 2026-08-01

# Inherit from xiaomi sm8450-common
include device/xiaomi/sm8450-common/BoardConfigCommon.mk

# Inherit from the proprietary version
include vendor/xiaomi/diting/BoardConfigVendor.mk

DEVICE_PATH := device/xiaomi/diting

# Audio
AUDIO_FEATURE_ENABLED_CIRRUS_CALIBRATION_RESISTANCE := true

# Properties
TARGET_ODM_PROP += $(DEVICE_PATH)/properties/odm.prop
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/properties/system.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/properties/vendor.prop

# Screen density
TARGET_SCREEN_DENSITY := 480

# Feature flags are shared by device Soong modules and policy.
SOONG_CONFIG_NAMESPACES += diting
SOONG_CONFIG_diting += island rom_updater torch_strength recovery_crypto recovery_mtp recovery_crypto_probe soong_only
SOONG_CONFIG_diting_soong_only := $(if $(filter true,$(UWU_SOONG_ONLY_BUILD)),true,false)
SOONG_CONFIG_diting_island := $(DITING_ISLAND)
SOONG_CONFIG_diting_rom_updater := $(DITING_ROM_UPDATER)
SOONG_CONFIG_diting_torch_strength := $(DITING_TORCH_STRENGTH)
SOONG_CONFIG_diting_recovery_crypto := $(DITING_RECOVERY_CRYPTO)
SOONG_CONFIG_diting_recovery_mtp := $(DITING_RECOVERY_MTP)
SOONG_CONFIG_diting_recovery_crypto_probe := $(DITING_RECOVERY_CRYPTO_PROBE)
ifeq ($(DITING_ROM_UPDATER),true)
SYSTEM_EXT_PRIVATE_SEPOLICY_DIRS += device/xiaomi/diting/rom-updater-sepolicy/private
endif
ifeq ($(DITING_RECOVERY_CRYPTO),true)
include device/xiaomi/diting/recovery-crypto/BoardConfig.mk
endif
ifeq ($(DITING_RECOVERY_MTP),true)
include device/xiaomi/diting/recovery-mtp/BoardConfig.mk
endif

ifeq ($(DITING_TORCH_STRENGTH),true)
BOARD_VENDOR_SEPOLICY_DIRS += device/xiaomi/diting/sepolicy/torch
endif
