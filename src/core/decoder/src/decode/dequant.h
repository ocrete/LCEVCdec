/* Copyright (c) V-Nova International Limited 2022-2024. All rights reserved.
 * This software is licensed under the BSD-3-Clause-Clear License.
 * No patent licenses are granted under this license. For enquiries about patent licenses,
 * please contact legal@v-nova.com.
 * The LCEVCdec software is a stand-alone project and is NOT A CONTRIBUTION to any other project.
 * If the software is incorporated into another project, THE TERMS OF THE BSD-3-CLAUSE-CLEAR LICENSE
 * AND THE ADDITIONAL LICENSING INFORMATION CONTAINED IN THIS FILE MUST BE MAINTAINED, AND THE
 * SOFTWARE DOES NOT AND MUST NOT ADOPT THE LICENSE OF THE INCORPORATING PROJECT. ANY ONWARD
 * DISTRIBUTION, WHETHER STAND-ALONE OR AS PART OF ANY OTHER PROJECT, REMAINS SUBJECT TO THE
 * EXCLUSION OF PATENT LICENSES PROVISION OF THE BSD-3-CLAUSE-CLEAR LICENSE. */

#ifndef VN_DEC_CORE_DEQUANT_H_
#define VN_DEC_CORE_DEQUANT_H_

#include "common/simd.h"
#include "common/types.h"

/*------------------------------------------------------------------------------*/

typedef struct QuantMatrix
{
    uint8_t values[LOQEnhancedCount][RCLayerCountDDS];
} QuantMatrix_t;

/*! \brief Restore the supplied quant-matrix to the standard defined default values. */
void quantMatrixSetDefault(QuantMatrix_t* matrix, ScalingMode_t loq0Scaling, TransformType_t transform);

/*! \brief Copies the LOQ-0 quant matrix to LOQ-1. */
void quantMatrixDuplicateLOQs(QuantMatrix_t* matrix);

/*! \brief Retrieve a pointer to the quant-matrix for LOQ-0 based upon the scaling
 *         mode used. */
static inline uint8_t* quantMatrixGetValues(QuantMatrix_t* matrix, LOQIndex_t index)
{
    if (!matrix || ((uint32_t)index >= LOQEnhancedCount)) {
        return NULL;
    }
    return matrix->values[index];
}

/*------------------------------------------------------------------------------*/

/*! \brief Contains dequantization settings for a single plane and LOQ. Must be aligned to 16bit
 *         boundaries, or else SSE generates a segfault */
#pragma pack(push, 16)
typedef struct Dequant
{
    int16_t stepWidth[TSCount][RCLayerCountDDS]; /**< Step-width per-temporal type per-layer. */
    int16_t offset[TSCount][RCLayerCountDDS];    /**< Offset per-temporal type per-layer. */

#if VN_CORE_FEATURE(SSE)
    __m128i stepWidthVector[TSCount][2]; /**< Step-widths packed into SIMD vector, maximum of 16-values. */
    __m128i offsetVector[TSCount][2]; /**< Offsets packed into SIMD vector, maximum of 16-values. */
#elif VN_CORE_FEATURE(NEON)
    int16x8_t stepWidthVector[TSCount][2];
    int16x8_t offsetVector[TSCount][2];
#endif
} Dequant_t;
#pragma pack(pop)

/*! \brief Containing dequantization settings for all planes and LOQs. */
typedef struct DequantParams
{
    Dequant_t values[LOQEnhancedCount][RCMaxPlanes];
} DequantParams_t;

/*------------------------------------------------------------------------------*/

typedef struct DeserialisedData DeserialisedData_t;

typedef struct DequantArgs
{
    uint32_t planeCount;
    uint32_t layerCount;
    DequantOffsetMode_t dequantOffsetMode;
    int32_t dequantOffset;
    bool temporalEnabled;
    bool temporalRefresh;
    uint8_t temporalStepWidthModifier;
    uint32_t stepWidth[LOQEnhancedCount];
    uint8_t chromaStepWidthMultiplier;
    QuantMatrix_t* quantMatrix;
} DequantArgs_t;

/*! \brief Copies a deserialised data struct to a dequant args struct.
 *
 * \param data     Input: an initialised DeserialisedData struct.
 * \param args     Ouput: an uninitialised DequantArgs struct.
 *
 * \return 0 on success, otherwise -1
 */
int32_t initialiseDequantArgs(const DeserialisedData_t* data, DequantArgs_t* args);

/*! \brief Calculates dequantization parameters to be used during decoding.
 *
 * \param params   The location to store the dequantization settings.
 * \param args     The arguments that control the dequantization calculations.
 *
 * \return 0 on success, otherwise -1
 */
int32_t dequantCalculate(DequantParams_t* params, const DequantArgs_t* args);

/*------------------------------------------------------------------------------*/

#endif /* VN_DEC_CORE_DEQUANT_H_ */