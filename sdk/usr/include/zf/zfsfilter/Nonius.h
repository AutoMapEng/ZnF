#ifndef FILTERDLL_NONIUS_CALC_H
#define FILTERDLL_NONIUS_CALC_H

#include "filter_config.h"
#ifdef ZFSFILTER_NONIUS

#include "filterapi.h"


class ZFSHeader;


#define MAX_LUT 65536

class FILTERDLL_API Nonius
{
public:
    int rawoffcosc;
    int rawoffsinc;
    int rawoffcosf;
    int rawoffsinf;

    double resolution;
    int offsetmm;
    int noniusfineparam;
    int noniuscoarseparam;
    int noniusfinetemp;
    int noniuscoarsetemp;
    int noniusfine;
    int noniuscoarse;

    double div;
    int nlut;
    int lut[MAX_LUT];
    double noniusjump;

    int statCoarse;
    int statAll;
    int statOver;

protected:
    int mm2inc(double mm);
    double Pythagoras(double cos, double sin);
    int inc2mm(int inc);

public:
    Nonius();

    void mm2CosSinFandC(int rg, int rf, double& cosf, double& sinf, double& cosc, double& sinc, int rgc);

    int errVec(int* mm, int* rf, int ocosf = 0, int osinf = 0, int ocosc = 0, int osinc = 0, int rgc = 0);

    int LoadLUT(const char*);

    void ParamFromHeader(ZFSHeader* h);

    void mm2cossinOpt(double mm, double rf, int& cosc, int& sinc, int& cosf, int& sinf, int& coarse);
    void mm2cossin(double mm, double rf, int& cosf, int& sinf, int& coarse);
    void mm2cossin(double mm, double rf, int& cosc, int& sinc, int& cosf, int& sinf, int& coarse);
    int mm2cossin(double mm, double rf, double& cosc, double& sinc, int& cosf, int& sinf, int& coarse, int rgc);
    void mm2cossinCoarse(double mm, double rf, int& cosc, int& sinc, int& cosf, int& sinf, int& coarse);
    double cossin2mm(int cosf, int sinf, int coarse);
    double cossin2mmCoarse(int cosc, int sinc, int coarse);
    double cossin2mm(int cosc, int sinc, int cosf, int sinf, int coarse);
    double cossin2mmRgc(double cosc, double sinc, int cosf, int sinf, int coarse, int rgc, bool bOverride = false);
    int cossin2rf(int cosf, int sinf);

    bool CheckInterval(double cosc, double sinc, int cosf, int sinf, int& coarse, int erg);
    int GetDiff(double cosc, double sinc, int cosf, int sinf);


    void AddVec(double& m, double& rf, int cosf, int sinf);
    void AddVec(double& m, double& rf, int cosc, int sinc, int cosf, int sinf);
    void AddVec(double& m, double& rf, int cosc, int sinc, int cosf, int sinf, int rgc);

    void clear();
};

class CSinCos
{
    double sinSum;
    double cosSum;
    long count;

public:
    CSinCos() { sinSum = 0; cosSum = 0; count = 0; };
    void Reset() { sinSum = 0; cosSum = 0; count = 0; };
    void AddSinCos(double s, double c) { sinSum += s; cosSum += c; count++; };
    double GetSin() { return sinSum / count; }
    double GetCos() { return cosSum / count; }
    long GetCount() { return count; }
};

extern  CSinCos scHimmel;

#endif // ZFSFILTER_NONIUS
#endif // FILTERDLL_NONIUS_CALC_H
