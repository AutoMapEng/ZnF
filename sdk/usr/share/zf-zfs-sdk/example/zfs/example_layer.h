#pragma once

#include <string>

#include <zfs/layer_tools.h>


bool Sample_MaskToLayer( const std::wstring& aScanname, bool bForce );

bool Sample_ReadLayersAndWriteToImage( const std::wstring& a, const zfs::layer2image_type_t aLayerImageOutType );
