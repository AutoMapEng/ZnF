#pragma once
#include "zfcore_api.h"

#include <string>


namespace zf
{
	/// give a string like 50sec or 1min 2sec or 1h 3min 30sec (longNames)
	ZFCORE_API
		std::string seconds2string( double seconds, bool longNames=false);

	// date+time as formated string  example: 10:21:19_2023-10-17
	ZFCORE_API
		std::string currentDateTimeAsString();
	ZFCORE_API
		std::string currentTimeDateAsString();
}
