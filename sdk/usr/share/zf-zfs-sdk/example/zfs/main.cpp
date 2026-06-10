//
// Example for reading zfs files
//
// ZFS is the basic file format which our scanner will provide
//
// The zfs.dll supports reading of zfs scans.
// This is a standalone library which depends on no other libraries
// Just this example uses the zftools.dll for the Argument/Zeitmessung class
// A further example uses the zfsfilter.dll as well

// Please read licence terms stored license.de.txt/license.en.txt as well

// history can be found in the file history.sdk.txt

// most important function is: Sample_LineByLineAccess_...()

/*
    Basic features history
    8.5 sync api
    8.6 dynamic compensator support
    9.0             support for 5016 serie
    9.0 10.7.2017   add example for profiler with filter
                    add example for infrared
    9.2 4.2020      add more examples (write, multithreaded)
                    some cleanups
                    removed profiler xml filtering, using standardfilter jet
                    removed old road sign filter (there is a new sdk for road sign filter)
    10.0            You need now the zfs.dll and the zfcore.dll
      31.8.2023     Added the possibility to choose the coordinatesystem the scans are registered
                    A new flag is added which has to be used now. This is the default and recommended option how to use it.
*/




#include <iostream>

// this two includes are just for this example and are not requied for your application
#include <zfcore/arg.h> // just to parse command line arguments
#include <zftools/zeitmessung.h> // zeitmessung = time measurment

#include "example_tools.h"
#include "example_imager.h"
#include "example_infrared.h"
#include "example_profiler.h"
#include "example_layer.h"
#include "example_write.h"
#include "example_lineheader.h"

//========================================================================================================================

#include "../license.h" // remove that line and ask Zoller+Froehlich support to get your own license. Activate following lines by removing comment and enter your code you got from us.
/*
#define ZFS_LICENSE_CUSTOMER    "customer"
#define ZFS_LICENSE_EXPIRE      "01.01.2010"
#define ZFS_LICENSE_KEY         "key"
*/



using namespace std;

