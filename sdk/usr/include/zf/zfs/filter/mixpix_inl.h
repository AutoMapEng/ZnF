#pragma once
#include "mixpix_algo.h"

#include <cmath>


inline bool ZFSFiltering::checkMinRef( double ref )
{
    if( removeMinMaxRf )
    {
        if( ref < minRf )
        {
            return false;
        }
    }
    return true;
}


inline bool ZFSFiltering::checkMaxRef( double ref )
{
    if( removeMinMaxRf )
    {
        if( ref > maxRf )
        {
            return false;
        }
    }
    return true;
}


inline int ZFSFiltering::mixHor(double range2, double range1)
{
    double rej = 0.0;
    if (range1 < range2)
    {
        if (range1)
            rej = ((range2 - range1) * cosSubAngleHoriz) / (range1 * sinSubAngleHoriz);
    }
    else if (range2 < range1)
    {
        if (range2)
            rej = ((range1 - range2) * cosSubAngleHoriz) / (range2 * sinSubAngleHoriz);
    }
    else if (range2 == range1)
        return 0;

    return fabs(rej) > threshold ? 1 : 0;
}

inline int ZFSFiltering::mixVert(double range2, double range1)
{
    double rej = 0.0;
    if (range1 < range2)
    {
        if (range1)
            rej = ((range2 - range1) * cosSubAngleVert) / (range1 * sinSubAngleVert);
    }
    else if (range2 < range1)
    {
        if (range2)
            rej = ((range1 - range2) * cosSubAngleVert) / (range2 * sinSubAngleVert);
    }
    else if (range2 == range1)
        return 0;

    return fabs(rej) > threshold ? 1 : 0;
}

inline int ZFSFiltering::mixDiag(double range2, double range1)
{
    double rej = 0.0;
    if (range1 < range2)
    {
        if (range1)
            rej = ((range2 - range1) * cosSubAngleDiag) / (range1 * sinSubAngleDiag);
    }
    else if (range2 < range1)
    {
        if (range2)
            rej = ((range1 - range2) * cosSubAngleDiag) / (range2 * sinSubAngleDiag);
    }
    else if (range2 == range1)
        return 0;

    return fabs(rej) > threshold ? 1 : 0;
}

//
// check intensity in different ranges
//
// wenn weit entfernte pixel hohe intensity haben ist davon auszugehen das pixel im falschen
// intervall liegt und geloescht werden muss

inline bool ZFSFiltering::checkIntDiffRange( double m, double rfinc )
{
    if( rfrg )
    {
        if( m > 70. && rfinc > 300000 )
        {
            return false;
        }
        else if( m > 50. && rfinc > 500000 )
        {
            return false;
        }
    }
    return true;
}

inline bool ZFSFiltering::checkRangeFilter( double m )
{
    if (!range) {
        return true;
    }

    if( minRg )
    {
        if( m < minRg )
        {
            return false;
        }
    }
    if( maxRg )
    {
        if( m > maxRg )
        {
            return false;
        }
    }
    return true;
}

// a b c
// d e f
// g h i
// e ist das aktuelle Pixel

// linePre->Rgm(i-1) linePre->Rgm(i) linePre->Rgm(i+1)
// zeile->Rgm(i-1) zeile->Rgm(i) zeile->Rgm(i+1)
// lineNext->Rgm(i-1) lineNext->Rgm(i) lineNext->Rgm(i+1)

__forceinline bool ZFSFiltering::checkMixedPixelFilter( bool pre, bool next,
                                                 double a, double b, double c,
                                                 double d, double e, double f,
                                                 double g, double h, double i )
{
    if (!mixedpixel) return true;

    int mx = 0;
    int nn = 0;

    nn += mixHor( e, d );
    nn += mixHor( e, f );

    if( pre )
    {
        nn += mixVert( e, b );
        mx += 1;
    }

    if( next )
    {
        nn += mixVert( e, h );
        mx += 1;

        if( pre && bDiag ) // next + pre
        {
            nn += mixDiag( e, c );
            nn += mixDiag( e, a );
            nn += mixDiag( e, i );
            nn += mixDiag( e, g );
        }
    }

    if( nn > (mx ? nptNP : nptS) )
    {
        return false;
    }

    return true;
}

/*
__forceinline bool ZFSFiltering::checkMixedPixelFilterSingle( double d, double e, double f )
{
    if (!mixedpixel) return true;

    int nn = 0;

    nn += mixHor( e, d );
    nn += mixHor( e, f );

    if( nn>nptS )
    {
        return false;
    }

    return true;
}*/

