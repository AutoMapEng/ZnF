#pragma once
#include "zfsapi.h"

#define SUPPORT_ZIP
#define SUPPORT_LZ4
//#define SUPPORT_BZ2


/**
 * @brief Decompressor interface
 */
class ZFS_API Decompress
{
public:
    virtual ~Decompress() {}

    /**
     * Decompress a chunk of memory
     *
     * @param bufComp Pointer to compressed memory
     * @param lenComp Length of compressed memory [bytes]
     * @param bufUncomp   Buffer to write uncompressed data to
     * @param availUncomp Length of the target buffer [bytes]
     * @return Number of uncompressed bytes [bytes]
     */
    virtual int decomp(const char* bufComp, int lenComp, char* bufUncomp, int availUncomp) = 0;
};

/**
 * @brief possible decompression algorithms
 */
enum class CompressFormats {
    none = 0,  ///< does nothing
    zip = 1,   ///< zlib algorithm
    bz2 = 2,   ///< bz2 algorithm. Not implemented!
    lz4 = 3,   ///< lz4 algrithm
};

/**
 * @brief Create new decompressor on heap using new
 *
 * @note The caller is responsible to free the allocated memory using delete!
 *
 * @param format Line format info (ZFSHeader-Key 13)
 * @return  nullptr on error or newly allocated decompressor
 */
ZFS_API Decompress *getDecompressor(CompressFormats format);
