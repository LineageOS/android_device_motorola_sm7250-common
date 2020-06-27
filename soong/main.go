package lito

import (
    "android/soong/android"
)

func init() {
    android.RegisterModuleType("motorola_lito_init_library_static", initLibraryFactory)
}
