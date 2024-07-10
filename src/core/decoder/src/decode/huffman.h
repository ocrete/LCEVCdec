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

#ifndef VN_DEC_CORE_HUFFMAN_H_
#define VN_DEC_CORE_HUFFMAN_H_

#include "common/bitstream.h"
#include "common/types.h"

/* These must add up to VN_BIG_TABLE_MAX_SIZE*/
#define VN_BIG_TABLE_LEADING_ZEROES_BITS 4
#define VN_BIG_TABLE_MAX_CODE_SIZE 8

/* (1 << VN_BIG_TABLE_LEADING_ZEROES_BITS) - 1 */
#define VN_BIG_TABLE_MAX_NUM_LEADING_ZEROES 15
/* VN_BIG_TABLE_MAX_NUM_LEADING_ZEROES + VN_BIG_TABLE_MAX_CODE_SIZE*/
#define VN_BIG_TABLE_CODE_SIZE_TO_READ 23

/* (1<<VN_BIG_TABLE_MAX_SIZE) - 1 */
#define VN_BIG_HUFFMAN_IDX_MASK 4095

/* (1<<VN_BIG_TABLE_MAX_CODE_SIZE) - 1 */
#define VN_BIG_HUFFMAN_CODE_MASK 255

/* (1<<VN_SMALL_TABLE_MAX_SIZE) - 1 */
#define VN_SMALL_HUFFMAN_CODE_MASK 1023

#define VN_BIG_TABLE_MAX_SIZE 12
#define VN_SMALL_TABLE_MAX_SIZE 10

/* VN_MAX_NUM_SYMBOLS is a feature of the stream: for sparse symbols, 5 bits store the symbol
 * count (so, at most 32 symbols). For dense symbols, there's a presence bitmap of size 256. */
#define VN_MAX_NUM_SYMBOLS 256

/* This is a property of the stream, and therefore guaranteed. However, in practice, none of the
 * streams in our test coverage exceed 16 bits as the maximum code length. */
#define VN_MAX_CODE_LENGTH 31

/*------------------------------------------------------------------------------*/

typedef struct Logger* Logger_t;

/*------------------------------------------------------------------------------*/

/*! \brief Huffman types. These are all used to encode the coefficients (e.g. the 16 elements of a
 *         DDS-transformed square) which are later get "reverse hadamard transformed" into plain
 *         old residuals. For example, a 48x32 image would have 3x2=6 DDS squares, so you'd have
 *         16 huffman streams, each providing 6 coefficients. */
enum
{
    HuffLSB, /* 6 least-significant bits of the coefficients. */
    HuffMSB, /* The remaining most-significant bits. Often zero. */
    HuffRL,  /* The run-length of zeros following the last decoded symbol. */
    HuffCount
};

static inline bool nextSymbolIsMSB(uint8_t symbol) { return (symbol & 0x01); }
static inline bool nextSymbolIsRL(uint8_t symbol) { return (symbol & 0x80); }

/*------------------------------------------------------------------------------*/

typedef struct HuffmanStream
{
    ByteStream_t byteStream;
    int32_t word;
    uint8_t wordStartBit;
    uint8_t wordEndBit;
    uint64_t bitsRead;
} HuffmanStream_t;

/*------------------------------------------------------------------------------*/

/* Huffman list is used for codes when they don't fit in a look-up table. Note that, even though
 * the maximum code size is 31, we only need a uint8_t for code. That's because it's not possible
 * to get a code with a value higher than 255 when the max num symbols is 256: codes are counted up
 * incrementally, so you can't get a higher value than maxNumSymbols. */
typedef struct HuffmanListEntry
{
    uint8_t code;
    uint8_t symbol;
    uint8_t bits;
} HuffmanListEntry_t;

/* sorted by code length (increasing), then by code itself (decreasing) */
typedef struct HuffmanList
{
    HuffmanListEntry_t list[VN_MAX_NUM_SYMBOLS];
    uint8_t idxOfEachBitSize[VN_MAX_CODE_LENGTH + 1]; /* the index at which each bit-size ends */
    uint8_t size;
} HuffmanList_t;

/*------------------------------------------------------------------------------*/

typedef struct HuffmanEntry
{
    uint8_t symbol;
    uint8_t bits;
} HuffmanEntry_t;

typedef struct HuffmanTable
{
    HuffmanEntry_t code[1 << VN_SMALL_TABLE_MAX_SIZE];
} HuffmanTable_t;

/*------------------------------------------------------------------------------*/

