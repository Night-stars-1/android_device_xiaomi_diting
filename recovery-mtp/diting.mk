# SPDX-License-Identifier: Apache-2.0
# Optional adapter; no edits in bootable/recovery. Existing crypto supplies
# verified user-0 keys. Writable media is opt-in; never changes normal-system USB.
ifeq ($(DITING_RECOVERY_MTP),true)
ifeq ($(DITING_RECOVERY_CRYPTO),true)
SOONG_CONFIG_NAMESPACES += recovery_mtp
SOONG_CONFIG_recovery_mtp += enabled
SOONG_CONFIG_recovery_mtp_enabled := true
PRODUCT_SOONG_NAMESPACES += device/xiaomi/diting/recovery-mtp
PRODUCT_PACKAGES += recovery_mtp diting_rec_mtp_config
endif
endif
