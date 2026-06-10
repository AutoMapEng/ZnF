#include <iostream>
#include <atomic>
#include <chrono>

#include "example_tools.h"
#include "example_profiler.h"

#include <zftools/file.h>
#include <zfs/lineheader.h>
#include <zfs/linebyline_smart.h>
#include <zfs/Tools/threadpool_scan.h>
#include <zfs/timeapi/timeapi.h>
#include <zfs/filter/mixpix.h>
#include <zfs/filter/filter.h>

using namespace std;


// based on new interface since 9.3
// this can run as multithreaded access as well
// there are a lot of improvements to get faster access. So code is much faster than old one

void Sample_LineByLineAccess_Profiler_V10(const std::wstring& win, const std::wstring& out, bool bSync, int subsample)
{
    const std::wstring filterTemplatePath{ L"c:/filter" }; // path where to find default filter settings (optional)

    const int flags = OPEN_CREATE_TEMP_POS | OPEN_IGNOR_LEVEL | OPEN_IGNOR_COMPENSATOR | OPEN_UNTIL_LAST | OPEN_USEMATRIX_320;
    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto lbl = zfs::open_linebyline(win, &err, flags); // open a zfs scan-file with line by line access
    if (!lbl)
    {
        std::wstring error = zfs::ZFSErrorString(err);
        std::wcout << L"Error:" << error << endl;
        return;
    }

    //
    // PPS Sync if scanner connected to a GPS which support PPS signal
    //  Works with scans in profiler-mode or profiler
    //  PPS signal has to be connected to counter1
    //  on other scans you will get time information as well, based on pc-time which is inaccurate
    //  NEW in 8.5 Nov 2013
    //
    zfs::TimeAPI sync;
    if (bSync)
    {
        // this will take some time. Reads whole scan and do sync. Later on you can get time for each pixel without delay
        zfs::TimeAPI_Params prms; // new interface since 8.7.2
        prms.maxOffset = 3000; // increase allowes time offset between sync start and file start [seconds]
        prms.prm = zfs::TimeAPI_Params::PRM_DEFAULT & ~zfs::TimeAPI_Params::PRM_VERIFY; // without verify (is much faster)
        prms.tryRepair = false; ///< define if code should try to repair to much pulses or ignor it and sync to next valid pulse. Since 10.0. Default has changed
        prms.report = zfs::TimeAPI_Params::REPORT_EVERYTHIN;
        const auto err = sync.Create(lbl.get(),
            prms,
            true    // force recalculation, otherwise it is done just once (default: false)
        );
        std::cout << "sync result:" << err << std::endl;
        std::cout << sync.result().toJson(2) << std::endl; 
    }

    // create the output file
    FILE* fpOut = zftools::fopen(out, L"wt");
    int outLines = 0;
    int outPart = 0;


    zfs::FilterParams filterParams;
    filterParams.buildStandardFilterParams(lbl->GetFirstZFSHeader());
    const auto ini = zfs::filterParameterFile(win);
    if (filterParams.loadFilterParams(ini) == false)// try to load params form a file
        loadFilterParamsFromTemplate(filterTemplatePath, win, filterParams); // if there is not a filer param file for this scan, use a default template if they exist
    // optional change a single value
    // filterParams.setInt(zfs::FilterParam::minRf, 1000); // set minimum intensity  zfs::FilterParam::minRf is the name as the value is stored in the file
    // or load all from file
    //filterParams.loadFilterParams(L"C:filterparams.ini");

    // the callback called for each line which is ready for writing
    atomic_int statisticLines{};
    atomic_int statisticPixel{};
    atomic_int statisticFilteredOut{};
    zfs::PDatasQueue::data_cb_f out_callback = [&](const zfs::PDatas<zfs::PDataDetail>* q, void*, int thid)
    {
        ++statisticLines;
        if (fpOut)
        {
            // go trought all pixel in the line
            for (const auto& px : q->points)
            {
                // the time of the pixel if sync is available
                double seconds = 0.;
                if (bSync)
                {
                    const zfs::ZFTime timePixel = sync.GetTime4Pixel(q->line, px.px);
                    seconds = timePixel.SecondsAbs(); // absolute time depending on UTC (hhmmss) or week-second. See ZFTime time type for more information
                }


                if ((px.mask & zfs::PDataDetail::MASK_FILTERED_OUT) == 0) // write out just not filtered out points
                {
                    // different kind of asc exports 

                    // linenumer, pixelnumber, x,y,z, intensity, pixeltime
                    //fprintf(fpOut, "%d %d %f %f %f %d %.6f\n", q->line, px.px, px.x(), px.y(), px.z(), px.rf, seconds);
                 
                    //fprintf(fpOut, "%f %f %f %d %.6f\n", px.x(), px.y(), px.z(), px.rf, seconds);

                    // ASC output
                    const double x = px.x() + 0.01 * statisticLines; // just to simualte aformward direction
                    fprintf(fpOut, "%d %.3f %f %f %f %d\n", q->line, seconds, x, px.y(), px.z(), (int)(px.RfNorm1() * 4096. - 2047.));
                    outLines++;
                    // create every x pixel a new part output file
                    if (outLines > 10000000)
                    {
                        outLines = 0;
                        outPart ++;
                        fclose(fpOut);
                        {
                            zf::FileName o(out);
                            o.RemoveExt();
                            o += ".";
                            o += std::to_string(outPart);
                            o += ".asc";
                            fpOut = zftools::fopen(o.WString(), L"wt");
                        }
                    }

                    ++statisticPixel;
                }
                else
                {
                    ++statisticFilteredOut;
                    // to keep linesize the same, you can output filtered out pixel here as well
                }
            }
        }
    };

    // go trought the whole scan and acces it linebyline. A callback for output will be called for each line to stream out
    // if multithreaded is activated. The lines are mixed in the out stream
    const bool multithreaded = false;
    const auto report = zfs::zfslinebylineQueue_WithFilter( lbl.get(), std::make_shared<zfs::FilterParams>(filterParams), out_callback, multithreaded?100:0);
    if (fpOut) fclose(fpOut);
    fpOut = nullptr;

    std::cout << "Lines:" << statisticLines << " pixel:" << statisticPixel << " filtered:" << statisticFilteredOut << std::endl;
}



