#pragma once

/*
MSVC++ 4.x  _MSC_VER == 1000 (Visual Studio 4.0?)
MSVC++ 5.0  _MSC_VER == 1100 (Visual Studio 97)
MSVC++ 6.0  _MSC_VER == 1200 (Visual Studio 6.0)
MSVC++ 7.0  _MSC_VER == 1300 (Visual Studio .NET 2002)
MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio .NET 2003)
MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
MSVC++ 14.1 _MSC_VER >= 1910 (Visual Studio 2017)
MSVC++ 16.0 _MSC_VER >= 1920 (Visual Studio 2019)
MSVC++ 17.0 _MSC_VER >= 1930 (Visual Studio 2022)
https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
*/

#if _MSC_VER < 1800
#define override
#endif

#if _MSC_VER < 1700
#define constexpr const
#endif


#pragma warning(disable: 4251) // class "std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>" erfordert eine DLL-Schnittstelle, die von Clients von ...
#pragma warning(disable: 4275) // DLL Schnittstelle class
