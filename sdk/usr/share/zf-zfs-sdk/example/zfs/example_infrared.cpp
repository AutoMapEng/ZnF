#include "example_infrared.h"
#ifdef INFRAREAD_EXAMPLE

#include <iostream>
#include <fstream>

#include "../../zfs/zfs.h"
#include "../../zfs/linebyline_smart.h"



using namespace std;



#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

//========================================================================================================================
//   Thermal camera related stuff

typedef unsigned short ir_value_t;  // der Wert wie er im Tiff gespeichert ist

                                    /**
                                    \brief Convert a value stored in the tiff (unsigned short) to a temperature (deg)
                                    */
inline double ir_val2temperature(ir_value_t v)
{
    return (0.1 * double((v - 1000)));
}

/**
This function computes the temperature for materials that have a emissivity different from 1.

The function uses the Stefan-Boltzmann law to correct the temperature.

\param blackBodyTemperature The temperature given by the camera assuming a emissivity of 1.
This is usually the value retrieved from the ir_val2temperature() function.

\param emissivity Get this from a material table or measure it yourself. Values are between 0 and 1

\return The corrected temperature is higher than the raw blackBodyTemperature because "gray-body" objects emit less radiation at same temperatures.
*/
inline double getEmissivityCorrectedTemperature(double blackBodyTemperature, double emissivity)
{
    return std::pow(1. / emissivity, 0.25) * blackBodyTemperature;
}


cv::Mat mycvReadImage(zf::FileName& fn, int flags = CV_LOAD_IMAGE_ANYDEPTH)
{
    std::string sin = fn.String();
    return cv::imread(sin, flags);
}


//========================================================================================================================


/*
    this 2. sample shows how to open a large file and access just only one line
    only 3 lines are hold in memory, so you can open very large scans

    if scan exist of more than one file, it will open the next file automatically
*/


/**
    Sample_LineByLineAccess_minimalCode
    !!!! no further filter than minimum intensity is applied !!!
    we recommend to use minimum intensity and mixed-pixel filter
 */
void Sample_LineByLineAccess_Infrared(const std::wstring& win)
{
    // the temperature for scaling
    const double tempFrom = 0;
    const double tempTo = 40;

    try
    {
        // open the scan
        zfs::zfserr_t err = zfs::ERR_UNDEF;
        zfs::zfslinebyline_ptr p = zfs::open_linebyline(win);
        if (!p.get())
        {
            std::cerr << "error open zfs scan\n";
            return;
        }

        // Open the thermal/infrared tiff image which has the absolute temperatures
        zf::FileName fn(win);
        fn.RemoveExt(".zfs");
        fn.AddExt(".ir.tiff");
        cv::Mat ir = mycvReadImage(fn);
        if (ir.empty())
        {
            std::cerr << "no infrared data found\n";
            return;
        }


        const int pixel = p->Pixel(); // number of pixel each line
        const int lines = p->Lines(); // number of lines (number of lines of all parts)


        ZFRf myrf(p->rf); // clone the class for accessing color if you want change view of scan
        myrf.ShowMask(false);   // redefine standard (do not show mask out pixel as color )
        myrf.ShowMinmax(false); // redefine standard (do not show pixel with too less or too bright intensity in different color)
        myrf.SetModNoRGB();    // force no color if there is any


        cv::Mat imageGray(lines, pixel, CV_8UC3);
        cv::Mat imageInfrared(lines, pixel, CV_8UC3);


        // all lines
        p->SkipTo(0); // start with first line
        for (int ln = 0; ln < lines; ln += 1) // go trough all lines
        {
            // go trough all pixel in current line
            for (int px = 0; px < pixel; px += 1)
            {
                if (p->Set(px))           // set pointer to pixel
                {


                    // Color/Grey value of pixel
                    const auto rgb = myrf.rgbrgb();   // if color-image you will get the color
                                            // without color red,green,blue has the same value
                                            // by default you will get a optimal grey value for non colored scans which depends on the RfNorm value
                                            // if you called used unahe-flag on open scan before you will get optimized grey values if RfNorm is not available
                    imageGray.at<cv::Vec3b>(ln, px) = cv::Vec3b(rgb.b, rgb.g, rgb.r);



                    // get the temperature for the pixel as absolute degree celsius
                    // material of surface has to be calculated as well by defining a material by the user

                    // Because of optical distortion not for every scan point there can be calculated a temperature value.
                    // In this case the mapping algorithm set - 100deg (abs without correction) for this points.
                    // For visualization may use the grayvalue of the scan instead.For creating a point cloud just clip out that points.

                    int iline = ln, ipixel = px;
                    p->Map(ipixel, iline); // map into tiff image
                    if (iline >= 0 && iline < ir.rows && ipixel >= 0 && ipixel < ir.cols)
                    {
                        ir_value_t val = ir.at<ir_value_t>(iline, ipixel);
                        double absCelsius = ir_val2temperature(val);
                        const double myEmissivity = 0.9;    // If you know the emissivity of the material that you want to observe, you can correct recorded temperature with it.
                        absCelsius = getEmissivityCorrectedTemperature(absCelsius, myEmissivity);

                        // convert the absulute temperature to a gray value for printing out into an image
                        int temp255 = int((absCelsius - tempFrom) / (tempTo - tempFrom) * 255.);
                        if (temp255 < 0) temp255 = 0;
                        else if (temp255 > 255) temp255 = 255;

                        imageInfrared.at<cv::Vec3b>(ln, px) = cv::Vec3b(temp255, temp255, temp255);
                    }
                    else
                    {
                        imageInfrared.at<cv::Vec3b>(ln, px) = cv::Vec3b(0, 0, 0);
                    }
                }
            }

            p->LoadLines(1); // load next line. Use 1 for loading every line
        }


        // store the scan a gray image
        zf::FileName fnGray(win);
        fnGray.RemoveExt(".zfs");
        fnGray.AddExt(".gray.jpg");
        fnGray.Delete();
        cv::imwrite(fnGray.c_str(), imageGray);

        // store the infrared as a gray image
        zf::FileName fnInfrared(win);
        fnInfrared.RemoveExt(".zfs");
        fnInfrared.AddExt(".infrared.jpg");
        fnInfrared.Delete();
        cv::imwrite(fnInfrared.c_str(), imageInfrared);
    }
    catch (const cv::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown exception" << std::endl;
    }
}



// =============================================================================================================================
#endif // INFRAREAD_EXAMPLE