/* New huffman triple-table: lookup table contains a list of triples.
 * lsb is the least-significant bit of the coefficient (a coefficient being thing which gets
 *  transformed to make a residual).
 * rl is the run-length, derived from strictly 0 to 2 codes. Note: we could have made this handle
 *  any number of subsequent run-lengths (up to 4 are possible). However, such sequences (3 or 4
 *  run-lengths, with a total code length less than 12) are vanishingly rare. Meanwhile,
 *  accommodating them imposes costs.
 * contents works like this:
 * { uint5_t bitsTotal, [blank x 1], bool msbOverflowed, bool rlOverflowed}
 *  - bitsTotal is a number from 1 to 31 (or 0 for invalid) indicating the combined number of bits
 *    taken up (in the stream) by the codes for all parts of the HuffmanTriplet which are present
 *  - the overflows tell us which symbol (if any) was unable to fit on this triple. Note that we
 *    never store the msb in this table, so even a 1bit msb code is "overflow". Note also that we
 *    don't need an "lsbOverflowed" bit: this is indicated by having 0 bitsTotal.
 */
typedef struct HuffmanTriple
{
    uint8_t contents;
    uint8_t lsb;
    uint16_t rl;
} HuffmanTriple_t;

typedef struct HuffmanTripleTable
{
    HuffmanTriple_t code[1 << VN_BIG_TABLE_MAX_SIZE];
} HuffmanTripleTable_t;

/*! \brief Huffman "manual" decoder state. This is used when look-up tables are insufficient. */
typedef struct HuffmanManualDecodeState
{
    HuffmanList_t list;
    uint8_t singleSymbol;  /**< When we only have one symbol in the alphabet, this is it */
    uint8_t minCodeLength; /**< The smallest code word has this length */
    uint8_t maxCodeLength; /**< The longest code word has this length */
} HuffmanManualDecodeState_t;

/*! \brief huffman decoder state */
typedef struct HuffmanTripleDecodeState
{
    HuffmanTripleTable_t tripleTable; /**< TripleTable, for short triplets */
    HuffmanTable_t rlTable;           /**< Fallback lookup-table for Run-lengths */
    HuffmanManualDecodeState_t manualStates[HuffCount]; /**< Individual decoders, as a double-fallback */
} HuffmanTripleDecodeState_t;

/*! \brief Initialize a triple huffman decoder
 *
 *  \param log     Context's logger
 *  \param state   Triple-decoder to initialize
 *  \param stream  Huffman stream to read initialization from
 *
 *  \return 0 on success, otherwise -1 */
int32_t huffmanTripleInitialize(Logger_t log, HuffmanTripleDecodeState_t* state,
                                HuffmanStream_t* stream, bool useOldCodeLengths);

/*! \brief Decode the next several huffman symbols
 *
 *  \param state    Triple-decoder to decode with
 *  \param stream   Huffman stream to read codes from
 *  \param valueOut Output coefficient (lsb, possibly with msb)
 *
 *  \return run-length, or -1 for error */
int32_t huffmanTripleDecode(const HuffmanTripleDecodeState_t* state, HuffmanStream_t* stream,
                            int16_t* valueOut);

/*------------------------------------------------------------------------------*/

/*! \brief Initialize a HuffmanStream_t
 *
 *  \param stream  Huffman stream to initialise
 *  \param data    pointer to start of data, so that Huffman stream points there.
 *  \param size    size of data, in bytes.
 *
 *  \return 0 on success, otherwise -1 */
int32_t huffmanStreamInitialise(HuffmanStream_t* stream, const uint8_t* data, size_t size);

/*! \brief Get number of remaining bits on the HuffmanStream_t */
static inline size_t huffmanStreamGetRemainingBits(const HuffmanStream_t* stream)
{
    const size_t wordBitsRemaining = 32 - stream->wordEndBit;
    const size_t byteBitsRemaining = bytestreamRemaining(&stream->byteStream) * 8;
    return wordBitsRemaining + byteBitsRemaining;
}

/*! \brief Advance the Huffman stream BY a certain number of bits. Can be used directly, if you
 *         know you don't yet have enough bits between wordStartBit and wordEndBit, or indirectly
 * through huffmanStreamAdvanceToNthBit, if you're not sure whether or not you have enough bits.
 *
 *  \param stream  Huffman stream to advance.
 *  \param bits    The number of bits by which you want to advance the stream. That is, the number
 *                 of bits that you want to append into the [wordStartBit, wordEndBit) window, not
 * knowing whether they're already in word, or need to come off of byteStream. */
static inline void huffmanStreamAdvanceByNBits(HuffmanStream_t* stream, uint8_t bits)
{
    /* Can only read, at most, 25 bits (if word is 32 bits). If we try to read more, we risk
     * pushing wordStartBit below zero, because we read 8 bits at a time. */
    assert(bits <= (8 * (sizeof(stream->word)) - 7));

    stream->wordEndBit += bits;
    stream->bitsRead += bits;

    if (stream->wordEndBit > (8 * sizeof(stream->word))) {
        /* If wordEndBit is past the end, then shuffle data in from the right to the left, 8 bits at
         * a time, until wordStartBit is as far left as possible. This helps us access the
         * byteStream as rarely as possible. */
        while (stream->wordStartBit > 7) {
            stream->word <<= 8;
            if (stream->byteStream.offset < stream->byteStream.size) {
                stream->word |= *(stream->byteStream.data + stream->byteStream.offset);
                stream->byteStream.offset++;
            }
            stream->wordStartBit -= 8;
            stream->wordEndBit -= 8;
        }
    }
}

