// SPDX-License-Identifier: Apache-2.0
#include <android-base/logging.h>
#include <android-base/parseint.h>
#include <fs_mgr.h>
#include <sys/system_properties.h>
#include <string>

namespace {
struct Property { const char* name; const char* value; };
struct Region {
    unsigned int id;
    const char* name;
    const Property* properties;
    unsigned int count;
};

constexpr Property kINProperties[] = {
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

constexpr Property kEUProperties[] = {
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

constexpr Property kNAProperties[] = {
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

constexpr Property kGLOProperties[] = {
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

constexpr Property kCNProperties[] = {
    {"ro.build.user", "root"},
    {"ro.build.host", "kvm-slave-build-s-system-14186846"},
    {"ro.build.flavor", "qssi-user"},
    {"ro.build.description", "qssi-user 17 CP2A.260605.016 1791045851048 release-keys"},
    {"ro.boot.vbmeta.public_key_digest", "8d897f62492ea617f777bad41a5711ab621fcac1efc1865b890328ee8c3853bb"},
    {"ro.boot.vbmeta.size", "19776"},
    {"ro.boot.vbmeta.boot.digest", "81a458a679979ea338aec8baa2f98e1a6149c0b4319ca8d7db4fee16908e7f34"},
    {"ro.boot.vbmeta.dtbo.digest", "e7abc43bc0385c9229c2c9f791919407e6effb86e0223797414371437a5578ca"},
    {"ro.boot.vbmeta.init_boot.digest", "8cc997e22096edb56cc482aa9b93bc63f4c08d329df85ce2b4646045fc01e628"},
    {"ro.boot.vbmeta.vendor_boot.digest", "a3c0bce6097b9677dd8089cce89e1382e395c38932519945d11a7fe8c12b87b3"},
    {"ro.boot.vbmeta.pvmfw.digest", "3b4e848c16ee43d233fb41fd4ae0dfda990fc5190e3a5d39fe6ccffe5e8b5c8f"},
    {"ro.boot.vbmeta.digest", "9d3b1a660b8bd4fe92aa8328be1d61d255233b7b5f71a22d6fd0d52f9acf55ff"},
};

constexpr Region kRegions[] = {
    {27, "IN", kINProperties, sizeof(kINProperties) / sizeof(Property)},
    {68, "EU", kEUProperties, sizeof(kEUProperties) / sizeof(Property)},
    {161, "NA", kNAProperties, sizeof(kNAProperties) / sizeof(Property)},
    {167, "GLO", kGLOProperties, sizeof(kGLOProperties) / sizeof(Property)},
    {151, "CN", kCNProperties, sizeof(kCNProperties) / sizeof(Property)},
};
constexpr unsigned int kFallback = 0;

const Region* selected_region() {
    std::string text;
    unsigned int region_id = 0;
    const Region* selected = nullptr;
    if (android::fs_mgr::GetKernelCmdline("oplus_region", &text) &&
        android::base::ParseUint(text, &region_id)) {
        for (const auto& region : kRegions) {
            if (region.id == region_id) {
                selected = &region;
                break;
            }
        }
    }
    if (!selected) {
        selected = &kRegions[kFallback];
        LOG(WARNING) << "infiniti: unavailable region ID " << region_id
                     << "; using " << selected->name << " profile";
    }
    return selected;
}
}  // namespace

// Use the standard device init hook to override existing read-only properties.
void vendor_load_properties() {
    const auto* region = selected_region();
    for (unsigned int i = 0; i < region->count; ++i) {
        const auto& property = region->properties[i];
        const auto* existing = __system_property_find(property.name);
        const auto length = std::char_traits<char>::length(property.value);
        const int result = existing
                ? __system_property_update(const_cast<prop_info*>(existing), property.value, length)
                : __system_property_add(property.name, std::char_traits<char>::length(property.name),
                                        property.value, length);
        if (result != 0) LOG(ERROR) << "infiniti: could not override " << property.name;
    }
}
