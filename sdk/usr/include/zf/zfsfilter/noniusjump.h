#ifndef FILTERDLL_NONIUSJUMP_H
#define FILTERDLL_NONIUSJUMP_H

#include "filter_config.h"
#ifdef ZFSFILTER_NONIUSJUMP

#include "filterapi.h"

#include <string>

class FILTERDLL_API FilterNoniusJumpParam
{
public:
    int minRf;
    int maxRf;
    double band;    // innerhalb dieser schranke muss pixel nach korrektur liegen
    bool jmp2;      // filter pixel which has the double of jump
    double band2;   // innerhalb dieser schranke muss pixel nach korrektur liegen bei jmp2=true

    bool dblPix;    // zwei aufeinander falsche auch korrigieren

    int iter;       // iterations

public:
    FilterNoniusJumpParam()
    {
        minRf = 100;
        maxRf = 10000;
        band = 0.025;
        jmp2 = true;
        band2 = 0.035;
        iter = 5;
        dblPix = false;
    }
};

FILTERDLL_API bool FilterNoniusJump(const char* fn, const char* out, FilterNoniusJumpParam& param, std::string& info);

#endif // ZFSFILTER_NONIUSJUMP
#endif // FILTERDLL_NONIUSJUMP_H
