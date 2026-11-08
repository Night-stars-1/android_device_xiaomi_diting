# SPDX-License-Identifier: Apache-2.0
ifeq ($(DITING_RECOVERY_CRYPTO),true)
BOARD_VENDOR_SEPOLICY_DIRS += device/qcom/recovery-crypto-common/sepolicy
BOARD_SEPOLICY_M4DEFS += recovery_crypto_android17=true
endif