int main(int argc, char* argv[])
{
    zf::Argument arg(argc, argv); // command line argument helper
//    zfs::SetAppIF(zfsCB); //new in 8.6   callbackinterface for progressbar and other stuff

#if 0 // just to try out / debug
    arg.addItem("in", "D:/Support/ToZF/MeasData/tmp_pps_fy033_emb-5_s1d1_f1_20240604-083633.zfs");
    arg.addItem("out", "");
    arg.addItem("process","1");
    arg.addItem("profiler", "1");
    arg.addItem("sync", "1");
#endif

    if (arg.count() < 1 || arg.has("help") )
    {
        cout << "\nusage: SampleZFS.exe\n" 
            << " [-in <inputfile full path>] 'input zfs file'\n"
            << " [-out [<outputfile full path>]] 'should output file be created and optional define a file'\n"
            << " [-subsample <sub>] 'access the scan with subsample default:1'\n"
            << " [-process] 'open a scan and process it'\n"
            << "   [-profiler] 'interpret the scan as a profiler scan'\n"
            << "     [-mt] 'use the multithreaded profiler example'\n"
            << "     [-sync] 'for profiler you can call synchronisation'\n"
            << "   [-full] 'more complex example for imager'\n"
            << "   [-zfscan] 'example load whole scan into memory for imager'\n"
            << "   [-lineheader] 'example load whole scan and export the lineheader'\n"
            << " [-header] 'export the zfs header as text'\n"
            << " [-write] 'example for writing scan'\n"
            << " [-generate_layers [force]] 'generate layer from filter masks'\n"
            << " [-read_layer] 'read layer and convert it to an image'\n"
            ;

        return -1;
    }

    cout << "ZFS.DLL Version" << ZFSDLLVersion() << " " << ZFSDLLDate() << endl;

    // set license
    if (!ZFSDLL_SetLicense(ZFS_LICENSE_CUSTOMER, ZFS_LICENSE_EXPIRE, ZFS_LICENSE_KEY)) // please ask zfsupport for your license
    {
        cout << "Sorry wrong license" << endl;
        return -1;
    }


    //
    // parse common command line params
    //
    std::wstring inFile = arg.getWString("in");  // the zfs file !!! must be full path !!!
    if( inFile.length()==0)
    {
        cout << "Please define the input file at least" << endl;
        return -1;
    }
    std::wstring outFile = arg.getWString("out");  // optional, the output file !!! must be full path !!!
    if (arg.has("out") && outFile.length() == 0)
        outFile = zf::FileName(inFile).ReplaceExt(".zfs", ".asc").WString(); // auto generate output file name if not other defined
    if (inFile == outFile) return -1;
    wcout << L"input: " << inFile << L" output:" << outFile << endl;

    const int subsample = 1;// std::max(arg.getInt("subsample", 1), 1);


    zftools::Zeitmessung tm; // process time


    //
    // examples for access to the scan
    //
    if( arg.has("process") )
    {
        if (arg.has("profiler"))
        {
            // example for profiler or imager in profiler mode

            if (arg.has("mt")) // new since 9.2
            {
                cout << "Example profiler multithreaded ...\n";
                Sample_LineByLineAccess_Profiler_MT(inFile, arg.has("sync"));
            }
            else if (arg.has("old")) 
            {
                cout << "Old example profiler single threaded and export ...\n";
                Sample_LineByLineAccess_Profiler(inFile, outFile, arg.has("sync"), subsample);
            }
            else // default without multithreading
            {
                cout << "Example profiler single threaded and export ...\n";
                Sample_LineByLineAccess_Profiler_V10(inFile, outFile, arg.has("sync"), subsample);
            }

            // For road sign filter, there is a different SDK. 
            // But this require that you have zfs files with time information, a track file from the GPS/IMU and a mounting calibration.
        }
        else // imager
        {
#ifdef INFRAREAD_EXAMPLE
            if (arg.has("infrared"))
                Sample_LineByLineAccess_Infrared(inFile); // need OpenCV library to work
            else
#endif
                if (arg.has("lineheader"))
                {
                    cout << "Example exporting the lineheader of a whole scan\n";
                    Sample_ExportLineheader(inFile, outFile); 
                }
                else if (arg.has("full"))
                {
                    cout << "Example imager full version ...\n";
                    Sample_LineByLineAccess_Imager_fullExample(inFile, outFile, subsample); // more advanced
                }
                else if (arg.has("zfscan"))
                {
                    cout << "Example reading scan into memory ...\n";
                    Sample_WholeScan(inFile, outFile, subsample); // not recommended, just for compatibility
                }
                else
                {
                    cout << "Minimal default example with export ...\n";
                    Sample_LineByLineAccess_minimalCode(inFile, outFile, subsample); // <<<<< example to start with. The default if no further param is defined <<<<<<<<<
                }

        }

    }

    //
    // tools
    //

    // read zfs header
    if (arg.has("header"))
    {
        SampleReadHeader(inFile);
    }

    //
    // more advanced examples
    //

    // read a zfs and write a modified zfs in a new file
    if (arg.has("write"))
    {
        std::wstring outZfs = arg.getWString("out");  // optional, the output file !!! must be full path !!!
        if ( outZfs.length() == 0)
            outZfs = zf::FileName(inFile).ReplaceExt(".zfs", ".new.zfs").WString(); // auto generate output file name if not other defined
        wcout << "Example modify: " << inFile << " -> " << outZfs << endl;
        Sample_ReadWrite(inFile, outZfs );
    }

    // generate layer files from filter masks
    if (arg.has("generate_layers"))
    {
        if (!Sample_MaskToLayer(inFile, arg.has("force")))
            cout << "\nSome of the required masks were not found\n"
                "or an error occured during call of Sample_MaskToLayer()!\n" << endl;
    }

    // read a layer and create a image
    if (arg.has("read_layer"))
    {
        if (!Sample_ReadLayersAndWriteToImage(inFile, zfs::layer2image_type_t::L2I_PNG))
            cout << "\nSome of the required layers were not found\n"
                "or an error occured during call of Sample_ReadLayersAndWriteToImage()!\n" << endl;
    }

    cout << "Processing time: " << ((double)tm.duration() / 1000.) << "sec" << endl;

    return 0;
}
