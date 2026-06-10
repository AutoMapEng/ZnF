#pragma once
#include "../zfsapi.h"

#include <string>
#include <vector>

#include "../Math/rgb.h"

namespace zfs
{

    // filter id's
    // the filteralgorithm will mark from which filter the pixel was filtered out
    typedef enum : uint8_t {
        FILTERID_NOTFILTERED = 0,

        FILTERID_BASIC = 1, ///< filtered out but not further spezified
        FILTERID_ONEINALL, ///< standard filter used. all filterresults in one mask
        FILTERID_MANUAL,   ///< manually filtered and result in one mask

        FILTERID_RF = 10, // minimum intensity
        FILTERID_OVER, //11
        FILTERID_RG,//12
        FILTERID_MIXPIX,//13
        FILTERID_SINGLE,//14
        FILTERID_JUMP,//15
        FILTERID_NONIUS,//16
        FILTERID_EDGE,//17
        FILTERID_SMALEOBJECT,//18
        FILTERID_COMPENSATOR,//19
        FILTERID_BADLINE,//20
        FILTERID_RFRG,//21
        FILTERID_FIRMMIXPIX_1,
        FILTERID_FIRMMIXPIX_2,
        FILTERID_FIRMMIXPIX_3,
        FILTERID_FIRM_NONIUS,
        FILTERID_SPIKE,
        FILTERID_THIN,
        FILTERID_MASK, // by user action
        FILTERID_VERTINVALID,
        FILTERID_RFMAX,

        // spezial
        FILTERID_ROADSIGN, ///< unused
        FILTERID_INFRARED, ///< pixel which has a infrared value
        FILTERID_SKY,
        FILTERID_GROUND,
        FILTERID_MIRROR,
        FILTERID_AMBIGUE,
        FILTERID_TEST,
        FILTERID_XYZ_IN,
        FILTERID_XYZ_OUT,
        FILTERID_MIXPIX_LN,
        FILTERID_RFNORM, /// filter based on normalized intensity

        // slower advanced filter
        FILTERID_MIXPIX2 = 100, ///< enhanced mixed pixel
        FILTERID_SPREAD,
        FILTERID_VOLUME,        ///< 3d Volume filter, obsolete, should be FILTERID_XYZ_IN or FILTERID_XYZ_OUT
        FILTERID_USERSELECT,
        FILTERID_ZFDB_SEL,  ///< selection made in zfdb
        FILTERID_PLANES,
        FILTERID_ENLARGE,

        FILTERID_EXPERIMENTAL
    } LINEMASK_ID_TYPE, filterID_t;


    // return a list of all filter ID's
    ZFS_API std::vector<std::pair< filterID_t, std::wstring>> filterIDs();

    // parse string and return id. If not found, return FILTERID_NOTFILTERED
    uint8_t ZFS_API filterString2ID( const std::wstring& nameOfFilter);

    /// gives a string reprensentation of the type of mask stored in the line
    std::wstring ZFS_API filterID2string(uint8_t t);
    std::wstring ZFS_API filterID2short(uint8_t t);
    inline std::wstring linemasktype2string(uint8_t id) { return filterID2string(id);  }
    inline std::wstring linemasktype2string_short(uint8_t id) { return filterID2short(id); }

    /// return the predefined color for this filter id
    math::rgb ZFS_API filterID2color(uint8_t);
    inline math::rgb linemasktype2color(uint8_t id) { return filterID2color(id); }



    /// setup the default table. Will be set on start of library
    void ZFS_API initFilterColors();

    /// set a new/different color for filter id
    void ZFS_API setFilterColor(uint8_t id, const math::rgb& rgb);
    int ZFS_API setFilterColors(const std::string& str );
    std::string ZFS_API getFilterColors();

}
