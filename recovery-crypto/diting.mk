# SPDX-License-Identifier: Apache-2.0
# This product include alone opts diting into the generic backend.
ifeq ($(DITING_RECOVERY_CRYPTO),true)
SOONG_CONFIG_NAMESPACES += recovery_crypto
SOONG_CONFIG_recovery_crypto += android17
SOONG_CONFIG_recovery_crypto_android17 := true
PRODUCT_SOONG_NAMESPACES += device/qcom/recovery-crypto-common device/xiaomi/diting/recovery-crypto vendor/xiaomi/sm8450-common
PRODUCT_PACKAGES += \
    diting_rec_android_hardware_gatekeeper_1_0_impl_qti \
    diting_rec_keymint \
    diting_rec_libGPreqcancel \
    diting_rec_libGPreqcancel_svc \
    diting_rec_libQSEEComAPI \
    diting_rec_libdiag \
    diting_rec_libdrmfs \
    diting_rec_libdrmtime \
    diting_rec_libkeymasterdeviceutils \
    diting_rec_libkeymasterutils \
    diting_rec_libqcbor \
    diting_rec_libqisl \
    diting_rec_libqtikeymaster4 \
    diting_rec_libqtikeymint \
    diting_rec_librpmb \
    diting_rec_libspcom \
    diting_rec_libspl \
    diting_rec_libssd \
    diting_rec_libtime_genoff \
    diting_rec_libvmmem \
    diting_rec_qseecomd \
    diting_rec_libxml2 \
    diting_rec_auth_service \
    diting_rec_crypto_config \
    diting_rec_crypto_fstab \
    diting_rec_crypto_manifest \
    diting_rec_crypto_init \
    librecovery_crypto_android17

# Keep the inherited common RC unchanged. The separate selected Soong module
# installs crypto init under /system/etc/init for init's automatic discovery.
PRODUCT_COPY_FILES += \
    device/xiaomi/diting/recovery-crypto/firmware_mount_point:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/firmware_mnt/.recovery-crypto-mount \
    device/qcom/recovery-crypto-common/ueventd.crypto.rc:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/etc/ueventd.rc \
    vendor/xiaomi/sm8450-common/proprietary/vendor/etc/gpfspath_oem_config.xml:$(TARGET_COPY_OUT_RECOVERY)/root/vendor/etc/gpfspath_oem_config.xml
endif
