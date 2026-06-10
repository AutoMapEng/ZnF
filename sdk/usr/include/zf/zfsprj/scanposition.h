#ifndef ZFPRJ_SCANPOSITION_H
#define ZFPRJ_SCANPOSITION_H
#include "ZfsPrjBasic.h"

#include <string>
#include <vector>

#include "groups.h"

namespace zfprj
{

    /**
        Holds information about the scanpositions ( viewpoints called in zfprj file)
    */
    class ScanPosition
    {
        std::wstring m_name;                ///< name of the scanposition (stored in the zfs file as well)
        double m_matReg[16];                   ///< the registrated position
        double m_matPre[16];                   ///< the registrated position
        int m_flagsReg;                 ///< flags der registrierung
        int m_flagsPre;                 ///< flags der preregistrierung
        double m_qualityPre;            ///< qualität der preregistrierung
        std::vector<std::wstring> m_scans;  ///< the scans which belongs to the scanposition
        SPGroup m_group;                ///< gruppe zu der der scan gehoert

    public:

        ScanPosition() { m_flagsReg=0; m_flagsPre=0; m_qualityPre=0.; }
        void AddScan( const std::wstring& s ) { m_scans.push_back(s); }
    };



} // end namespace

#endif // ZFPRJ_SCANPOSITION_H
