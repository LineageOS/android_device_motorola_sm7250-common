/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vendor/lineage/livedisplay/2.0/ISunlightEnhancement.h>

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace sysfs {

using ::android::hardware::Return;

class SunlightEnhancement : public ISunlightEnhancement {
  public:
    SunlightEnhancement();
    bool isSupported();

    // Methods from ::vendor::lineage::livedisplay::V2_0::ISunlightEnhancement follow.
    Return<bool> isEnabled() override;
    Return<bool> setEnabled(bool enabled) override;

  private:
    const char* file_;
    int32_t enabled_mode_;
};

}  // namespace sysfs
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
