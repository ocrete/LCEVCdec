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

// Unpacking/unescaping raw LCEVC data from NAL units.

#ifndef VN_LCEVC_UTILITY_EXTRACT_H
#define VN_LCEVC_UTILITY_EXTRACT_H

#ifdef __cplusplus
#include <cstdint>
extern "C"
{
#else
#include <stdbool.h>
#include <stdint.h>
#endif

/*!
 * \brief The type of NAL unit encoding
 */
typedef enum LCEVC_CodecType // NOLINT
{
    LCEVC_CodecType_Unknown = 0,
    LCEVC_CodecType_H264 = 1,
    LCEVC_CodecType_H265 = 2,
    LCEVC_CodecType_H266 = 3
} LCEVC_CodecType;

/*!
 * \brief The type of NAL formatting
 */
typedef enum LCEVC_NALFormat // NOLINT
{
    LCEVC_NALFormat_Unknown = 0,
    LCEVC_NALFormat_LengthPrefix = 1,
    LCEVC_NALFormat_AnnexB = 2,
} LCEVC_NALFormat;

/*!
 * \brief Extract LCEVC enhancement data from a buffer containing NAL Units
 *
 * @param[in]       nalData              Pointer to buffer containing NAL units.
 * @param[in]       nalSize              Size in bytes of input NAL data
 * @param[in]       nalFormat            How the NAL units are formatted
 * @param[in]       codecType            What coding standard to use for parsing NAL units
 * @param[out]      enhancementData      Where to put extracted enhancement data
 * @param[in]       enhancementCapacity  Capacity of enhancement data buffer
 * @param[out]      enhancementSize      Pointer to where to write the extracted size - will be 0 if none found
 * @return                               1 = LCEVC found, 0 = no LCEVC found, -1 = an error ocurred
 */
int32_t LCEVC_extractEnhancementFromNAL(const uint8_t* nalData, uint32_t nalSize, LCEVC_NALFormat nalFormat,
                                        LCEVC_CodecType codecType, uint8_t* enhancementData,
                                        uint32_t enhancementCapacity, uint32_t* enhancementSize);

/*!
 * \brief Extract LCEVC enhancement data from a buffer containing NAL Units, and remove extracted data from input buffer.
 *
 * @param[in]       nalData              Pointer to buffer containing NAL units.
 * @param[in]       nalSize              Size in bytes of input NAL data
 * @param[in]       nalFormat            How the NAL units are formatted
 * @param[in]       codecType            What coding standard to use for parsing NAL units
 * @param[out]      nalOutSize           Pointer to where to write the size of the NAL units buffer after the enhancement data is removed
 * @param[out]      enhancementData      Where to put extracted enhancement data
 * @param[in]       enhancementCapacity  Capacity of enhancement data buffer
 * @param[out]      enhancementSize      Pointer to where to write the extracted size - will be 0 if none found
 * @return                               1 = LCEVC found, 0 = no LCEVC found, -1 = an error ocurred
 */
int32_t LCEVC_extractAndRemoveEnhancementFromNAL(uint8_t* nalData, uint32_t nalSize,
                                                 LCEVC_NALFormat nalFormat, LCEVC_CodecType codecType,
                                                 uint32_t* nalOutSize, uint8_t* enhancementData,
                                                 uint32_t enhancementCapacity, uint32_t* enhancementSize);

#ifdef __cplusplus
}
#endif

#endif
