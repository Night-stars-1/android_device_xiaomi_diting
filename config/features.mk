# SPDX-License-Identifier: Apache-2.0
# Generated user selections live outside tracked source projects.
DITING_ISLAND ?= false
DITING_ROM_UPDATER ?= false
DITING_TORCH_STRENGTH ?= false
DITING_RECOVERY_CRYPTO ?= false
DITING_RECOVERY_MTP ?= false
DITING_RECOVERY_CRYPTO_PROBE ?= false
DITING_OTA_BASE_URL ?=
-include .uwu-diting/features.mk

_diting_boolean_features := DITING_ISLAND DITING_ROM_UPDATER DITING_TORCH_STRENGTH DITING_RECOVERY_CRYPTO DITING_RECOVERY_MTP DITING_RECOVERY_CRYPTO_PROBE
$(foreach var,$(_diting_boolean_features),$(if $(filter 1,$(words $($(var)))),$(if $(filter true false,$($(var))),,$(error $(var) must be true or false)),$(error $(var) must be true or false)))
ifeq ($(DITING_RECOVERY_MTP),true)
ifneq ($(DITING_RECOVERY_CRYPTO),true)
$(error DITING_RECOVERY_MTP requires DITING_RECOVERY_CRYPTO=true)
endif
endif
ifeq ($(DITING_ROM_UPDATER),true)
# Escape quotes before passing the generated literal through the shell.
_diting_url_valid := $(shell python3 device/xiaomi/diting/config/validate_ota_url.py '$(subst ','"'"',$(DITING_OTA_BASE_URL))' && echo true)
ifneq ($(_diting_url_valid),true)
$(error DITING_ROM_UPDATER requires a valid literal HTTPS DITING_OTA_BASE_URL)
endif
endif
