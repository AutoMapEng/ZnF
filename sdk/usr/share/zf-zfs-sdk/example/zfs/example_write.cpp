#include <iostream>

#include <zfs/zfs.h>
#include <zfs/lineheader.h>
#include <zfs/linebyline_smart.h>
#include <zfs/linebyline_wite.h>

#include "example_tools.h"

using namespace zfs;
using namespace std;



/**
\brief This example will read a zfs scan, modify the counter 1 and write it to a new zfs file
*/
bool Sample_ReadWrite(const std::wstring& in, const std::wstring& out)
{
    const int counter_start = 10; // first counter value to set in the data

    if (in.length() == 0) return false;
    if (out.length() == 0) return false;
    if (in == out) return false;

    // open the existing scan
    zfserr_t err{};
    auto lbl = open_linebyline(in, &err, OPEN_DEFAULTFLAGS_LBL | OPEN_UNTIL_LAST | OPEN_NOIMAGE | OPEN_USEMATRIX_320); // optional add OPEN_READ_MASKS
    if (!lbl) return false;


    ZFSHeader newHeader;
    newHeader.Copy(lbl->GetZFSHeader()); // clone header
    // header can be modified here.
    // 23000 to 23013 are the entries for time sync. See zfs.ini
    newHeader.setLong(23001, 1); // frequenz of pulse
    newHeader.SetStringC(23010, "zda");
    newHeader.SetStringC(23002, "120000"); // utc time at counter zero hhmmss
    const uint64_t pctime_counter_start = counter_start * 1000; // [ms]
    newHeader.setLong(23006, pctime_counter_start & 0xffffffff );
    newHeader.setLong(23007, (pctime_counter_start>>32) & 0xffffffff);

    const bool profiler = IsProfiler(&newHeader);
    const int counterbyte = lbl->LineInfo()->counterbyte; // index of counterbyte or zero if not available

    // create a destination scan
    ZFLineByLineWriteSplitted writer;
    writer.create(out, &newHeader);
    if (writer.isOk() == false) return false;
    writer.setSplitSize(250 * 1024 * 1024); // create every 250mb a new zfs file. Files are automatically linked together. The splitting is and can be different from the source

    // go trough all lines
    const uint32_t lines = lbl->Lines();
    const uint32_t pixel = lbl->Pixel();

    // fill in a simulated counter 1 into dataset
    uint32_t simLn = 0; // line counter for simulator
    uint32_t simCnt = counter_start; // current counter value
    uint32_t lastCnt{}; // counter value of last line

    unsigned statisticLines{}; // counts the lines written

    for(uint32_t ln=0; ln<lines; ++ln )
    {
        auto line = lbl->GetLine(); // get the current line for modify and write back

        //
        // modify
        //
        const uint32_t curCnt = simCnt;
        // modify counterbyte of pixel
        if (counterbyte && profiler)
        {
            uint32_t ch = pixel; // the pixel ant which the counter will change
            // to adapt to your needs, please modify following block and let all other as it is
            if ((simLn % 200) == 0)
            {
                ch = curCnt % pixel; // mark the pixel at which the counter should change
                simCnt++;
            }

            // go trough all pixel in the line and modify additional byte
            for (uint32_t px = 0; px < pixel; px++)
            {
                if (lbl->Set(px))
                {
                    uint8_t* add = (uint8_t*)(lbl->L()->PPtr() + counterbyte);
                    const uint32_t cnt = px >= ch ? curCnt + 1 : curCnt;
                    *add = ((*add) & 0xfc) | (cnt & 3); // the least 2 bits are the least two bits of the counter 1
                }
            }
        }
        // modify line header
        LineHeader lh(line->LPtr(), lbl->LineInfo()->lineHeaderSize);
        if (lh.hasCnt1())
        {
            lh.setCnt1(curCnt); // counter value at pixel zero
            // the lineheader has a flag for counter change, set this
            uint16_t flags = lh.flags();
            flags &= ~LH_FLAG_CNT1_CHANGED; // remove flags if there any
            if( curCnt != lastCnt )
                flags |= LH_FLAG_CNT1_CHANGED; // the next line of the change has to be marked
            lh.setFlags(flags);
        }
        lastCnt = curCnt;
        simLn++;
        //
        // end modify
        //


        // write out the current line
        if (writer.write(line) == false)
            break;
        else
            statisticLines++;

        // load next line from source
        if (lbl->LoadLines() < 0) break;
    }
    writer.close();

    cout << "Recording time:" << scanRecordingTime(lbl.get()) << "sec\n";
    cout << "lines read: " << lines << " wrote: " << statisticLines << endl;
    return lines == statisticLines;
}
