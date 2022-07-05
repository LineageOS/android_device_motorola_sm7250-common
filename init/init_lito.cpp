/*
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (c) 2019, The LineageOS Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/sysinfo.h>
#include <android-base/logging.h>
#include <android-base/properties.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;
using android::base::SetProperty;
using std::string;

constexpr const char* RO_PROP_SOURCES[] = {
    nullptr,
    "bootimage.",
    "odm.",
    "odm_dlkm.",
    "product.",
    "system.",
    "system_dlkm.",
    "system_ext.",
    "vendor.",
    "vendor_dlkm.",
};

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

void property_override(string prop, string value)
{
    auto pi = (prop_info*) __system_property_find(prop.c_str());

    if (pi != nullptr)
        __system_property_update(pi, value.c_str(), value.size());
    else
        __system_property_add(prop.c_str(), prop.size(), value.c_str(), value.size());
}

void set_avoid_gfxaccel_config() {
    struct sysinfo sys;
    sysinfo(&sys);

    if (sys.totalram <= 4096ull * 1024 * 1024) {
        // Reduce memory footprint
        property_override("ro.config.avoid_gfx_accel", "true");
    }
}

void OverrideMemoryProperties() {
    struct sysinfo sys;
    std::string heapstartsize;
    std::string heapgrowthlimit;
    std::string heapsize;
    std::string heaptargetutilization;
    std::string heapminfree;
    std::string heapmaxfree;

    sysinfo(&sys);

    if (sys.totalram > 5072ull * 1024 * 1024) {
        // from - phone-xhdpi-6144-dalvik-heap.mk
        heapstartsize = "16m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.5";
        heapminfree = "8m";
        heapmaxfree = "32m";
    } else if (sys.totalram > 3072ull * 1024 * 1024) {
        // from - phone-xxhdpi-4096-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "256m";
        heapsize = "512m";
        heaptargetutilization = "0.6";
        heapminfree = "8m";
        heapmaxfree = "16m";
    } else {
        // from - phone-xhdpi-2048-dalvik-heap.mk
        heapstartsize = "8m";
        heapgrowthlimit = "192m";
        heapsize = "512m";
        heaptargetutilization = "0.75";
        heapminfree = "512k";
        heapmaxfree = "8m";
    }

    OverrideProperty("dalvik.vm.heapstartsize", heapstartsize.c_str());
    OverrideProperty("dalvik.vm.heapgrowthlimit", heapgrowthlimit.c_str());
    OverrideProperty("dalvik.vm.heapsize", heapsize.c_str());
    OverrideProperty("dalvik.vm.heaptargetutilization", heaptargetutilization.c_str());
    OverrideProperty("dalvik.vm.heapminfree", heapminfree.c_str());
    OverrideProperty("dalvik.vm.heapmaxfree", heapmaxfree.c_str());
}

void OverrideCarrierProperties() {
    const auto ro_prop_override = [](const char* source, const char* prop, const char* value,
                                     bool product) {
        std::string prop_name = "ro.";

        if (product) prop_name += "product.";
        if (source != nullptr) prop_name += source;
        if (!product) prop_name += "build.";
        prop_name += prop;

        OverrideProperty(prop_name.c_str(), value);
    };

    // Setting carrier prop
    std::string carrier = GetProperty("ro.boot.carrier", "unknown");
    OverrideProperty("ro.carrier", carrier.c_str());

    std::string bootsku = GetProperty("ro.boot.hardware.sku", "");
    if (bootsku == "XT2075-1") {
        /* Motorola One 5G */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "nairo", true);
            ro_prop_override(source, "model", "motorola one 5G", true);
        }
    } else if (bootsku == "XT2075-2") {
        /* Motorola One 5G (AT&T) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "nairo", true);
            ro_prop_override(source, "model", "motorola one 5G", true);
        }
    } else if (bootsku == "XT2075-3") {
        /* Moto G 5G Plus (EURO) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "nairo", true);
            ro_prop_override(source, "model", "moto g 5G plus", true);
        }
    } else if (bootsku == "XT2113-2") {
            std::string carrier = GetProperty("ro.boot.carrier", "");
            if (carrier == "tmo") {
                /* Motorola One 5G Ace (TMO) */
                for (const auto &source : RO_PROP_SOURCES) {
                    OverrideProperty("ro.mot.build.customerid", "t");
                    ro_prop_override(source, "device", "kiev", true);
                    ro_prop_override(source, "fingerprint", "motorola/kiev_t/kiev:11/RZKS31.Q3-45-16-3-11/49e278:user/release-keys", true);
                    ro_prop_override(source, "model", "motorola one 5G ace", true);
                    ro_prop_override(source, "name", "kiev_t", true);
                }
            } else {
                /* Motorola One 5G Ace (RETUS) */
                for (const auto &source : RO_PROP_SOURCES) {
                    ro_prop_override(source, "device", "kiev", true);
                    ro_prop_override(source, "model", "motorola one 5G ace", true);
                }
            }
    } else if (bootsku == "XT2113-3") {
        /* Moto G 5G (Unlocked) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "kiev", true);
            ro_prop_override(source, "model", "moto g 5G", true);
        }
    } else if (bootsku == "XT2113-5") {
        /* Motorola One 5G Ace (AT&T) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "kiev", true);
            ro_prop_override(source, "model", "motorola one 5G ace", true);
        }
    } else if (bootsku == "XT2071-3") {
        /* Motorola Razr 5G (RETUS) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "smith", true);
            ro_prop_override(source, "model", "motorola razr (2020)", true);
        }
    } else if (bootsku == "XT2071-4") {
        /* Motorola Razr 5G (RETEU) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "smith", true);
            ro_prop_override(source, "model", "motorola razr 5G", true);
        }
    } else if (bootsku == "XT2071-5") {
            std::string carrier = GetProperty("ro.boot.carrier", "");
            if (carrier == "tmo") {
                /* Motorola Razr (2020) (TMO) */
                for (const auto &source : RO_PROP_SOURCES) {
                    OverrideProperty("ro.mot.build.customerid", "t");
                    ro_prop_override(source, "device", "smith", true);
                    ro_prop_override(source, "fingerprint", "motorola/smith_t/smith:12/S2PS32M.57-23-31/82a27f:user/release-keys", true);
                    ro_prop_override(source, "model", "motorola razr (2020)", true);
                    ro_prop_override(source, "name", "smith_t", true);
                }
            } else {
                for (const auto &source : RO_PROP_SOURCES) {
                    ro_prop_override(source, "device", "smith", true);
                    ro_prop_override(source, "model", "motorola razr 5G", true);
                }
            }
    } else if (bootsku == "XT2063-3") {
        /* Motorola Edge (2021) */
        for (const auto &source : RO_PROP_SOURCES) {
            ro_prop_override(source, "device", "racer", true);
            ro_prop_override(source, "model", "motorola edge", true);
        }
    }
}

void vendor_load_properties() {
    set_avoid_gfxaccel_config();
    OverrideMemoryProperties();
    OverrideCarrierProperties();
}
