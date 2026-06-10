#pragma once
#include "zfcore_api.h"


#include <string>
#include <vector>


namespace zf {

	/**
	 * split a string and return a vector of strings
	 * 
	 * example
	 * '' = 0
	 * 'first' = 1
	 * 'first;' = 1
	 * ';' = 0
	 * 'first;second' = 2
	 */
	ZFCORE_API std::vector<std::string> split_string(const std::string& str, const std::string& delimiter="\t;");

} // end namespace

