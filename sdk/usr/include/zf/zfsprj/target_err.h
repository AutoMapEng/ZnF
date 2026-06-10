#pragma once

enum errTarget_t
{
    ERR_TARGET_OK,
    ERR_TARGET_FORMATNOTSUPPORTED,
    ERR_TARGET_EXIST,       //!< Target already exists
    ERR_TARGET_NOT_EXIST,   //!< Target not found
    ERR_TARGET_TOMUCH,
    ERR_TARGET_PARAM,
    ERR_TARGET_WRITEFILE,
    ERR_TARGET_HEADERSIZE,
    ERR_TARGET_PARENT,		//!< can't write target to parent scan as well, mayby name changed
    ERR_TARGET_UNKNOWN,
    ERR_TARGET_SCANNOTFOUND,    ///< scan, where target should be added, not found in the projekt
    ERR_TARGET_NAN,             ///< one ore more of the target valuse (x/y/z) are nan 
    ERR_TARGET_INTERNAL,
    ERR_TARGET_NAME,        //target name is empty

    _ERR_TARGET_
}; //!< errorcode addTarget
