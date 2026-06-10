#ifndef ZFSPRJ_XML_H
#define ZFSPRJ_XML_H


#ifdef _WINDOWS
// #include "../zftools/zftools.h"
#include "../zftools/quickxml2.h"
#include <zfcore/tinyxml2.h>
#endif

#ifdef LINUX
#include "../Common/zfstring.h"
#include "../zftools/quickxml2.h"
#include <zfcore/tinyxml2.h>
#endif

#endif // XML_H
