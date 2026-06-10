#pragma once

#include "ZfsPrjBasic.h"


class QString;

/**
*   Repariert die Projektdatei
*   - GPS flag: Korrektur des Fehlerhaften Zeichens nach dem Flag. Bug 5522 
*/
void ZFSPRJ_API repairProject( const QString& filename );

