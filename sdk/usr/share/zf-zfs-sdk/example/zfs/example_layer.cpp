#include "example_layer.h"

#include <zfs/zfs.h>

#include <zfs/layer_impl.h>
#include <zfs/layer.h>
#include <zfs/layer_tools.h>
#include <zfs/filter/filter_id.h>

#include <zfsfilter/basic/basic.h>
#include <zfsfilter/Matrix.h>

#define ImagesSubDirName L".images_from_layers"


// ====================================================================================================================
// example to copy the mask of the minimum intensity and the mixed pixel (enhanced) filter from
// a given scanfile with full path provided in parameter aScanname to a layer file in the .database directory
// additionally it is checked whether the layer file already exists

bool Sample_MaskToLayer( const std::wstring& aScanname, bool bForce)
{
    bool bSuccess = true;
    
    if(bForce || !zfs::ZFLayer::exist( aScanname, LAYER_NAME_MASK, zfs::filterID2string(zfs::FILTERID_MIXPIX2)) )
        bSuccess = zfs::copyMaskFromScan2Layer( aScanname, zfs::filterID2string(zfs::FILTERID_MIXPIX2), L"" );

    if(bForce || !zfs::ZFLayer::exist( aScanname, LAYER_NAME_MASK, zfs::filterID2string(zfs::FILTERID_RF)) )
        bSuccess = zfs::copyMaskFromScan2Layer( aScanname, zfs::filterID2string(zfs::FILTERID_RF), L"" );

    return bSuccess;
}

// ====================================================================================================================
// . shows the example use, i.e. reading of layers and transfering a layer to an output image in a provided format
// . the available types of layers can be found in the include-file layer_impl.h

bool Sample_ReadLayersAndWriteToImage( const std::wstring& a, const zfs::layer2image_type_t aLayerImageOutType)
{
    bool bSuccess = true;

    zf::FileName Folders( a );
    Folders.InsertPath(ImagesSubDirName);
    Folders.RemoveExt();
    Folders.MkDir();
    std::wstring imageOutputFolder = Folders.WString() + L"/";

    // open a layer and, e.g., copy values to a matrix for further use ...
    const std::wstring nameXYZ(zfs::ZFLayer::buildLayerName(a, LAYER_NAME_XYZ));
    zfs::ZFLayer_XYZ l_xyz;
    if(!l_xyz.open( nameXYZ, zfs::ZFLayer::READONLY ))
        bSuccess= false;

    uint32_t w = l_xyz.getWidth();
    uint32_t h = l_xyz.getHeight();

    zfsfilter::Matrix<double> RangeMatrix( h, w );

    for(uint32_t y = 0; y < h; ++y)
    {
        for(uint32_t x = 0; x < w; ++x)
        {
            zfs::math::Vec3d cur_vector= l_xyz.getVal( x, y );

            RangeMatrix[y][x]= cur_vector.length();
        }
    }

    // open layer and write layer to an
    const std::wstring nameRF(zfs::ZFLayer::buildLayerName(a, LAYER_NAME_RF));
    zfs::ZFLayer_Intensity l_rf;

    if(!l_rf.open( nameRF, zfs::ZFLayer::READONLY ))
        return false;

    if(!zfs::layer2image( nameRF, imageOutputFolder + LAYER_NAME_RF, aLayerImageOutType, true ))
        return false;

    return true;
}

