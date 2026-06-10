#pragma once
#include "zfxyzapi.h"
#include <stdint.h>



namespace zfxyz
{
    typedef float pixel_xyz_t;


#define ZFXYZ_SIZE_MAGICNUMBER  8               ///< size for magic number
#define ZFXYZ_MAGIC_NUMBER      "ZFXYZ002"      ///< file magic number
#define ZFXYZ_EXT               ".zfxyz"        ///< file extension

    /**
     * @brief
     *
     * The structure the application will get the line of xyz pixels
     */
#pragma pack(1)
    struct XYZPixelOut
    {
        pixel_xyz_t x, y, z;	///< meter (localy)
        uint32_t rf;   ///< intensity [increments]
        unsigned char add;	    ///< additional byte. See advanced manual
        unsigned char mask;     ///< see MASK_...  0=pixel valid
    };
#pragma pack()

    /**
    @brief the struct for storing data to disk as a .xyz file
    */
#pragma pack(1)
    struct XYZPixelIn
    {
        pixel_xyz_t x, y, z;	///< meter (localy)
        uint32_t rf;   ///< intensity [increments]
        unsigned char add;	    ///< additional byte. See advanced manual
        unsigned char mask;     ///< see MASK_...  0=pixel valid
        float rg;               ///< range [meter]
    };
#pragma pack()

    /// flags for mask
    #define MSK_FILTERED_OUT            (1<<0)
    #define MSK_FILTERED_OUT_BY_USER    (1<<1)
    #define MSK_SELECTED_OUT            (1<<2)




    /**
     * @brief Correction Vector for a single vertical encoder increment
     *
     * Needed to calculate the calibrated data from the data stream
     */
    #pragma pack(1)
    struct CorrectVect
    {
        float x, y, z;      ///< Correction values for the three dimensions
    };
    #pragma pack()


    enum FileOptsEnum {
        OPT_HAS_LUT = 1 << 0  ///< zfs header is followed by LUT
    };

} // namespace

