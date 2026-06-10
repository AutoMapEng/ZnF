#pragma once

#include <string>
#include "zftools_api.h"


//Nur für windows
class ZFTOOLS_API CheckFreeSpace
{
public:
	CheckFreeSpace(void){};
	~CheckFreeSpace(void){};

	static float getFreeSpace( const std::string& drive );  ///< Returns the free space in kilobyte
    static float getFreeSpace( const std::wstring& drive ); 
    static float getFreeSpaceMByte( const std::wstring& drive);
    //ok Rueckgabewert ob die Abfrage erfolgreich war. Bei Netzlaufwerken kann die groesse eventuell nicht abgefragt werden
	static bool checkFreeSpaceKilo( const std::string& drive, float size, bool ok );
    static bool checkFreeSpaceKilo( const std::wstring& drive, float size, bool ok );
	static bool checkFreeSpaceByte( const std::string& drive, float size );
    static bool checkFreeSpaceByte( const std::wstring& drive, float size );
	static bool checkFreeSpacePercent( const std::string& drive, int& freePercent, int* freeKB=NULL );

protected:
	static std::wstring getDriveName( const std::wstring& drive );
    static std::wstring toWString( const std::string& string);

};