/*! \brief Advance the Huffman stream UNTIL you have at least desiredNumBits between wordStartBit and
 *         wordEndBit. Good if you've already grabbed a bunch of bits, so you may or may not already
 *         have enough bits. Also good if you're not sure how many bits you need, since this won't
 *         push wordStartBit forwards.
 *
 *  \param stream          Huffman stream to advance.
 *  \param desiredNumBits  The minimum number of bits that you want to have.
 *
 *  \return desiredNumBits of data, from wordStartBit forwards. */
static inline uint32_t huffmanStreamAdvanceToNthBit(HuffmanStream_t* stream, uint8_t desiredNumBits)
{
    const uint8_t usableBits = stream->wordEndBit - stream->wordStartBit;
    /* Top up the number of bits and check the rlTable */
    uint8_t endBit = stream->wordStartBit + desiredNumBits;
    if (usableBits < desiredNumBits) {
        huffmanStreamAdvanceByNBits(stream, desiredNumBits - usableBits);
        endBit = stream->wordEndBit;
    }
    return extractBits(stream->word, stream->wordStartBit, endBit);
}

/*! \brief Read and immediately consume data via a HuffmanStream_t, with bounds-checking. This is
 *         not suitable for typical Huffman decoding. Rather, this is best when (1) you know the
 *         size of the data that you want to read, (2) you're not too concerned about performance
 *         (e.g. during initialisation), and (3) there's nothing in [wordStartBit, wordEndBit) to read.
 *
 *  \param stream   Huffman stream to read bits from.
 *  \param numBits  The minimum number of bits that you want to read.
 *  \param out      The resulting data.
 *
 *  \return 0 if read was within bounds, 0 otherwise. */

/* A wrapper for when you want to read and immediately consume data. Note that you SHOULD NOT!!!
 * use this for huffman decoding. HuffmanStream_t is designed so that you read a lot of bits at
 * once, and then gradually use those bits. Nonetheless, this is convenient in less performance-
 * sensitive areas, e.g. initialisation. */
static inline int32_t huffmanStreamReadBits(HuffmanStream_t* stream, uint8_t numBits, uint32_t* out)
{
    assert(stream && out && (stream->wordEndBit == stream->wordStartBit));
    if (huffmanStreamGetRemainingBits(stream) < numBits) {
        return -1;
    }
    *out = huffmanStreamAdvanceToNthBit(stream, numBits);
    stream->wordStartBit += numBits;
    return 0;
}

/*------------------------------------------------------------------------------*/

/*! \brief Huffman single-decoder. Used for temporal, and for size-parsing decoders. Unlike the
 *         triple-decoder, this decoder only decodes one symbol at a time (using a look-up table
 *         and a sorted list as a fallback). */
typedef struct HuffmanSingleDecoder
{
    HuffmanManualDecodeState_t manualState;
    HuffmanTable_t table;
} HuffmanSingleDecoder_t;

/*! \brief Initialize both parts of a huffman single-decoder.
 *
 *  \param log    Context's logger
 *  \param state  The manual-decoder part of the single-decoder to initialize
 *  \param table  The lookup-table part of the single-decoder to initialize
 *  \param stream HuffmanStream to read initialization from
 *
 *  \return 0 on success, otherwise -1 */
int16_t huffmanManualInitialiseWithLut(Logger_t log, HuffmanManualDecodeState_t* state,
                                       HuffmanTable_t* table, HuffmanStream_t* stream,
                                       bool useOldCodeLengths);

/*! \brief Decode the next huffman symbol using a look-up table
 *
 *  \param rlTable   Lookup-table to look in for the code seen in the stream.
 *  \param stream    HuffmanStream to decode from
 *  \param symbolOut Decoded symbol
 *
 *  \return 0 on success, otherwise -1 */
int32_t huffmanLutDecode(const HuffmanTable_t* rlTable, HuffmanStream_t* stream, uint8_t* symbolOut);

/*! \brief Decode the next huffman symbol using a manual-decoder (i.e. a sorted list)
 *
 *  \param state     Manual-decoder to use.
 *  \param stream    HuffmanStream to decode from
 *  \param symbolOut Decoded symbol
 *
 *  \return 0 on success, otherwise -1 */
int32_t huffmanManualDecode(const HuffmanManualDecodeState_t* state, HuffmanStream_t* stream,
                            uint8_t* symbolOut);

/*! \brief Get the single-symbol associated with the manual-decoder, if it's a single-symbol decoder.
 *
 *  \param state     Manual-decoder to use.
 *  \param symbolOut Decoded symbol
 *
 *  \return true if this decoder is a single-symbol decoder, otherwise false */
bool huffmanGetSingleSymbol(const HuffmanManualDecodeState_t* state, uint8_t* symbolOut);

/*------------------------------------------------------------------------------*/

#endif /* VN_DEC_CORE_HUFFMAN_H_ */