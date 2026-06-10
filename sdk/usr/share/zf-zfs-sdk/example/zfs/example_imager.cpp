#include "example_imager.h"

#include <iostream>
#include <fstream>

#include <zftools/file.h>
#include <zfs/level/level_register.h>
#include <zfs/filter/mixpix.h>
#include <zfs/way.h>
#include <zfs/lineheader.h>
#include <zfs/level/compensator.h>
#include <zfs/filter/filter.h>
#include <zfs/linebyline_smart.h>
#include <zfs/xyz.h>

#include "example_tools.h"

using namespace std;




void Sample_LineByLineAccess_Imager_V10(const std::wstring& win, const std::wstring& out, int subsample)
{
    const std::wstring filterTemplatePath{ L"c:/filter" }; // path where to find default filter settings (optional)

    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto lbl = zfs::open_linebyline(win, &err, OPEN_DEFAULTFLAGS_ZFSCAN | OPEN_USEMATRIX_320); // open a zfs scan-file with line by line access
    if (!lbl)
    {
        std::wstring error = zfs::ZFSErrorString(err);
        std::wcout << L"Error:" << error << endl;
        return;
    }


    // create the output file
    FILE* fpOut = zftools::fopen(out, L"wt");


    zfs::FilterParams filterParams;
    filterParams.buildStandardFilterParams(lbl->GetFirstZFSHeader());
    const auto ini = zfs::filterParameterFile(win); 
    if (filterParams.loadFilterParams(ini) == false)// try to load params form a file  
        loadFilterParamsFromTemplate(filterTemplatePath, win, filterParams); // if there is not a filer param file for this scan, use a default template if they exist
    // optional change a single value
    // filterParams.setInt(zfs::FilterParam::minRf, 1000); // set minimum intensity  zfs::FilterParam::minRf is the name as the value is stored in the file
    // or load all from file
    //filterParams.loadFilterParams(L"C:/filterparams.ini");

    // the callback called for each line which is ready for writing
    zfs::PDatasQueue::data_cb_f out_callback = [&](const zfs::PDatas<zfs::PDataDetail>* q, void*, int thid)
    {
        if (fpOut)
        {
            // go trought all pixel in the line
            for (const auto& px : q->points)
            {
                if ((px.mask & zfs::PDataDetail::MASK_FILTERED_OUT) == 0) // write out just not filtered out points
                {
                    // PTS
                    fprintf(fpOut, "%f %f %f %d\n", px.x(), px.y(), px.z(), (int)(px.RfNorm1() * 4096. - 2047.));

                    // example to show the index for example in the _color.jpg image
                    std::pair<int32_t, int32_t> im;
                    if (lbl->physical2image({ px.px,q->line }, im))
                    {
                    }
                }
            }
        }
    };

    // go trought the whole scan and acces it linebyline. A callback for output will be called for each line to stream out
    // if multithreaded is activated. The lines are mixed in the out stream
    zfs::FilterExecution exec;
    const auto report = zfs::zfslinebylineQueue_WithFilter(lbl.get(), std::make_shared<zfs::FilterParams>(filterParams), out_callback, 0);
    if (fpOut) fclose(fpOut);
    fpOut = nullptr;
}




//========================================================================================================================


/*
    this 2. sample shows how to open a large file and access just only one line
    only 3 lines are hold in memory, so you can open very large scans

    if scan exist of more than one file, it will open the next file automatically
*/


/**
    Sample_LineByLineAccess_minimalCode: Line by line => using a roll buffer, handling 3 lines in the memory to apply filtering.
    !!!! no further filter than minimum intensity is applied !!!
    we recommend to use minimum intensity and mixed-pixel filter
 */
