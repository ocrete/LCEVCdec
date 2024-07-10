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

// LCEVC type utilities:
//  - to/from strings
//
#ifndef VN_LCEVC_TYPES_CONVERT_H
#define VN_LCEVC_TYPES_CONVERT_H

#include "LCEVC/lcevc_dec.h"

#include <string_view>

namespace lcevc_dec::utility {

// Decoder API enums
//
std::string_view toString(LCEVC_ReturnCode returnCode);
bool fromString(std::string_view str, LCEVC_ReturnCode& out);

std::string_view toString(LCEVC_ColorRange colorRange);
bool fromString(std::string_view str, LCEVC_ColorRange& out);

std::string_view toString(LCEVC_ColorPrimaries colorPrimaries);
bool fromString(std::string_view str, LCEVC_ColorPrimaries& out);

std::string_view toString(LCEVC_TransferCharacteristics transferCharacteristics);
bool fromString(std::string_view str, LCEVC_TransferCharacteristics& out);

std::string_view toString(LCEVC_PictureFlag pictureFlag);
bool fromString(std::string_view str, LCEVC_PictureFlag& out);

std::string_view toString(LCEVC_ColorFormat colorFormat);
bool fromString(std::string_view str, LCEVC_ColorFormat& out);

std::string_view toString(LCEVC_Access access);
bool fromString(std::string_view str, LCEVC_Access& out);

std::string_view toString(LCEVC_Event event);
bool fromString(std::string_view str, LCEVC_Event& out);

} // namespace lcevc_dec::utility

#endif
