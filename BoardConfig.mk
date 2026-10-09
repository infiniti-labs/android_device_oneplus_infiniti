#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

# Partitions
BOARD_SUPER_PARTITION_SIZE := 17062428672

# Include the common OEM chipset BoardConfig.
include device/oneplus/sm8850-common/BoardConfigCommon.mk

DEVICE_PATH := device/oneplus/infiniti

# GBL
ifeq ($(TARGET_ENABLE_GBL),true)
TARGET_KERNEL_BAZEL_FLAGS += --defconfig_fragment=//vendor/oneplus/sm8850:arch/arm64/configs/infiniti.fragment
TARGET_GBL_BOOTSTRAP_ABL := vendor/oneplus/infiniti/gbl/abl.img
TARGET_GBL_SYSTEM_VERSION := 262144
TARGET_GBL_SYSTEM_SPL := 2473
TARGET_GBL_ROT_DIGEST := 44149b5df4f23466590b6e9888b75e618dbe07220a078efcca37ef6218e566c7
TARGET_GBL_PUBKEY_DIGEST := 8d897f62492ea617f777bad41a5711ab621fcac1efc1865b890328ee8c3853bb
TARGET_GBL_VERIFIED_BOOT_HASH := c92a400c1dd86869c0ba2e0e5e5faf839f9cad3ebe8cfb3056ec4d8183653144
include external/gbl/BoardConfig.mk

# VBMeta
BOARD_AVB_MAKE_VBMETA_IMAGE_ARGS := --flags 1
endif

# Assert
TARGET_OTA_ASSERT_DEVICE := OP60FFL1,OP611FL1

# Display
TARGET_SCREEN_DENSITY := 560

# Kernel
TARGET_KERNEL_BAZEL_FLAGS += --//vendor/oneplus/sm8850:dtbo_config=//vendor/oneplus/sm8850-devicetrees:infiniti_dtbo_config

# Properties
TARGET_ODM_PROP += $(DEVICE_PATH)/properties/odm.prop
TARGET_SYSTEM_EXT_PROP += $(DEVICE_PATH)/properties/system_ext.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/properties/vendor.prop

# Recovery
TARGET_RECOVERY_DENSITY := xxhdpi
TARGET_RECOVERY_UI_MARGIN_HEIGHT := 103

# Include the proprietary files BoardConfig.
include vendor/oneplus/infiniti/BoardConfigVendor.mk
