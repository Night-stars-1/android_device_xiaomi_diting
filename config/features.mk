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
ifeq ($(filter https://%,$(DITING_OTA_BASE_URL)),)
$(error DITING_ROM_UPDATER requires an explicit HTTPS DITING_OTA_BASE_URL)
endif
ifeq ($(patsubst https://%,%,$(DITING_OTA_BASE_URL)),)
$(error DITING_OTA_BASE_URL must contain a host)
endif
ifneq ($(words $(DITING_OTA_BASE_URL)),1)
$(error DITING_OTA_BASE_URL must be a single HTTPS URL)
endif
endif
