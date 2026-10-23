// SPDX-License-Identifier: Apache-2.0
#include <android-base/logging.h>
#include <sys/system_properties.h>
#include <string>

namespace {
struct Property { const char* name; const char* value; };

// Stock values shared by every region.
constexpr Property kProperties[] = {
    {"ro.build.user", "root"},
    {"ro.build.host", "kvm-slave-build-s-system-14456854"},
    {"ro.build.flavor", "qssi_64-user"},
    {"ro.build.description", "qssi_64-user 17 CP2A.260605.016 1792254325225 release-keys"},
    {"ro.boot.vbmeta.boot.digest", "06e95e9ae3f8f546ab9af9a58cec0cde59a46d9fdc6bbe5fed18bfdf3400e492"},
    {"ro.boot.vbmeta.dtbo.digest", "6b85d05b705b203ad090e3de0b745ead3c394ea45592db65d1314caedc50f992"},
    {"ro.boot.vbmeta.init_boot.digest", "54877e6c929342abcc58be2eebf5de9a3a493a597aa355a9e6f4a7ed91a71239"},
    {"ro.boot.vbmeta.public_key_digest", "8d897f62492ea617f777bad41a5711ab621fcac1efc1865b890328ee8c3853bb"},
    {"ro.boot.vbmeta.pvmfw.digest", "0c9b8a30d03c89249125ff55554f91fb7d793c4d18f4e32216e026448b23d99f"},
    {"ro.boot.vbmeta.vendor_boot.digest", "e764af7df4362af46f9d8c867271aaf8cffd365067dc0c448951af0700c5e1d5"},
    {"ro.boot.vbmeta.size", "19776"},
    {"ro.boot.vbmeta.digest", "4f37f0cf8d94f052f2096c45c3e08c03e12b010d1c1ca7918b7a1bd45b4ef1b9"},
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
