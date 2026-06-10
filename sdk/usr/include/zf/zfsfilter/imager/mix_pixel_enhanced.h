#pragma once
#include "../filterapi.h"

#include <string>


#if 0
namespace zf
{
	namespace filter
	{
		/*
		*	Enhanced Mixed Pixel Filter
		*	Simple interface
		*/
		
		struct MxPxEnhParams
		{
			enum class strength_t { LOW=1, MID=2, HIGH=3 };
			strength_t strength = strength_t::MID;
			bool smooth{ false };
		};

		FILTERDLL_API
		bool mix_pixel_enhanced(const std::wstring& fn, const MxPxEnhParams& prm );

	}
}
#endif