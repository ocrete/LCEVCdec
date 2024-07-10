/* Copyright (c) V-Nova International Limited 2023-2024. All rights reserved.
 * This software is licensed under the BSD-3-Clause-Clear License.
 * No patent licenses are granted under this license. For enquiries about patent licenses,
 * please contact legal@v-nova.com.
 * The LCEVCdec software is a stand-alone project and is NOT A CONTRIBUTION to any other project.
 * If the software is incorporated into another project, THE TERMS OF THE BSD-3-CLAUSE-CLEAR LICENSE
 * AND THE ADDITIONAL LICENSING INFORMATION CONTAINED IN THIS FILE MUST BE MAINTAINED, AND THE
 * SOFTWARE DOES NOT AND MUST NOT ADOPT THE LICENSE OF THE INCORPORATING PROJECT. ANY ONWARD
 * DISTRIBUTION, WHETHER STAND-ALONE OR AS PART OF ANY OTHER PROJECT, REMAINS SUBJECT TO THE
 * EXCLUSION OF PATENT LICENSES PROVISION OF THE BSD-3-CLAUSE-CLEAR LICENSE. */

// writer for V-Nova internal .bin format.
//
#ifndef VN_LCEVC_UTILITY_BIN_FORMAT_H
#define VN_LCEVC_UTILITY_BIN_FORMAT_H

#include <cstdint>

namespace lcevc_dec::utility {

// LCEVC BIN constants
//
static constexpr char kMagicBytes[8] = {'l', 'c', 'e', 'v', 'c', 'b', 'i', 'n'};
static constexpr uint32_t kVersion = 1;

enum class BlockTypes
{
    LCEVCPayload = 0,
    Extension = 65535,
    Unknown = 65536
};

} // namespace lcevc_dec::utility

#endif