void Sample_LineByLineAccess_minimalCode( const std::wstring& win, const std::wstring& out, int subsample )
{
    const int subsampleLine = subsample;
    const int subsamplePixel = subsample;

    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto p = zfs::open_linebyline( win, &err, OPEN_DEFAULTFLAGS_LBL | OPEN_READ_MASKS | OPEN_USEMATRIX_320); // open a zfs scan-file with line by line access
    if( p )
    {
        const int px = p->Pixel(); // number of pixel each line
        const int ln = p->Lines(); // number of lines (number of lines of all parts)

        const double dx = p->IsProfiler() ? 0.01 : 0.; // unit of x: here, for display/example an arbitrary progress per profile is assumed (no trajectory data available)

        const int minrf = p->MinRf(); // minimum usable intensity
        const int maxrf = p->MaxRf(); // maximum intensity of device. You will get higher values if laser is overloaded. Overloaded pixel are not valid

        ZFRf myrf( p->rf ); // clone the class for accessing color if you want change view of scan
        myrf.ShowMask( false );   // redefine standard (do not show mask out pixel as color )
        myrf.ShowMinmax( false ); // redefine standard (do not show pixel with too less or too bright intensity in different color)
        //myrf.SetModNoRGB();    // force no color if there is any

        ZFSFilter filterParams( p->GetFirstZFSHeader() ); // set default filter parameter depending on scanner
            // filterParams.useCriticalCompensationFilter = false; // The compensation of lines with too fast tilt changes is critical.
                                                            // To filter these lines => useCriticalCompensationFilter = true (default)
                                                            // To keep these lines   => useCriticalCompensationFilter = false
        p->FilterDefine( filterParams ); // set all parameter now
        // !!!!
        // The implementation of the filter will change from version to version
        // so you will get different number of filtered out pixels
        // we try to improve the filters over the time. So it is not a bug if a different number of pixels are returned

        // create a new file for asc xyz export
        int numPoints = 0;
        int numPointsValid = 0;
        int numPointsFilteredOut = 0;

        FILE* fp = zftools::fopen(out, L"wt" );

        {
            p->SkipTo(0); // start with first line

            // all lines
            for( int nl=0; nl<ln; nl+=subsampleLine ) // go trough all lines
            {
                p->Filter(); // call filter with the parameter defined before. Whole line is filtered. Use IsSet afterwards to detect filtered out pixels

                // go trough all pixel in current line
                for( int i=0; i<px; i+=subsamplePixel )
                {
                    if (p->Set(i))           // set pointer to pixel
                    {
                        numPoints++;

#if 1 // example 1: store only valid points -> unsorted point-cloud
                        if (p->IsSetMask(i))// If you want check masks stored in scan as well, you have to open scan with flag OPEN_READ_MASKS
                        {
                            if (p->IsSet(i))   // Check if pixel is enabled. Filter function will disable bad points
                            {
                                numPointsValid++;

                                const int rf = p->Rf();       // raw intensity depends on Imager type
                                                        // Imager5003 has a range up to ~33000
                                                        // Imager5006 has a range up to ~5000000
                                                        // imager5010 has ~1500 up to ~5000000
                                                        // imager5016 has up to ~4100000
                                                        // greater values are possible is laser is overloaded, these values should be filtered out

                                if (rf > minrf)        // check for minimum intensity. This is the most important filter
                                {
                                    // XYZ registered and calibrated
                                    double x, y, z;
                                    p->XYZ(x, y, z); // unit=m  xyz global/registered coordinates

                                    x += nl * dx; // offset to view with, e.g. Lasercontrol when data was acquired in profiler mode

                                    // Color/Grey value of pixel
                                    const auto rgb = myrf.rgbrgb();   // if color-image you will get the color
                                                            // without color red,green,blue has the same value
                                                            // by default you will get a optimal grey value for non colored scans which depends on the RfNorm value
                                                            // if you called used unahe-flag on open scan before you will get optimized grey values if RfNorm is not available

                                    // Some examples for output
                                    if (fp) fprintf(fp, "%.3f %.3f %.3f %d\n", x, y, z, rgb.r()); // optimized grey value because r,g,b are the same, use just red
                                }
                            }
                            else
                                numPointsFilteredOut++;
                        }
                        else
                            numPointsFilteredOut++;

#else // example 2: store as array and mark pixel which are bad
                        bool ok = false;
                        if (p->IsSetMask(i))// If you want check masks stored in scan as well, you have to open scan with flag OPEN_READ_MASKS
                            if (p->IsSet(i))   // Check if pixel is enabled. Filter function will disable bad points
                                ok = true;

                        int rf = p->Rf();       // raw intensity depends on Imager type
                                                // Imager5003 has a range up to ~33000
                                                // Imager5006 has a range up to ~5000000
                                                // imager5010 has ~1500 up to ~5000000
                                                // greater values are possible is laser is overloaded, these values should be filtered out

                        if (rf < minrf)        // check for minimum intensity. This is the most important filter
                            ok = false;

                        // XYZ registered and calibrated
                        double x, y, z;
                        p->XYZ(x, y, z); // unit=m  xyz global/registered coordinates

                        // Color/Grey value of pixel
                        const auto rgb = myrf.rgbrgb();   // if color image you will get the color
                                                // without color red,green,blue has the same value
                                                // by default you will get a optimal grey value for non colored scans which depends on the RfNorm value
                                                // if you called used unahe-flag on open scan before you will get optimized grey values if RfNorm is not available

                        // Some examples for output
                        fprintf(fp, "%.3f %.3f %.3f %d %d %d\n", x, y, z, rgb.r, rf, ok); // optimized grey value because r,g,b are the same, use just red + raw intensity
#endif

                    }
                }

                p->LoadLines(subsampleLine); // load next line. Use 1 for loading every line
            }
            if(fp) fclose( fp );
        }

        std::cout << "points             :" << numPoints << std::endl;
        std::cout << "points valid       :" << numPointsValid << std::endl;
        std::cout << "points filtered out:" << numPointsFilteredOut << std::endl;
    }
}