/*
__forceinline bool ZFSFiltering::checkConnectedPixel( double rg_a, double rg_b, int i_a, int i_b )
{
    const double ns_a= RangeNoiseDataset.calcMM( i_a );
    const double ns_b= RangeNoiseDataset.calcMM( i_b );
    const double ns = smallObjectsNSigma * std::sqrt( ns_a* ns_a + ns_b* ns_b ) + 0.5;

    if(fabs( rg_a - rg_b )* 1e3 < ns)
    {
        return true;
    }
    else if(!mixVert( rg_a, rg_b ))
    {
        return true;
    }

    return false;
}
*/


/**
*	Single pixel filter
*	\param a => a1 linePre->Rgm(i-1)
*	\param b => a2 linePre->Rgm(i)
*	\param c => a3 linePre->Rgm(i+1)
*	\param d => b1 zeile->Rgm(i-1)
*	\param e => b2 zeile->Rgm(i)
*	\param f => b3 zeile->Rgm(i+1)
*	\param g => c1 lineNext->Rgm(i-1)
*	\param h => c2 lineNext->Rgm(i)
*	\param i => c3 lineNext->Rgm(i+1)
*/
__forceinline bool ZFSFiltering::checkSinglePix( bool maskA, bool maskB, bool maskC,
                                              bool maskD,             bool maskF, 
                                              bool maskG, bool maskH, bool maskI, 
                                              double a, double b, double c,  
                                              double d, double e, double f, 
                                              double g, double h, double i )
{
    int nn = 0;
    double dist = e/singleD;
    if( dist<0.01 )
        dist = 0.01;
    else if( dist>10 )
        dist=10;
#ifdef _DEbUG
    if( !maskD || !maskF || !maskA || !maskB || !maskC || !maskG || !maskH || !maskI )
        int dd = 0;
#endif
    // mask gesetzt wenn pixel ok (nicht gefiltert)
    // dist ok wenn differenz klein
    if( maskD && fabs(e-d)<dist )
        nn++;
    if( maskF && fabs(e-f)<dist )
        nn++;

    if( maskA && fabs(e-a)<dist )
        nn++;
    if( maskB && fabs(e-b)<dist )
        nn++;
    if( maskC && fabs(e-c)<dist )
        nn++;

    if( maskG && fabs(e-g)<dist )
        nn++;
    if( maskH && fabs(e-h)<dist )
        nn++;
    if( maskI && fabs(e-i)<dist )
        nn++;

    // wenn weniger gültige Pixel als definiert, dann auch dieses löschen
    // im idealfall alle 8 ok
    // je kleiner der wert, desto weniger wird gefiltert
    if( nn >= nsinglePN )
    {
        return true;
    }
    return false;
}

__forceinline bool ZFSFiltering::checkSinglePixLine( bool maskA, bool maskC, double a, double b, double c )
{
    int nn = 0;
    double dist = b/singleD;
    if( dist<0.01 )
        dist = 0.01;
    else if( dist>10 )
        dist=10;
    if( maskA && !(fabs(b-a)>dist) ) nn++;
    if( maskC && !(fabs(b-c)>dist) ) nn++;
    if( nn < nsingleS )
        return false;
    return true;
}

__forceinline bool ZFSFiltering::jumpFilter( double rgm1, double rg, double rgp1, double rgpre, double rgnext )
{
    double g = rg / jumpF; //  1/10grenze
    if( g<0.01 )
        g = 0.01;
    else if( g>10 )
        g=10;
    int n = 0;
//    if (fabs(rg - rgm1) > g) n++;
    if (fabs(rg - rgp1) > g) n++;
    if (fabs(rg - rgpre) > g) n++;
    if (fabs(rg - rgnext) > g) n++;

    if( n >= nJumpsPN )
    {
        return false;
    }
    return true;
}

/*
__forceinline bool ZFSFiltering::jumpFilterSingle( double rgm1, double rg, double rgp1 )
{
    double g = rg / jumpF; //  1/10grenze
    if( g<0.01 )
        g = 0.01;
    else if( g>10 )
        g=10;
    int nn=0;
    if( fabs(rg - rgm1)>g )
        nn++;
    if( fabs(rg - rgp1)>g )
        nn++;
    if( nn >=  nJumpsS )
    {
        return false;
    }
    return true;
}
*/
