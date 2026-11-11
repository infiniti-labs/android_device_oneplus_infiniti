// SPDX-License-Identifier: Apache-2.0
#include <android-base/logging.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <string>

namespace {
struct Property { const char* name; const char* value; };

// Stock values shared by every region.
constexpr Property kProperties[] = {
    {"ro.build.user", "root"},
    {"ro.build.host", "kvm-slave-build-s-system-14711641"},
    {"ro.build.flavor", "qssi_64-user"},
    {"ro.build.description", "qssi_64-user 17 CP2A.260605.016 1793215540701 release-keys"},
    {"ro.boot.vbmeta.boot.digest", "ee8b6be26f78d2be1f8e1c29c439f0c94fd8df3bcddbf337777a22b4aa3cdcd9"},
    {"ro.boot.vbmeta.dtbo.digest", "9ef58a9c6171c7ae9b658b33cd58aceb6cc2e408c56435e0ec8aef8aae8c8e4a"},
    {"ro.boot.vbmeta.init_boot.digest", "c69bd1c351302e5d914551f79246e05a8ddc0c1f758471b57fb36f92ba8a4bdc"},
    {"ro.boot.vbmeta.public_key_digest", "8d897f62492ea617f777bad41a5711ab621fcac1efc1865b890328ee8c3853bb"},
    {"ro.boot.vbmeta.pvmfw.digest", "d59a7ebe6c8d66e438debe8637bef99485bfcb0ad462844f36ded93211667762"},
    {"ro.boot.vbmeta.vendor_boot.digest", "0d943a95a4a531aec88fa166c6dc11121becb94968d9ba22a6e8e5e091cd14b5"},
    {"ro.boot.vbmeta.size", "19776"},
    {"ro.boot.vbmeta.digest", "b3a36735edddc817c5e7c14a05f2bda6935ab3c7d92b56632251ccb78f65b2c3"},
};

// Recovery and fastbootd gate flashing and adb on an orange verified boot state.
constexpr Property kRecoveryProperties[] = {
    {"ro.boot.verifiedbootstate", "orange"},
};

void Override(const Property& property) {
    const auto* existing = __system_property_find(property.name);
    const auto length = std::char_traits<char>::length(property.value);
    const int result = existing
            ? __system_property_update(const_cast<prop_info*>(existing), property.value, length)
            : __system_property_add(property.name, std::char_traits<char>::length(property.name),
                                    property.value, length);
    if (result != 0) LOG(ERROR) << "infiniti: could not override " << property.name;
}
}  // namespace

// Use the standard device init hook to override existing read-only properties.
void vendor_load_properties() {
    for (const auto& property : kProperties) Override(property);
    if (access("/system/bin/recovery", F_OK) == 0) {
        for (const auto& property : kRecoveryProperties) Override(property);
    }
}
