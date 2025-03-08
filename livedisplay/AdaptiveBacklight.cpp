/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include "AdaptiveBacklight.h"

#include <android-base/file.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

using ::android::base::GetBoolProperty;
using ::android::base::ReadFileToString;
using ::android::base::Trim;
using ::android::base::WriteStringToFile;

namespace {
    constexpr const char *kFileAcl = "/sys/devices/platform/soc/soc:qcom,dsi-display-primary/dsi_display_acl";
    constexpr const char *kFileCabc = "/sys/devices/platform/soc/soc:qcom,dsi-display-primary/dsi_display_cabc";
    constexpr const char *kFossProperty = "ro.vendor.display.foss";
}  // anonymous namespace

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace sysfs {

AdaptiveBacklight::AdaptiveBacklight() {
    if (!access(kFileAcl, R_OK | W_OK)) {
        file_ = kFileAcl;
    } else if (!access(kFileCabc, R_OK | W_OK)) {
        file_ = kFileCabc;
    } else {
        file_ = nullptr;
    }
}

bool AdaptiveBacklight::isSupported() {
    if (GetBoolProperty(kFossProperty, false) || file_ == nullptr) {
        return false;
    }
    return true;
}

// Methods from ::vendor::lineage::livedisplay::V2_0::IAdaptiveBacklight follow.
Return<bool> AdaptiveBacklight::isEnabled() {
    std::string tmp;
    int32_t contents = 0;

    if (ReadFileToString(file_, &tmp)) {
        contents = std::stoi(Trim(tmp));
    }

    return contents > 0;
}

Return<bool> AdaptiveBacklight::setEnabled(bool enabled) {
    return WriteStringToFile(std::to_string(enabled), file_, true);
}

}  // namespace sysfs
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
