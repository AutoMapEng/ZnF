#include "example_tools.h"

#include <iostream>
#include <fstream>

#include <zfs/lineheader.h>


using namespace std;



//========================================================================================================================

// return the scan recording time in seconds
double scanRecordingTime(ZFSLinebyline* lbl)
{
    if (!lbl) return 0;
    lbl->SkipTo(0);
    zfs::LineHeader lh0(lbl->L()->LPtr(), lbl->LineInfo()->lineHeaderSize);
    const auto start = lh0.time();
    lbl->SkipTo(lbl->Lines() - 1);
    zfs::LineHeader lhn(lbl->L()->LPtr(), lbl->LineInfo()->lineHeaderSize);
    const auto end = lhn.time();
    lbl->SkipTo(0);
    return (double)(end - start) / 1000.;
}

//========================================================================================================================

// Very fast access to read header of ZFS file. The ZFS file header is dynamic.

void SampleReadHeader( const std::wstring& zfsfile )
{
    ZFSHeader h;
    if( h.Read( zfsfile ) )
    {
        // print out full header
        // if you need a description as well, you have to read the zfs.ini file and find the corresponding index
        ZFSHeaderItem* i = h.First();
        while( i )
        {
            int index = i->Index();
            if( index!=-1 && index!=65535 )
            {
                if( i->Typ()==0 ) // long/double
                {
                    if( i->Len()==ZFSHeaderItem::LEN_DOUBLE )
                        cout << "double " << index << ":" << i->Dbl() << endl;
                    else if( i->Len()==ZFSHeaderItem::LEN_FLOAT2 )
                        cout << "2xfloat " << index << ":" << i->Float1() << "," << i->Float2() << endl;
                    else
                        cout << "long " << index << ":" << i->DWord() << endl;
                }
                else if( i->Typ()==1 ) // array
                {
                    cout << "string " << index << ":" << i->Str() << endl;
                }
            }
            i = h.Next(); // next item
        }
    }
}

//========================================================================================================================

// Sample how to change/add item to zfs-header. The header indices are descibed in zfs.ini file.

bool SampleChangeHeader( const std::wstring& zfsfile )
{
    ZFSHeader h;
    if( h.Read( zfsfile ) ) // read only header of zfs-scan
    {
        h.SetDouble( 66000, 1. );				//example set a new header index double value
        h.SetLong( 66001, 2 );					//example set a new header index long value
        h.SetStringW( 66002, L"Test" );         //example set a new header index string value

        // write registrations matrix back to scan
        double m[16];
        memset( m, 0, sizeof(m) );
        m[0] = m[5] = m[10] = m[15] = 1.;
        for( int i=0; i<16; i++ )
            h.SetDouble( 320+i, m[i] );

        return h.Write(); // write header back
    }
    return false;
}


//========================================================================================================================

//
// Calculates dimensions of a ZFS scan file. Get resolution of high/low encoder, get start angle, get scan presets
// Some of the parameters are only valid for specific scanner types or since a certain firmwre version.

bool DetectDim( ZFSLinebyline* zf )
{
    if (!zf) return false;

    printf( "------ Dim -------\n" );

    ZFSHeader& hd = *zf->GetZFSHeader();

    int model = hd.getLong( 18, 0 ); // only Imager5006 otherwise 0
    int pixel = hd.getLong( 2, 0 );    
    int lines = hd.getLong( 4, 0 );
    
    double henc = hd.GetLong( 73, 200000 ); // encoder resolution
    double lenc = hd.GetLong( 74, 200000 ); // encoder resolution
    if( henc==0 || lenc==0 )
        return false;
        
    int start1 = hd.getLong( 61, 0 ); // high-speed (vertical) selection 1
    int stop1  = hd.getLong( 62, 0 );
    int start2 = hd.getLong( 63, 0 ); // high-speed (vertical) selection 2, <=0 if not used
    int stop2  = hd.getLong( 64, 0 );
    
    int sub = hd.GetLong( 65, 0 ); // used sub-sample for high-speed (vertical) encoder
    
    // only Imager5006 (horizontal)
    double lfrom = hd.GetDouble( 67, 0. ); // low-speed (horizontal) from
    double lto   = hd.GetDouble( 68, 0. ); // low-speed (horizontal) to
    double lfield= hd.GetDouble( 69, 0. ); // low-speed (horizontal) field
    
    // calculate high-speed (vertical)  angles (vertical)
    double hfrom1 = (double)start1 * 360. / henc;
    double hto1 = (double)(stop1+sub) * 360. / henc;
    if( hto1>360. )
        hto1 = 360.;
    double hfrom2 = 0.;
    double hto2 = 0.;
    if( start2 )
    {
        hfrom2 = (double)start2 * 360. / henc;
        hto2 = (double)(stop2+sub) * 360. / henc;
        if( hto2 > 360. )
            hto2 = 360.;
    }
    
    
    int maxh  = (int)henc;
    int maxh2 = maxh/2;
    
    if( model==2 ) // profiler
    {
        printf( "Profiler\n" ); // do not do anything in profiler mode
    }
    else if( model<=1 )
    {    
        // horz and vert selection
        double horfrom1 = 0.; // low speed motor, rotation
        double horto1 = 0.;
        double vertfrom1 = 0.; // high speed motor
        double vertto1 = 0.;

        // hor (rotation)
        if( lfrom || lto )
        {
            horfrom1 = lfrom;
            horto1 = lto;
        }
        else // must be detected from last line in scan, can take some time
        {
            zf->SkipTo(0);
            if (zf->Set(0))
            {
                double h = 0, l = 0;
                zf->Angle(h, l); // get angle of first pixel
                horfrom1 = TOGRAD(l);
                int ln = zf->SkipTo(-1); // go to last line
                int lnn = zf->Lines();
                if (zf->Set(pixel - 1))
                {
                    zf->Angle(h, l);
                    horto1 = TOGRAD(l);
                }
            }
        }
        // vert up-down
        vertfrom1 = hfrom1;
        vertto1 = hto1;

        int w = 0; // mapped width
        int h = 0; // mapped height

        // 2 selections available
        // BOTH in two
        if( stop2 && stop2 > stop1 )
        {
            printf( "BOTH in two\n" );
            w = lines * 2;
            h = pixel / 2;
        }

        // if start2 greater than 180deg, than just right side
        // RIGHT
        // imager5006 right from display
        // imager5003 opposite of key side
        else if( start1 >= maxh2 )
        {
            printf( "RIGHT\n" );
            w = lines;
            h = pixel;
        }

        // if stop1 less than 180deg and not stop2 than only left side
        // LEFT
        else if( stop1 <= maxh2 )
        {
            printf( "LEFT\n" );
            w = lines;
            h = pixel;
        }

        // left and right in one section (normal operation)
        // BOTH in one
        else if( stop1 > maxh2 )
        {
            printf( "BOTH in one\n" );
            w = lines * 2;
            h = pixel / 2;
        }

        // unknown
        else
        {
            printf( "UNKNOWN\n" );
        }

        printf( "Mapped w:%d h:%d\n", w, h );
        printf( "Mapped hor: %.1f - %.1f vert: %.1f - %.1f\n", horfrom1,horto1, vertfrom1, vertto1 );
    }

    printf( "------------------\n" );
    return true;
}