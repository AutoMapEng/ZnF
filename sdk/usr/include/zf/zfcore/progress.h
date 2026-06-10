#pragma once

#include <string>
#include <functional>



namespace zf
{
	/**
	* \brief progress callback support
	*/
	typedef std::function< void(int percent, const std::string& info) > progress_f;


	/**
	* \brief a progress callback which can be canceled
	* \return true for continue or false for abort
	*/
	typedef std::function< bool(int percent, const std::string& info) > cancel_progress_f;

	/**
	* \brief function just for tracing logging informations
	*/
	typedef std::function< void( const std::string& msg) > trace_f;
}