/**
    Sample_LineByLineAccess_fullExample: Line by line => using a roll buffer, handling 3 lines in the memory to apply filtering.
    access to line-header and raw-data like counters
 */
void Sample_LineByLineAccess_Imager_fullExample( const std::wstring& win, const std::wstring& out, int subsample )
{
    int subsampleLine = subsample;
    int subsamplePixel = subsample;

    int flags = OPEN_DEFAULTFLAGS_LBL | OPEN_USEMATRIX_320;
#define USE_SCAN_MASKS // we recommend to use this
#ifdef USE_SCAN_MASKS
    flags |= OPEN_READ_MASKS;  // use filter and selections already in scan defined
#endif
    /* possible flags, see zfs/linebyline.h for current available flags
    OPEN_SINGLE             // open only single file if scan has more scans
    OPEN_NOPRENEXT          // do not load preview and next line, is much faster but filter and dynamic track calculation will not work
    OPEN_CREATE_TEMP_POS    // create a linepos file in temp folder, useful for faster read is scan is opened several times
    OPEN_READ_MASKS         // 14.5.08 read masks as well, Needs memory and takes time. You can use IsSetMask() afterwards to check if mask bit is set for a pixel
    OPEN_IGNOR_LEVEL        // NEW 8.3  do not calculate level-matrix
    OPEN_IGNOR_COMPENSATOR	// NEW 8.6  November 2014. If you dont want to use dynamic compensation correction. For example if scan was taken on a ship, you may have to use the flag.
    OPEN_USEMATRIX_320      // use final registration matrix with coordinate system (default for SDK). Without this option you will get the local registered scans without the coordinate system
    */
    //flags |= OPEN_SINGLE;
    //flags |= OPEN_NOPRENEXT;
    //flags |= OPEN_UNAHE;   // create nice grey values, need a lot of time

    // NEW 8.6  November 2014
    // Dynamic compensation of scans is supported. If scan moves while scanning, it will be corrected.
    // If there are too big movements detected, an error will be returned.
    // Disable by using flag OPEN_IGNOR_COMPENSATOR

    zfs::zfserr_t err = zfs::ERR_UNDEF;
    auto p = zfs::open_linebyline( win, &err, flags ); // open a zfs scan-file with line by line access
    if( !p )
    {
        std::wstring error = zfs::ZFSErrorString( err );
        std::wcout << L"Error:" << error << endl;
    }
    else
    {
        zfs::way_t wm = zfs::WayCounting::DetectWayModeFromZFSHeader(p->GetFirstZFSHeader());
        if( wm>=zfs::WAY_NO )
            p->InitWay( wm, 0 );

        DetectDim( p.get() ); // Dimension of Scan. Remove it if you don't need it

        const bool color = p->HasRGB(); // true if scan has color information

        const int px = p->Pixel(); // number of pixel each line
        const int ln = p->Lines(); // number of lines (number of lines of all parts)
        cout << "Pixel " << px << " x Lines " << ln << endl;



        // registration / global-matrix / world-coordinates
        double matrix[16];
        memset( matrix, 0, sizeof(matrix) );
        if( p->Matrix().HasTR() ) // Check if available
            p->Matrix().GetTRMatrix4x4( matrix );
        cout << "Translation " << matrix[3] << " " << matrix[7] << " " << matrix[11] << " m" << endl;
        // different style for access translation
        //double tx = p->Matrix().tx();
        //double ty = p->Matrix().ty();
        //double tz = p->Matrix().tz();

        // NEW 8.3  Mai 2012
        // level matrix
        // profiler does not support level
        const int lev = p->LevelMatrixStatus();
        if( lev == (int)zfs::level::eErrorCode::ERROR_OK ) // level-matrix was loaded from zfs-header and calculated successfully
        {
            // level-matrix will be stored in the global-matrix as well if no other registration is defined

            // TIP
            // the internal calculation calculate a translation as well, if you just need the rotation, set translation to zero

            // scan uses level
            ZFTRMatrix& mat = p->LevelMatrix();
            double matLevel[16];
            memset( matLevel, 0, sizeof(matLevel) );
            if( mat.HasTR() ) // Check if available
            {
                mat.GetTRMatrix4x4( matLevel );

                cout << "Level " << matLevel[3] << " " << matLevel[7] << " " << matLevel[11] << " m" << endl;
            }
        }


        // NEW 8.5  October 2013
        // Compensator feature:
        // profiler does not support compensator feature
        const bool lev_dyn = p->compensator->IsPresent();
        if( lev_dyn) // true => compensator is used to compensate scanner movement that occurred during scanning
        {
            // if lev_dyn==true the coordinates (p->XYZ0) are corrected by (p->x_level_corr_glob and p->y_level_corr_glob)
            p->compensator->x_level_corr_glob;  // [µrad] xlevel values without scanner tilt (n_x) and without xlevel offset (s_x) ==> shows scanner movement, wobbling during scan
            p->compensator->y_level_corr_glob;  // [µrad] ylevel values without scanner tilt (n_y) and without ylevel offset (s_y) ==> shows scanner movement, wobbling during scan
            p->compensator->level_x;            // level_x[0] = n_y
                                                // level_x[1] = n_x
                                                // level_x[2] = s_y
                                                // level_x[3] = s_x
            cout << "n_y" << p->compensator->level_x[0] << "[rad]"  << endl;
            cout << "n_x" << p->compensator->level_x[1] << "[rad]"  << endl;
            cout << "s_y" << p->compensator->level_x[2] << "[rad]"  << endl;
            cout << "s_x" << p->compensator->level_x[3] << "[rad]"  << endl;


            p->compensator->Cxx;    // covariance matrix: [rad]
                                    // | cov(n_y,n_y)   cov(n_y,n_x)   cov(n_y,s_y)   cov(n_y,s_x)  |
                                    // | cov(n_x,n_y)   cov(n_x,n_x)   cov(n_x,s_y)   cov(n_x,s_x)  |
                                    // | cov(s_y,n_y)   cov(s_y,n_x)   cov(s_y,s_y)   cov(s_y,s_x)  |
                                    // | cov(s_x,n_y)   cov(s_x,n_x)   cov(s_x,s_y)   cov(s_x,s_x)  |
            cout << "covariance matrix: [rad]"<< endl;
            for(int i=0;i<4;i++)
            {
                cout << p->compensator->Cxx[i][0] << " " << p->compensator->Cxx[i][1] << " " << p->compensator->Cxx[i][2]  << " " << p->compensator->Cxx[i][3]  << endl;
            }
        }

        //*******************************************
        // NEW 8.6  November 2014
        // Level - Movements while scanning
        zfs::zfserr_t err_;
        ZFSHeader h;
        err_ = h.ReadE(win);

        zfs::level::registerLevel leveler;
        double max_tilt_changes = 0.1;	// The level parameters which are measured before and after scanning, are used to detect movements.
                                        // If the movements are bigger than "max_tilt_changes" ([deg]) the method leveler.init(..)
                                        // (default max_tilt_changes=0.1 ) will return an error: "ERROR_Movement".
                                        // If you prefer other maximal movement values, you can change "max_tilt_changes".
                                        // Note: If you use OpenLineByLine the default value for "max_tilt_changes" is used.
                                        //       To ignore the level use flag OPEN_IGNOR_LEVEL while opening the scan.
        const auto code = leveler.init(&h, false,-0.3373,2);
        if (code == zfs::level::eErrorCode::ERROR_OK)
        {
            ZFTRMatrix* pMatrix =  new ZFTRMatrix(leveler.matrix());
        }
        //*******************************************



        const int minrf = p->MinRf(); // minimum usable intensity
        const int maxrf = p->MaxRf(); // maximum intensity of device. You will get higher values if laser is overloaded. Overloaded pixel are not valid

        // example access zfs-header
        ZFSHeader* firsthead = p->GetFirstZFSHeader(); // access to zfs file header. if scan exist of more scans it is the header of the first scan otherwise it is the same as GetZFSHeader
        ZFSHeader* head = p->GetZFSHeader(); // access to zfs file header. if scan exist of more than one scans it is the head of the last scan
        if( firsthead && head )
        {
            std::string help;
            cout << "Typ       : " << head->GetLong(1,0) << endl; // (index,default-value)
            cout << "Next scan : '" << firsthead->getStringC(592,help) << "'" << endl; // name of next file if scan exist of more than one part

            // detect number of pixels in each part
            const int start1 = head->GetLong( 61, 0 );
            const int stop1  = head->GetLong( 62, 0 );
            const int start2 = head->GetLong( 63, 0 );
            const int stop2  = head->GetLong( 64, 0 );
            const int subsample  = head->GetLong( 65, 0 );
            const int pixel1 = (stop1 - start1 + subsample) / subsample;
            cout << "Pixel part1 : " << pixel1 << endl;
            if( start2>0 ) // part 2 is not always available
            {
                const int pixel2 = (stop1 - start1 + subsample) / subsample;
                cout << "Pixel part2 : " << pixel2 << endl;
            }
        }
        else
        {
            cerr << "Scan has no header. Maybe no ZfS file." << endl;
            return;
        }

#if RAWPIXELDATAACCESS
        int byteperpixel = head->GetLong(3,0);
#endif

        //
        // Profiler stuff
        //
        if( zfs::IsProfiler(head) )
        {
            // Scan is a Profiler scan
            //p->UseLProfiler(false); // do not use l-encoder if scan is a profiler, x-direction can be added by your own
            //p->SetXDistance( xDis ); // for profiler mode set x-direction distance if no x-axis info available
        }


        //
        // Filter
        //
        // Some filter already implemented in the zfs.dll
        // You may implement your own filter too get better results for your application.
        // The implementation of the filter may change from version to version a little bit
        // Pixels are only masked out, they are not removed while filtering
        // The most important filter are minimum intensity and mixed-pixel
        // !!!! The filter are designed to run very fast, they are not designed to get best result
#define USEFILTER
#ifdef USEFILTER
        // NEW 8.3
        ZFSFilter filterParams( p->GetFirstZFSHeader() ); // set default filter parameter depending on scanner
        filterParams.minRg = 0.5; // for example set minimum range [meter]
        cout << "minimum intensity" << filterParams.minRf << endl;
        // call your own dialogue to allow change of parameter
        // filterParams.mix_d = 2; // example change parameter
        // filterParams.maxRg = 50; // example change maximum range

        //filterParams.useCriticalCompensationFilter = false; // The compensation of lines with too fast tilt changes is critical.
                                                              // To filter these lines => useCriticalCompensationFilter = true (default)
                                                              // To keep these lines     => useCriticalCompensationFilter = false

        p->FilterDefine( filterParams ); // set all parameter now

        // print out all available filter params for this scan type including description and its current setting
        zfs::FilterParams filter;
        filter.useFrom(filterParams);
        std::cout << "Filter params used for this scan" << filter << std::endl;
#endif



        ZFRf myrf( p->rf ); // clone the class for accessing color if you want change view of scan
        myrf.ShowMask( false );   // redefine standard (do not show mask out pixel as color )
        myrf.ShowMinmax( false ); // redefine standard (do not show pixel with too less or too bright intensity in different color)
        //myrf.SetMod( ZFRf::M_UNAHE ); // use different mode than default which is M_RFCOMP. M_LINEAR is with linear interpolation

        // create a new file for asc xyz export
        cout << "Export" << endl;
        FILE* fp = zftools::fopen(out, L"wt");

        p->SkipTo(0); // start with first line

        int lineheadersize = head->GetLong(7,0) + head->GetLong(8,0);


        bool end = false;
        // all lines
        int akt_n = 0, akt_m = 0;
        for( int nl=0; nl<ln && !end; nl+=subsampleLine ) // go trough all lines
        {
            // information about current scan-file if scan consist of more than one part
            const int currentpart = p->getCurrentPart();
            const int parts = p->getParts();
            const int lineInCurrentPart = p->LineInCurrentPart();
            std::wstring currentFile = p->CurrentFileNameW();

#ifdef USEFILTER // should be used
            // filter current line
            //   pixel will be mask out, they are still available
            //   the IsSet function will test pixel afterwards
            //   pixel below minrf will be masked out as well
            p->Filter();
#endif

            zfs::LineHeader lineheader(p->L()->LPtr(),lineheadersize);
            if (lineheader.hasCnt1())
            {
                uint32_t counter1 = lineheader.cnt1();
            }
            if (lineheader.hasCnt2())
            {
                uint32_t counter2 = lineheader.cnt2();
            }

#ifdef RAWLINEDATAACCESS
            // access to line-header raw data
            // you need that if you connected external counters to the scanner
            long pps_line_start = 0;
            if( lineheadersize )
            {
                long* lheader = (long*)(p->L()->LPtr()); // Pointer to line-header

                // identifier
                long identifier = *lheader;

                // PPS Counter-1
                pps_line_start = *(lheader+11);

                // all
                for( int lh=0; lheader && lh<=lineheadersize; lh+=sizeof(long) )
                {
                    long v = *lheader++;
                    //...
                }
            }
#endif




            // go trough all pixel in current line
            for( int i=0; i<p->Pixel(); i+=subsamplePixel )
            {
                if (p->Set(i))              // set pointer to pixel
                {


#if RAWPIXELDATAACCESS
                    // access to pixel raw data
                    char* raw = p->L()->PPtr();
                    for (int bpp = 0; bpp < byteperpixel; bpp++)
                    {
                        char v = *raw++;
                        //....
                    }

                    int pps_pixel = (*(raw - 1)) & 3; // additional byte see documentation (counter1 and counter2 least significant bits)
#endif





#ifdef USE_SCAN_MASKS
                    if (p->IsSetMask(i))// If you want check masks stored in scan as well, you have to open scan with flag OPEN_READ_MASKS
#endif
                    {
                        if (p->IsSet(i))   // Check if pixel is enabled. Filter function will disable bad points
                        {


                            const int rf = p->Rf();       // raw intensity depends on Imager type
                                                    // Imager5003 has a range up to ~33000
                                                    // Imager5006 has a range up to ~5000000
                                                    // imager5010 has ~1500 up to ~5000000
                                                    // greater values are possible is laser is overloaded, these values should be filtered out

                            if (rf > minrf)        // check for minimum intensity. This is the most important filter
                            {
                                const double rg = p->Rgm();   // range unit=m

                                // XYZ registered and calibrated
                                double x, y, z;
                                p->XYZ(x, y, z); // unit=m  xyz global/registered coordinates
                                //use XYZ0(x,y,z) for unregistered/local but calibrated
                                //p->XYZ0( x,y,z ); // unit=m

                                // If you need polar-coordinates instead of Cartesian, use following after XYZ
                                double elev, hor, range;// [rad,rad,meter]
                                zfs::XYZ2polar(x, y, z, elev, hor, &range);

                                const double rfnorm = p->RfNorm1(); /// intensity in the range from 0 to 1.
                                                                /// if scanner support intensity correction depending on range and temperature you will get corrected values
                                                                /// you will get 1 for white and 0 for back BUT depending on angle the values change
                                                                /// you may get values greather than 1 for the function RfNorm() which means that laser is overloaded
                                                                /// use p->PtrRfComp() to detect if scanner support it
                                                                /// if scanner does not support intensity correction this function returns a linear interpolated value
                                                                /// index 700 in the zfs-header tells if compensation is available. setting this value to zero disables intensity correction
                                                                /// never use this value for filtering

                                const uint32_t encoderVert_increments = p->L()->EncH();
                                const uint32_t encoderHorz_increments = p->L()->EncL();

                                // Color/Grey value of pixel
                                const auto rgb = p->rgb();   // if color image you will get the color
                                                        // without color red,green,blue has the same value
                                                        // by default you will get a optimal grey value for non colored scans which depends on the RfNorm value
                                                        // if you called used unahe-flag on open scan before you will get optimized grey values if RfNorm is not available
                                const auto rgb2 = myrf.rgbrgb(); // use not standard viewing mode. use mode we defined in the ZFRf class

                                // Some examples for output
                                if (fp)
                                {
                                    //fprintf(fp, "%.3f %.3f %.3f %d\n", x, y, z, rgb.r); // optimized grey value because r,g,b are the same, use just red
                                    //fprintf( fp, "%.3f %.3f %.3f %d\n", x,y,z, rf ); // raw intensity
                                    // alternativ
                                    //fprintf( fp, "%.3f %.3f %.3f %.0f\n", x,y,z, rfnorm*255. ); // real grey value in the range from 0 to 255
                                    // alternativ
                                    if (color)
                                        fprintf(fp, "%f %f %f %f %d %d %d\n", x, y, z, rfnorm, rgb.r(), rgb.g(), rgb.b()); // real grey value in the range from 0 to 1
                                    else
                                        fprintf(fp, "%f %f %f %f\n", x, y, z, rfnorm); // real grey value in the range from 0 to 1
                                                                                       // color
                                    //fprintf( fp, "%.3f %.3f %.3f %d %d %d\n", x,y,z, rgb.r,rgb,g,rgb,b ); // color
                                    // range and intensity
                                    //fprintf( fp, "%.3f %d\n", rg, rf ); // color
                                }
                            }
                        }
                    }
                }
            }

            p->LoadLines(subsampleLine); // load next line. Use 1 for loading every line
        }
        if( fp ) zftools::fclose( fp );
    }
}

