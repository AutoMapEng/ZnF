#pragma once


namespace zfs
{
    /** 
    \brief  Track calculation for Profiler
            
            The order and meaning is the same as defined in the zfs-header index 31
            Do not change order
    */
    enum way_t {
        WAY_UNDEF = -1,
        WAY_NO = 0,						///< standard / default. No additional calculation
        WAY_CONST,						///< constant value set value v
        WAY_ENC23,						///< use low-speed encoder
        WAY_CNT1, WAY_CNT2, WAY_CNT3,	///< Counter 1..3
        WAY_IMG_NO,                     ///<
        WAY_ENC32,						///< use low-spped encoder 32 bit
        WAY_CNT1_32BIT, WAY_CNT2_32BIT,  ///<

        _WAY_MAX_
    };
} // end namespace zfs
