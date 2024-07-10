# Copyright (c) V-Nova International Limited 2022-2024. All rights reserved.
# This software is licensed under the BSD-3-Clause-Clear License.
# No patent licenses are granted under this license. For enquiries about patent licenses,
# please contact legal@v-nova.com.
# The LCEVCdec software is a stand-alone project and is NOT A CONTRIBUTION to any other project.
# If the software is incorporated into another project, THE TERMS OF THE BSD-3-CLAUSE-CLEAR LICENSE
# AND THE ADDITIONAL LICENSING INFORMATION CONTAINED IN THIS FILE MUST BE MAINTAINED, AND THE
# SOFTWARE DOES NOT AND MUST NOT ADOPT THE LICENSE OF THE INCORPORATING PROJECT. ANY ONWARD
# DISTRIBUTION, WHETHER STAND-ALONE OR AS PART OF ANY OTHER PROJECT, REMAINS SUBJECT TO THE
# EXCLUSION OF PATENT LICENSES PROVISION OF THE BSD-3-CLAUSE-CLEAR LICENSE.

# Use NDK provided toolchain with LCEVC default settings.
#

set(ANDROID_PLATFORM
    21
    CACHE STRING "Android API Level")
set(ANDROID_ABI
    armeabi-v7a
    CACHE STRING "Android ABI")
set(ANDROID_ARM_NEON TRUE)

# Look for packages in the build directory as well
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE "BOTH")

include($ENV{ANDROID_NDK}/build/cmake/android.toolchain.cmake)