// =============================================================================================================================


/*
    example code for reading whole scan into memory

    !!! This code can not read big files (except 64bit version)
    !!! This sample is very slow because scan is read into memory
    !!! Please use the previous example with the linebyline code if possible

*/

int Sample_WholeScan(const std::wstring& in, const std::wstring& out, int subsample )
{

    cout << "ZFS Version: " << ZFSDLLVersion() << endl;

#if 0
    // this will be necessary if calibration should be loaded from ini file (Z+F IMAGER 5003 only)
    // be default you don't need this
    char exepath[256];
    strcpy( exepath, "c:\\Prj\\lrc\\exe" );
    ZFScanBasic::SetPath4Calibfile( exepath );
#endif


    cout << "Loading ..." << endl;
    // scan-list holds all open scans
    zfs::zfserr_t err = zfs::ERR_UNDEF;
    ZFScan* scan = scanlist.Open( in, NULL, OPEN_DEFAULTFLAGS_ZFSCAN | OPEN_USEMATRIX_320, &err ); // open and load scan into memory
    if( scan )
    {
        // some basic infos
        cout << "Pixel : " << scan->Pixel() << endl;
        cout << "Lines : " << scan->Lines() << endl;

        cout << (scan->IsCalibrated() ? "Calibrated" : "NOT calibrated") << endl;

        // call filter(s) (important)
        // this filter should be always used
        // filter will take a long time on big scans
        // filter will mask out pixel which are not ok
        cout << "Apply filters ..." << endl;
//FIXME_FILTER        scan->applyFilter( "MixedPixel", "-angle 2.0 -num 6 -horz -vert -diag -removeedge" );
        cout << "Ready" << endl;

        /*FIXME
        // filter will mask out all pixel of a line with too fast tilting changes (=>critical compensation)
        cout << "Apply critical compensation filters ..." << endl;
        scan->applyFilter_CriticalCompensation( "criticalcompensation", " " );
        cout << "Ready" << endl;
        */


        scan->unahe(); // create lut for optimized contrast

#if 1 // matrix
        // get camera-position (rotation-matrix and translation)
        // if scan is registered you will get registered xyz coordinates with the XYZ() function
        // so it is not necessary to read out registration
        double matrix[16];
        memset( matrix, 0, sizeof(double)*16 );
        if( scan->HasTR() ) // Check if available
            scan->GetTRMatrix4x4( matrix );
        cout << "Translation " << matrix[3] << " " << matrix[7] << " " << matrix[11] << " m" << endl;
#endif // matrix

        // access to ZFS file header
        // a description of all items you will find in the zfs.ini file
        int lhsz = 0; // line-header size
        int bpp = 0; // byte per pixel
        ZFSHeader* head = scan->GetZFSHeader();
        if( head )
        {
            cout << "Typ    : " << head->GetLong(1,0) << endl; // (index,default-value)
            cout << "Filter : " << head->GetDouble(11015,0) << " hz" << endl; // (index,default-value)
            cout << "Speed  : " << head->GetDouble(70,0) << " rps" << endl; // (index,default-value)

            lhsz = head->GetLong(8,0); // (index,default value)
            bpp = head->GetLong(3,0); // (index,default value)
        }



        const int inc = subsample;        // sub-sample 1..n
        const int minrf = 200;    // minimum intensity 0..


        // calculate xyz value
        const int xp = 10;    // x-position of interest
        const int yp = 10;    // y-position
        if (scan->Set(xp, yp)) // registered xyz if available
        {
            double x, y, z;
            scan->XYZ(x, y, z); // registered xyz if available
            cout << "XYZ at " << xp << "," << yp << " : " << x << "," << y << "," << z << " m" << endl;

            // get range and intensity
            const double rg = scan->Rgm(); // unit: meter
            const int rf = scan->Rf(); // unit: inc
            cout << "Range     : " << rg << " m" << endl;
            cout << "Intensity : " << rf << " inc" << endl;

            // additional infos about scan
            cout << "Info:" << endl;
            cout << scan->Info() << endl;



            if (scan->HasRGB()) // scan is a scan with color information
            {
                cout << "Scan with color information" << endl;
            }


#if 1 // export to  asc
            //
            // create asc xyz file
            //
            //   xyz values are calibrated and registered
            //
            if (out.length())
            {
                FILE* outf = zftools::fopen(out, L"wt");
                if (outf)
                {
                    cout << "Creating output file ..." << endl;
                    //go trough whole file
                    for (int l = 0; l < scan->Lines(); l += inc)
                    {
                        const char* pLH = scan->AdrLine(l); // pointer to current line-header
                        const int encl = *(int*)(pLH + 8);      // get value for l-encoder of line-header

                        for (int p = 0; p < scan->Pixel(); p += inc)
                        {
                            if (scan->IsSet(p, l)) // check if all masks are enabled for this pixel (mixed pixel filter will clr maskbit)
                            {
                                if (scan->Set(p, l))  // set internal pointer to pixel
                                {

                                    const int rf = scan->Rf();        // Intensity in increments (0..32768)
                                    // very important
                                    if (rf >= minrf)           // remove pixel with too less intensity (sky,...)
                                    {
                                        const double rg = scan->Rgm();   // Range as meter
                                        const auto col = scan->rf.rgbrgb(); // Intensity as rgb with optimized grey value.
                                                                    // If scan is a colored scan you will get the color information
                                                                    //  otherwise the red,green and blue value is the same

                                        const double rfcomp = scan->RfNorm1(); // get compensated intensity in the range of 0..1 if scanner support it otherwise fall-back
                                                                         // use scan->PtrRfComp() to check if scanner support it
                                                                         // never use this value for filtering

                                        // pointer to current pixel
                                        const char* pPixel = pLH + lhsz + scan->CurrentPixel() * bpp + (bpp - 1);
                                        const int pps = encl + *pPixel;

                                        // If Scan is a profiler scan you have to recalculate the xyz value by your own
                                        double x, y, z;
                                        scan->XYZ(x, y, z);    // calibrated and registered(if available) xyz value. If not registered it is the same as XYZ0(...)
        //                                    scan->XYZ0( x,y,z );   // local scanner coordinate

        //                                    fprintf( out, "%.3f %d\n", rg, rf ); // write range and intensity
        //                                    fprintf( out, "%.3f %.3f %.3f %d\n", x,y,z, rf ); // write 3D XYZ values (meter) and raw-intensity
                                        fprintf(outf, "%.3f %.3f %.3f %d\n", x, y, z, col.r()); // write 3D XYZ values (meter), use value from optimized-lut for color
        //                                    fprintf( out, "%.3f %.3f %.3f %d %d %d\n", x,y,z, col.r,col.g,col.b ); // write 3D XYZ values (meter) and color
                                    }
                                }
                            }
                        }
                    }
                    zftools::fclose(outf);
                }
                else
                    cout << "Can't create output file " << endl;
            }
#endif // export
        }

        if (system("Pause") != 0)
            cout << "failed to pause\n";
        scanlist.Close(scan); // close scan and free memory
    }
    else
    {
        std::wstring error = zfs::ZFSErrorString( err );
        std::wcout << L"Error:" << error << endl;
    }

    return 0;
}

// =============================================================================================================================
