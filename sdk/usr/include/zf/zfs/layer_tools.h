#pragma once
#include "zfsapi.h"

#include <string>
#include <vector>
#include <cstdint>
#include "Math/rgb.h"

class ZFScan;

namespace zfs
{
    class ZFLayer;

    /**
    \brief use the layer as a standard mask
    */
    ZFS_API bool copyLayerToMaskOfScan( ZFLayer&, ZFScan*,                         const std::wstring& maskName, zfs::math::rgb col = zfs::math::rgb(64, 64, 64), bool invert = false);
    ZFS_API bool copyLayerToMaskOfScan( ZFLayer&, const std::wstring& zfsFileName, const std::wstring& maskName, zfs::math::rgb col = zfs::math::rgb(64, 64, 64), bool invert = false);

    /**
    @brief copy the mask from a scan into a separate layer file
    @param zfsFile the scan
    @param maskSrcName the name of the mask from the scan
    @param layer_dest_name the name of the layerfile
    @return true if sucessfull
    */
    ZFS_API bool copyMaskFromScan2Layer(const std::wstring& zfsFile, const std::wstring& maskSrcName, const std::wstring& layer_dest_name);

    ZFS_API std::vector<std::wstring> exportMasks2Layer(const std::wstring& zfsFile);

    /**
     * @brief convert layer to image for visualisation
     *
     * @param zfl Path to layer file
     * @param imageFilename Where to store the resulting PNG image
     * @param imageType see L2I_...
     * @param flip whether image should be rotated / mapped. false = physical view
     * @param zfsFile required for flip image
     * @param param specifies image scaling
     * @return true if successfull
     */
    enum class layer2image_type_t: uint32_t 
    {
        L2I_PNG,
        L2I_JPG,
        L2I_TIFF,
    };
    ZFS_API bool layer2image( const std::wstring& srcZfl, const std::wstring& destImageFileName, layer2image_type_t imageType, bool flip=false, const std::wstring& zfsFile=L"", const std::wstring& param=L"" );

    // info about layer
    ZFS_API std::string layerInfo(const std::wstring& srcZfl, const std::string& cr="\n" );

    /**
     * @brief convert layer to png for visualisation
     *
     * @param zfl Path to layer file
     * @return true if successfull
     */
    ZFS_API bool layer2png(const std::wstring& srcZfl);


    /**
     * \brief create layer files for each part of a scan.
     */
    ZFS_API int create_layers_mask(const std::wstring& zfs, const std::wstring& layer_name = L"mask", bool fillExisting = false);

}
