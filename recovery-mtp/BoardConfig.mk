# SPDX-License-Identifier: Apache-2.0
ifeq ($(DITING_RECOVERY_MTP),true)
ifeq ($(DITING_RECOVERY_CRYPTO),true)
BOARD_SEPOLICY_M4DEFS += recovery_mtp=true
endif
endif
