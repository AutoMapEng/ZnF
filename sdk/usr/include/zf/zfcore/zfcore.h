#pragma once
#include "zfcore_api.h"

#include <string>

/**
* Basic core Z+F Library for common stuff
*/

#include "compiler.h"
#include "trace.h"
#include "filename.h"
#include "unicode.h"
#include "zfmath.h"

namespace zf
{
	ZFCORE_API
	std::string getCoreVersion();
	
}
