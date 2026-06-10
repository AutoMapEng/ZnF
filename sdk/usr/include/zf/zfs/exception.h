#pragma once

#include <stdexcept>
#include <string>
#include <cstdint>

namespace zf
{

	/**
	\brief exception used by zf
	*/
	class zf_error : public std::runtime_error
	{
	public:
		zf_error(const std::string& what) : std::runtime_error(what) {}
		zf_error(const std::string& what, int32_t error_code) : std::runtime_error(what), err(error_code) {}

		//! @returns The error code associated with this error
		int32_t error_code() const { return err; }

	private:
        int32_t err = -1; ///< the z+f error code
	};

} // namespace zf
