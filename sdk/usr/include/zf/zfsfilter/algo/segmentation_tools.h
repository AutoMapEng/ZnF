#pragma once

#include <zfsfilter/filterapi.h>

#include <string>
#include <vector>
#include <tuple>

#include <Common/inttypes_.h>

#include <zfs/Math/zfvec.h>

#include <zfs/Math/2d/point.h>

#define SEGMENTATION_LABEL_SEGMENT_INFO  L"segment_info"
#define SEGMENTATION_SEGMENT_INFO_LEXT   L".asc"

namespace zfsfilter
{
    namespace algo
    {
        // #############################################################
        typedef std::tuple< uint32_t, uint32_t, zfs::math::Vec3d, zfs::math::Vec3f> segment_info_t; // <segment_id, segment size, segment's mean normal vector>
        typedef std::vector<segment_info_t> segments_info_t;
        // #############################################################
        FILTERDLL_API std::wstring buildSegmentsInfoFileName( const std::wstring& zfsAbsFile, const std::wstring& layerName, const std::wstring& subName = L"" );
        FILTERDLL_API bool WriteBaseSegmentsInfo( const std::wstring& out, const segments_info_t& segments_info, const int last_field=2 );
        FILTERDLL_API bool WriteExtendedSegmentsInfo( const std::wstring& out, const segments_info_t& segments_info );
        FILTERDLL_API bool ReadBaseSegmentsInfo( const std::wstring& in , segments_info_t& segments_info );
        FILTERDLL_API bool ReadExtendedSegmentsInfo( const std::wstring& in , segments_info_t& segments_info );
        FILTERDLL_API void PointGroupDataListtoSegmentsInfo( const zfs::math::PointGroupDataListType& aPointGroupDataList, segments_info_t& segments_info);
        // #############################################################
        FILTERDLL_API bool sort_segments_info(uint32_t field, segments_info_t& segments_info, const std::string& sort_direction, std::vector<int>& relabel_table);
        // #############################################################
        FILTERDLL_API std::wstring extract_normal_name(const std::wstring& a);
        // #############################################################
    }
}