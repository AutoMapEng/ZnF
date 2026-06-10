#include <iostream>

#include <zfs/zfs.h>
#include <zfs/lineheader.h>
#include <zfs/linebyline.h>
#include <zfs/linebyline_smart.h>
#include <iostream>     // std::cout, std::ios
#include <sstream>      // std::ostringstream

#include <zftools/file.h>

#include "example_lineheader.h"

using namespace zfs;
using namespace std;



/**
\brief This example read the lineheader of each file and export it
*/
bool Sample_ExportLineheader(const std::wstring& in, const std::wstring& out)
{
    if (in.length() == 0) return false;
    if (out.length() == 0) return false;
    if (in == out) return false;

    // open the existing scan
    zfserr_t err{};
    auto lbl = open_linebyline(in, &err, OPEN_DEFAULTFLAGS_LBL | OPEN_UNTIL_LAST | OPEN_NOIMAGE | OPEN_USEMATRIX_320); // optional add OPEN_READ_MASKS
    if (!lbl) return false;


    FILE* fpOut = zftools::fopen(out, L"wt");
    if (!fpOut) return false;

    // help
    fprintf(fpOut, "# f=flags r=reference-range t=time l=linenumber u=user x=level-x y=level-y 1=counter-1 2=counter-2 a=gnss-lat o=gnss-long\n");

    // go trough all lines
    const uint32_t lines = lbl->Lines();
    int linesOut{};
    for (uint32_t ln = 0; ln < lines; ++ln)
    {
        std::ostringstream o;

        zfs::LineHeader lineheader(lbl->L()->LPtr(), lbl->LineInfo()->lineHeaderSize);

        // print out the typically lineheader items if they are available

        if (lineheader.hasFlags())
            o << "f:" << lineheader.flags() << " ";
        if (lineheader.hasRefRg())
            o << "r:" << lineheader.refRg() << " ";
        if (lineheader.hasTime())
            o << "t:" << lineheader.time() << " ";
        if (lineheader.hasLinenumber())
            o << "l:" << lineheader.linenumber() << " ";
        if (lineheader.hasUser())
            o << "u:" << lineheader.user() << " ";
        if (lineheader.hasLevel())
            o << "x:" << lineheader.levelX()  << " y:" << lineheader.levelY() << " ";
        if (lineheader.hasCnt1())
            o << "1:" << lineheader.cnt1() << " ";
        if (lineheader.hasCnt2())
            o << "2:" << lineheader.cnt2() << " ";
        if (lineheader.hasGps())
            o << "a:" << lineheader.gpsLat() << " o:" << lineheader.gpsLong() << " ";

        fprintf(fpOut, "%s\n", o.str().c_str());
        ++linesOut;

        // load next line from source
        if (lbl->LoadLines() < 0) break;
    }
    fclose(fpOut);

    std::cout << "exported lineheader of:" << linesOut << " lines" << std::endl;
    return true;
}
