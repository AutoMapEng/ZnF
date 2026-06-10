#pragma once

#include <string>

#include <zfs/zfs.h>  // the most importand includefile


class ZFSLinebyline;


//========================================================================================================================

// Very fast access to read header of zfs file without opening the whole scan
void SampleReadHeader( const std::wstring& zfsfile );
//========================================================================================================================

// sample how to change/add item to zfs-header
bool SampleChangeHeader( const std::wstring& zfsfile );
//========================================================================================================================

// Calculates dimensions of scan
bool DetectDim( ZFSLinebyline* zf );
//========================================================================================================================

/**
\brief the recording time of the scan
\return seconds
*/
double scanRecordingTime(ZFSLinebyline* lbl);
