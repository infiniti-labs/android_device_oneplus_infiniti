#
# SPDX-FileCopyrightText: The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit_only.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from infiniti device
$(call inherit-product, device/oneplus/infiniti/device.mk)

# Inherit some common PixelOS-CLO stuff.
TARGET_INCLUDE_PIXEL_LAUNCHER := false
TARGET_SUPPORTS_QUICK_TAP := false
$(call inherit-product, vendor/custom/config/common_full_phone.mk)

PRODUCT_NAME := custom_infiniti
PRODUCT_DEVICE := infiniti
PRODUCT_MANUFACTURER := OnePlus
PRODUCT_BRAND := OnePlus
PRODUCT_MODEL := CPH2745

PRODUCT_GMS_CLIENTID_BASE := android-oneplus

PRODUCT_BUILD_PROP_OVERRIDES += \
    BuildId=CP2A.260605.016 \
    BuildFlavor=qssi_64-user \
    Platform_security_patch=2026-10-01 \
    BuildDesc="qssi_64-user 17 CP2A.260605.016 1792254325225 release-keys" \
    BuildFingerprint=OnePlus/CPH2747/OP611FL1:17/CP2A.260605.016/B.R4T3.55357f5-2eb9868-2eda512:user/release-keys \
    DeviceName=OP611FL1 \
    DeviceProduct=CPH2745 \
    SystemDevice=OP611FL1 \
    SystemName=CPH2745
