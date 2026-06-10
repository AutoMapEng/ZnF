#pragma once
#include <string>
#include <utility>
#include "zftools_api.h"


#include <string>

class ZFTOOLS_API ScantraBasic
{
public:
	ScantraBasic() {}
	virtual ~ScantraBasic() {}

	/// Eine Kopie des Projektfiles für Scantra anlegen
	static std::wstring copyPrj4Scantra( const std::wstring& projFile_ );

    /**
    *   Erstellt den namen der Rueckgabedatei fuer die Namenszuweisung von Punkten.
    */
    static std::wstring createResultFilePoints( const std::wstring& scantraProjFile );

	// give the registration item and key
	static std::pair<std::string, std::wstring> getScantra_LocalMachineKey();

	/**
	*   Gibt den Installationspfad von Scantra zurueck.
	*/
	static std::wstring getScantraStartPath();

	/// create filename for scantra projectfile
	static std::wstring Prj4Scantra( const std::wstring& projFile_ );

    static std::wstring getScantraDBNameW( const std::wstring& projFileName, bool useScantra = true );
};
