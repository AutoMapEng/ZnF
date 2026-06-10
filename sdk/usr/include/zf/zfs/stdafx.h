// stdafx.h : Includedatei für Standardsystem-Includedateien,
//  oder projektspezifische Includedateien, die häufig benutzt, aber
// in unregelmäßigen Abständen geändert werden.
//

#pragma once

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif

#ifdef WIN64
#define WIN64_LEAN_AND_MEAN		// Selten verwendete Teile der Windows-Header nicht einbinden
#endif


// Windows-Headerdateien:
#ifdef WINDOWS
#include <windows.h>
#endif

// TODO: Verweisen Sie hier auf zusätzliche Header, die Ihr Programm erfordert