//####################################################################################

/**
\brief Sample for getting time information for each pixel using the SyncAPI
       Old one, but still working
       Run slow compared to the other ones
*/
void Sample_LineByLineAccess_Profiler( const std::wstring& win, const std::wstring& out, bool bSync, int subsample )
{
    const int subsampleLine = subsample;
    const int subsamplePixel = subsample;

    const int flags = OPEN_CREATE_TEMP_POS | OPEN_IGNOR_LEVEL | OPEN_IGNOR_COMPENSATOR | OPEN_USEMATRIX_320;
    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto lbl = zfs::open_linebyline( win, &err, flags ); // open a zfs scan-file with line by line access
    if(!lbl)
    {
        std::wstring error = zfs::ZFSErrorString( err );
        std::wcout << L"Error:" << error << endl;
        return;

    }
    const int px = lbl->Pixel(); // number of pixel each line
    const int ln = lbl->Lines(); // number of lines (number of lines of all parts)

    const int minrf = lbl->MinRf(); // minimum usable intensity

    //
    // PPS Sync if scanner connected to a GPS which support PPS signal
    //  Works with scans in profiler-mode or profiler
    //  PPS signal has to be connected to counter1
    //  on other scans you will get time information as well, based on pc-time which is inaccurate
    //  NEW in 8.5 Nov 2013
    //
    zfs::TimeAPI sync;
    if(bSync)
    {
        // this will take some time. Reads whole scan and do sync. Later on you can get time for each pixel without delay
        zfs::TimeAPI_Params prms; // new interface since 8.7.2
        prms.maxOffset = 10000; // increase allowes time offset between sync start and file start [seconds]
        prms.prm = zfs::TimeAPI_Params::PRM_DEFAULT & ~zfs::TimeAPI_Params::PRM_VERIFY; // without verify (is fater)
        prms.tryRepair = false; ///< define if code should try to repair to much pulses or ignor it and sync to next valid pulse. Since 10.0. Default has changed
        const auto err = sync.Create( lbl.get(),
                                                prms,
                                                true    // force recalculation, otherwise it is done just once (default: false)
        );
        std::cout << "sync result:" << err << std::endl;
        std::cout << sync.result().toJson(2) << std::endl;
    }

    zfs::FilterParams filterParams(lbl->GetFirstZFSHeader());
    // optional change a single value
    // filterParams.setInt(zfs::FilterParam::minRf, 1000); // set minimum intensity  zfs::FilterParam::minRf is the name as the value is stored in the file
    // or load all from file
    //filterParams.loadFilterParams(L"C:filterparams.ini");
    ZFSFilter zf;
    filterParams.setup(zf);
    lbl->FilterDefine(zf); // set all parameter now

    FILE* fpOut = zftools::fopen(out.c_str(), L"wt" );

    int statisticPixel{};
    int statisticValid{};

    // all lines
    lbl->SkipTo( 0 ); // start with first line
    for(int nl = 0; nl < ln; nl += subsampleLine) // go through all lines
    {
        lbl->Filter();

        const auto val23002 = lbl->GetFirstZFSHeader()->getDouble(23002,0.); // get a value from zfs-header for example utc time. See zfi.ini for description of possible values.

        // New in 8.5  Sync API
        int nValid = px;
        if(bSync)
        {
            zfs::ZFLineSyncInfo* syncLine = sync.GetLineSyncInfo( nl );
            if(syncLine)
            {
                //cout << syncLine->Dump(sync) << endl;
                nValid = syncLine->nValid; // valid points in this line
            }
            else
                continue;
        }

        // access lineheader
        zfs::LineHeader lineheader(lbl->L()->LPtr(), lbl->LineInfo()->lineHeaderSize);
        // read odometry value for actual line
        const uint32_t odometerFirstPixel = lineheader.encHorz();
        // read out time added by the firmware while prozessing data
        const auto time_ms = lineheader.time();

        // go through all pixels in current line
        for(int i = 0; i < px; i += subsamplePixel)
        {
            if (lbl->Set(i))              // set pointer to pixel
            {
                statisticPixel++;

                // set odometry value of actual pixel
                const int32_t offsetToFirstPixel = lbl->L()->EncL();
                const int32_t odometer = (int32_t)odometerFirstPixel + offsetToFirstPixel;

                // New in 8.5  Sync API
                // get time for this pixel
                double seconds = 0.;
                if (bSync)
                {
                    const zfs::ZFTime timePixel = sync.GetTime4Pixel(nl, i);
                    seconds = timePixel.SecondsAbs(); // absolute time depending on UTC (hhmmss) or week-second. See ZFTime time type for more information
                }

#ifdef USE_SCAN_MASKS
                if (lbl->IsSetMask(i))// If you want check masks stored in scan as well, you have to open scan with flag OPEN_READ_MASKS. Scans from the device does not have a mask stored.
#endif
                {
                    if (lbl->IsZeroPixel() == false && lbl->IsSet(i))   // Check if pixel is enabled. Filter function will disable bad points
                    {
                        const int rf = lbl->Rf();
                        if (rf > minrf)        // check for minimum intensity. This is the most important filter
                        {
                            // XYZ registered and calibrated
                            const zfs::math::Vec3d vec = lbl->XYZ(); // unit=m  xyz global/registered coordinates
                            if (fpOut) fprintf(fpOut, "%f %f %f %d %f\n", vec.x(), vec.y(), vec.z(), rf, seconds);

                            statisticValid++;
                        }
                    }
                }
            }
        }

        lbl->LoadLines( subsampleLine ); // load next line. Use 1 for loading every line
    }

    if(fpOut)
        fclose( fpOut );

    cout << "Recording time:" << scanRecordingTime(lbl.get()) << "sec\n";
    cout << "Pixel:" << (px*ln) << " processed:" << statisticPixel << " filtered-out:" << (statisticPixel-statisticValid) << endl;
}

