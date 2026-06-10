#ifndef _ZFPRJ_LOG_H_
#define _ZFPRJ_LOG_H_

#include "ZfsPrjBasic.h"

#include <string>

class ZFProject;
class QString;

namespace zfprj
{

	/**
	@brief Logfile Einruecken fuer einen Block
	*/
	class ZFSPRJ_API LogIndent
	{
		int m_spaces;
		int m_old;
		ZFProject& m_prj;
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
		std::string m_inf;
		std::string m_threadID;
#pragma warning(pop)

	public:
		LogIndent(ZFProject& p, const char* info = NULL, int spaces = 2);
		LogIndent(ZFProject& p, const QString&, int spaces = 2);
		virtual ~LogIndent();

    private:
        LogIndent& operator=( const LogIndent&){return (*this);}
	};

} // end namespace

#endif // _ZFPRJ_GROUPS_H_
