#pragma once

#include <stdexcept>
#include <string>

namespace zf
{

	/**
	\brief exception used by zf
	*/
	class runtime_error : public std::runtime_error
	{
	public:
		runtime_error(const std::string& what) : std::runtime_error(what) {}
		runtime_error(const char* what) : std::runtime_error(what) {}
		runtime_error(const std::string& what, int error_code) : std::runtime_error(what), err(error_code) {}
		runtime_error(const const char* what, int error_code) : std::runtime_error(what), err(error_code) {}
		runtime_error( int error_code) : std::runtime_error(), err(error_code) {}

		//! @returns The error code associated with this error
		int error_code() const { return err; }

	private:
        int err = -1; ///< the z+f error code
	};


} // namespace zf
