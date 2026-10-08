// SPDX-License-Identifier: Apache-2.0
#include <android-base/logging.h>
#include <sys/system_properties.h>
#include <string>

namespace {
struct Property { const char* name; const char* value; };

// Stock values shared by every region.
constexpr Property kProperties[] = {
    {"ro.build.user", "root"},
    {"ro.build.host", "kvm-slave-build-s-system-14236107"},
    {"ro.build.flavor", "qssi_64-user"},
    {"ro.build.description", "qssi_64-user 17 CP2A.260605.016 1791473060061 release-keys"},
    {"ro.boot.vbmeta.boot.digest", "32143c5fa2b88587d2c341090cd004a644d11735692532615af3377bc4a2cdec"},
    {"ro.boot.vbmeta.dtbo.digest", "74e4dbb99187ab5d3a4952ec27f112989e1b96a1ada90028485c3d86d593e7da"},
    {"ro.boot.vbmeta.init_boot.digest", "1a729d8470335e959cc928a531072d679cb4525a6fd88ba9d886f98148286412"},
    {"ro.boot.vbmeta.public_key_digest", "8d897f62492ea617f777bad41a5711ab621fcac1efc1865b890328ee8c3853bb"},
    {"ro.boot.vbmeta.pvmfw.digest", "0c9b8a30d03c89249125ff55554f91fb7d793c4d18f4e32216e026448b23d99f"},
    {"ro.boot.vbmeta.vendor_boot.digest", "37db244154b46b6c99f6515a8b5b43842d3c79bc97609b15a6f0968387e2c0a8"},
    {"ro.boot.vbmeta.size", "19776"},
    {"ro.boot.vbmeta.digest", "09eaff3a5c9ee12c78eaa79c8aaa2b352d1e9d93170b626ff44659b444f305a4"},
};
}  // namespace

// Use the standard device init hook to override existing read-only properties.
void vendor_load_properties() {
    for (const auto& property : kProperties) {
        const auto* existing = __system_property_find(property.name);
        const auto length = std::char_traits<char>::length(property.value);
        const int result = existing
                ? __system_property_update(const_cast<prop_info*>(existing), property.value, length)
                : __system_property_add(property.name, std::char_traits<char>::length(property.name),
                                        property.value, length);
        if (result != 0) LOG(ERROR) << "infiniti: could not override " << property.name;
    }
}
