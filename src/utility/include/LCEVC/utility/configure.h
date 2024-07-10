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

// Generate LCEVC_DEC API LCEVC_Configure... calls from json
//
#ifndef VN_LCEVC_UTILITY_CONFIGURE_H
#define VN_LCEVC_UTILITY_CONFIGURE_H

#include "LCEVC/lcevc_dec.h"

#include <string_view>

namespace lcevc_dec::utility {

/*!
 * \brief Extract configuration from JSON string or file and apply it to a decoder.
 *
 * If 'json' starts with '[', it is treated as a JSON string, otherwise it is treated
 * as a filename.
 *
 * @param[in]       decoder 		Decoder to be configured
 * @param[in]       json            JSON string of filename
 * @return 					        LCEVC_Success
 *                  				LCEVC_NotFound
 *                  				LCEVC_Error
 */
LCEVC_ReturnCode configureDecoderFromJson(LCEVC_DecoderHandle decoder, std::string_view json);

} // namespace lcevc_dec::utility

#endif
