/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <vendor/lineage/livedisplay/2.0/IAdaptiveBacklight.h>

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace sysfs {

using ::android::hardware::Return;

class AdaptiveBacklight : public IAdaptiveBacklight {
  public:
    AdaptiveBacklight();
    bool isSupported();

    // Methods from ::vendor::lineage::livedisplay::V2_0::IAdaptiveBacklight follow.
    Return<bool> isEnabled() override;
    Return<bool> setEnabled(bool enabled) override;

  private:
    const char* file_;
};

}  // namespace sysfs
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
