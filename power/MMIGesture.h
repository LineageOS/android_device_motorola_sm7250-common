/*
 * SPDX-FileCopyrightText: 2023 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <android-base/file.h>
#include <android-base/strings.h>

namespace mmi_gesture {

inline const std::string& TouchscreenPath() {
    static const std::string path = []() -> std::string {
        constexpr const char* kTouchscreenPaths[] = {
                "/sys/class/touchscreen/primary",
                "/sys/class/touchscreen/ft8756",
        };

        for (const char* candidate : kTouchscreenPaths) {
            if (access(candidate, F_OK) == 0) {
                return candidate;
            }
        }

        /* return as fall back */
        return "/sys/class/touchscreen/primary";
    }();

    return path;
}

inline const std::string& GestureNode() {
    static const std::string node = TouchscreenPath() + "/gesture";
    return node;
}

inline const std::string& GestureModeTypeNode() {
    static const std::string node = TouchscreenPath() + "/gesture_mode_type";
    return node;
}

// Keep them in sync with the kernel
enum class Gesture { kSingleTap = 0x20, kDoubleTap = 0x30 };

inline bool IsEnabled(Gesture gesture) {
    std::string buf;
    if (!android::base::ReadFileToString(GestureModeTypeNode(), &buf)) {
        return false;
    }

    // Bits 0-2 correspond to zero tap, single tap, and double tap
    std::bitset<3> gesture_mode_type = std::stoi(android::base::Trim(buf));
    switch (gesture) {
        case Gesture::kSingleTap:
            return gesture_mode_type[1];
        case Gesture::kDoubleTap:
            return gesture_mode_type[2];
        default:
            return false;
    }
}

inline bool SetEnabled(Gesture gesture, bool enabled) {
    int code = static_cast<int>(gesture);
    if (enabled) ++code;
    if (!android::base::WriteStringToFile(std::to_string(code), GestureNode())) {
        return false;
    }
    return true;
}

}  // namespace mmi_gesture