//##############################################################################################################################

// this implements a multithreaded access trought the zfs file
// multiple parts are read
// the output is not in a linear order
void Sample_LineByLineAccess_Profiler_MT(const std::wstring& win, bool bSync)
{
    const int flags = OPEN_CREATE_TEMP_POS | OPEN_IGNOR_LEVEL | OPEN_IGNOR_COMPENSATOR | OPEN_USEMATRIX_320;
    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto lbl = zfs::open_linebyline(win, &err, flags); // open a zfs scan-file with line by line access
    if (!lbl)
    {
        const std::wstring error = zfs::ZFSErrorString(err);
        std::wcout << L"Error:" << error << endl;
        return;

    }
    const int px = lbl->Pixel(); // number of pixel each line
    const int ln = lbl->Lines(); // number of lines (number of lines of all parts)

    const int minrf = lbl->MinRf(); // minimum usable intensity

                                  //
                                  // PPS Sync if scanner connected to a GPS which support PPS signal
                                  //  Works with scans in profiler-mode or profiler
                                  //  PPS signal has to be connected to counter1
                                  //  on other scans you will get time information as well, based on pc-time which is inaccurate
                                  //  NEW in 8.5 Nov 2013
                                  //
    zfs::TimeAPI sync;
    if (bSync)
    {
        // this will take some time. Reads whole scan and do sync. Later on you can get time for each pixel without delay
        zfs::TimeAPI_Params prms; // new interface since 8.7.2
        prms.prm = zfs::TimeAPI_Params::PRM_DEFAULT & ~zfs::TimeAPI_Params::PRM_VERIFY; // without verify (is fater)
        prms.tryRepair = false; ///< define if code should try to repair to much pulses or ignor it and sync to next valid pulse. Since 10.0. Default has changed
        const auto err = sync.Create(lbl.get(),
            prms,
            false    // force recalculation, otherwise it is done just once. (default: false)
        );
        std::cout << "sync result:" << err << std::endl;
        std::cout << sync.result().toJson(2) << std::endl;
    }

    ZFSFilter filterParams(lbl->GetFirstZFSHeader()); // set default filter parameter depending on scanner
    lbl->FilterDefine(filterParams); // set all parameter now



    int statisticPixel{};
    int statisticValid{};
    std::mutex mut;

    // print out progress
    std::atomic<int> progress{};
    std::thread prog([&]()
    {
        int last = 0;
        while (progress >= 0 && ln)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (progress>=0 && progress != last)
                std::cout << (progress * 100 / ln) << " percent line: " << progress << " of: " << ln << std::endl;
            last = progress;
        }
    });

    const auto counterbyte = lbl->LineInfo()->counterbyte; // zero if there is no counterbyte

    // all lines in multithreaded
    zf::ThreadPoolScan pool(ln, std::thread::hardware_concurrency(), lbl.get(),
        [&](ZFSLinebyline* clone, int tfrom, int tto, int tidx)
    {
        int nPixel{};
        int nValid{};
        for (int l = tfrom; l < tto; l++) // go through all lines
        {
            progress++;
            clone->Filter();

            zfs::LineHeader lineheader(clone->L()->LPtr(), clone->LineInfo()->lineHeaderSize);
            const uint32_t odometerFirstPixel = lineheader.encHorz(); // the counter value

            for (int i = 0; i < px; i++)
            {
                nPixel++;
                if (clone->IsSet(i))   // Check if pixel is enabled. Filter function will disable bad points
                {
                    if (clone->Set(i))
                    {

                        double seconds = 0.; // pixel time from synchronisation
                        if (bSync)
                        {
                            const zfs::ZFTime timePixel = sync.GetTime4Pixel(l, i);
                            seconds = timePixel.SecondsAbs(); // absolute time depending on UTC (hhmmss) or week-second. See ZFTime time type for more information
                        }

                        const int rf = clone->Rf();
                        if (rf > minrf)        // check for minimum intensity. This is the most important filter
                        {
                            const zfs::math::Vec3d vec = clone->XYZ();

                            // access to counterbyte of counter 1
                            uint8_t add = 0;
                            if (counterbyte)
                            {
                                add = *(uint8_t*)(clone->L()->LPtr() + counterbyte);
                                uint8_t cnt1_low_bits = add & 3;
                            }

                            // the x value maybe close to zero, but you sould use it
                            nValid++;
                        }
                    }
                }
            }
            if (clone->LoadLines() < 0) break; // load next line.
        }
        std::unique_lock<std::mutex> lk(mut);
        statisticPixel += nPixel;
        statisticValid += nValid;
    });
    progress = -1;
    prog.join();

    cout << "Recording time:" << scanRecordingTime(lbl.get()) << "sec\n";
    cout << "Pixel:" << (px*ln) << " processed:" << statisticPixel << " filtered-out:" << (statisticPixel - statisticValid) << endl;
}


