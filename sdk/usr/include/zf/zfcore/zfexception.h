#pragma once

#include <exception>


namespace zf
{
	class ZFException : public std::exception
	{
		int m_errorcode{};

	public:
		ZFException(int errorcode) : m_errorcode(errorcode), std::exception("zf_exception") {}
		ZFException(const std::string& s) : m_errorcode(1), std::exception(s.c_str()) {}
		ZFException(int errorcode, const std::string& s) : m_errorcode(errorcode), std::exception(s.c_str()) {}

		int error() const { return m_errorcode; }
	};
}
