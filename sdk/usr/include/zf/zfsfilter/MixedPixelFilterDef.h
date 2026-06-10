#pragma once

#include <cfloat>
#include <cstdint>

#include "basic/basic.h"

#include <zfs/Math/math/tuple.h>

// mask names for experimental filters

// subdirectory name for filtered data set
#define FILTERFOLDER L"Filter"

namespace zfsfilter {
    enum class FilterStrengthOptionType : uint8_t
    {
        FILTER_STRENGTH_UNCHOOSEN = 0,
        FILTER_STRENGTH_LOW = 1,
        FILTER_STRENGTH_MIDDLE = 2,
        FILTER_STRENGTH_HIGH = 3
    };

#define      NOOFFILTERSTRENGTHOPTIONS 4
    const double FilterStrengthValues[NOOFFILTERSTRENGTHOPTIONS] = { -1.,0.0,0.50,1.0 };

    class FilterStrengthOption
    {
    public:
        // -- constructors
        FilterStrengthOption() :
            mFilterStrengthOption(FilterStrengthOptionType::FILTER_STRENGTH_UNCHOOSEN),
            mNoOfFilterStrengthOptions(NOOFFILTERSTRENGTHOPTIONS - 1) {}

        FilterStrengthOption(FilterStrengthOptionType aFilterStrengthOption) :
            mFilterStrengthOption(aFilterStrengthOption),
            mNoOfFilterStrengthOptions(NOOFFILTERSTRENGTHOPTIONS - 1) {}

        FilterStrengthOption(double                   aValue) :
            mFilterStrengthOption(FilterStrengthOptionType::FILTER_STRENGTH_UNCHOOSEN),
            mNoOfFilterStrengthOptions(NOOFFILTERSTRENGTHOPTIONS - 1) {
            Set(aValue);
        }

        // -- operators
        FilterStrengthOption& operator=(FilterStrengthOptionType aFilterStrengthOption)
        {
            Set(aFilterStrengthOption);
            return *this;
        }

        // -- getter
        int NoOfFilterStrengthOptions() { return mNoOfFilterStrengthOptions; }
        double Value() // main function <-> conversion
        {
            switch (mFilterStrengthOption)
            {
            case FilterStrengthOptionType::FILTER_STRENGTH_LOW: return FilterStrengthValues[1];
            case FilterStrengthOptionType::FILTER_STRENGTH_MIDDLE: return FilterStrengthValues[2];
            case FilterStrengthOptionType::FILTER_STRENGTH_HIGH: return FilterStrengthValues[3];
            default: break;
            }
            return FilterStrengthValues[2];
        }

        int Idx() { return (int)mFilterStrengthOption - 1; }

        FilterStrengthOptionType Selection() { return mFilterStrengthOption; }

        // -- setter
        void Set(FilterStrengthOptionType aFilterStrengthOption) { mFilterStrengthOption = aFilterStrengthOption; }

        // LOW: 0 ... 0.33; MIDDLE: 0.33 ... 0.66; HIGH: 0.66 ... 1.0; DEFAULT/ELSE: MIDDLE
        void Set(double aValue)
        {
            mFilterStrengthOption = FilterStrengthOptionType::FILTER_STRENGTH_MIDDLE; // default
            if (aValue >= 0.00 && aValue <= 1. / 3) mFilterStrengthOption = FilterStrengthOptionType::FILTER_STRENGTH_LOW;
            if (aValue > 2. / 3 && aValue <= 1.00) mFilterStrengthOption = FilterStrengthOptionType::FILTER_STRENGTH_HIGH;
        }
    private:
        FilterStrengthOptionType mFilterStrengthOption;
        int                      mNoOfFilterStrengthOptions;
    };


    enum class ProcessingDirectionType:uint8_t {
        PROC_DIR_UNDEF,
        PROC_DIR_HORIZONTAL,
        PROC_DIR_VERTICAL,
        PROC_DIR_DIAGONAL45,
        PROC_DIR_DIAGONAL135
    };

    enum class ProcessingDirOptionsType:uint8_t {
        PROC_DIR_OPT_UNCHOOSEN = 0,
        PROC_DIR_OPT_PROFILE = 1,
        PROC_DIR_OPT_PROFILE_AND_X_PROFILE = 2,
        PROC_DIR_OPT_ALL_DIRECTIONS = 3
    };

    // ---------------------------------------------------------------------------------------------------------------------
    template<typename T> struct ProcessingParameter
    {
        zfs::math::Tuple2<int> n_smooth_filter_sizes; // (row,column)-direction
        int32_t             flags{ 0 };
        int32_t             ndT{ 0 };
        int32_t             mean_meas_width{ 0 };
        int32_t             line_fit_width{ 0 };
        int32_t             single_pix_filter_lowest_nn{ 0 };

        T                   normal_threshold_angle{ 0 };
                            
        T                   dT{ 0 };
        T                   std_mul{ 0 };
                            
        T                   sharpness_factors[3]{ 0 };           // the higher the more pixels are filtered out
        T                   q_stddev_values[3]{ 0 };             // the lower  the more pixels are filtered out
                                                                 // fixed values to guarantee good filtering
        T                   max_rel_i_change_values[3]{ 0 };     // the higher the less pixels are filtered out
        T                   normal_threshold_angles[3]{ 0 };     // the closer to 90.0° the less are filtered out
//        int32_t             single_pix_filter_lowest_nns[3]{ 0 };// the higher the more pixels are filtered out
                            
        T                   q_stddev{ 0 };         // user specific choice
        T                   q_stddev_factor{ 0 }; // user specific choice
        T                   max_rel_i_change{ 0 }; // user specific choice
    };
    // ---------------------------------------------------------------------------------------------------------------------

} // end namespace zfsfilter
