#pragma once
#ifndef ZFPRJ_INTERFACE
#define ZFPRJ_INTERFACE

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ZfsPrjBasic.h"


//###################################################################################################

/**
* @brief open a zfproject. Project has to be closed afterwards
* @param absProjectFileName the file
* @return a handle of the project
*/
ZFPRJ_API int64_t zfprjOpenProject(const wchar_t* absProjectFileName);

/**
* @brief clone the project and free memory
* @param handle the handle of the zfprjOpenProject function
*/
ZFPRJ_API void zfprjCloseProject(uint64_t handle);



//###################################################################################################

/**
* @brief a class for handling a zfproject file
*/
class ZFProjectInterface
{
public:
	ZFProjectInterface(const wchar_t* absProjectFileName)
	{
		handle = zfprjOpenProject(absProjectFileName);
	}

	virtual ~ZFProjectInterface()
	{
		zfprjCloseProject(handle);
	}

	int64_t handle{};
};


#ifdef __cplusplus
} // extern "C"
#endif

#endif// ZFPRJ_INTERFACE
