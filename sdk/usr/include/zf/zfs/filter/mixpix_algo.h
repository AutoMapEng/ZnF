#pragma once

#include "mixpix.h"


//##################################################################################################################################################

class ZFS_API ZFSFiltering : public ZFSFilter
{
public:
    ZFSFiltering() : ZFSFilter() {}
    ZFSFiltering(const ZFSFilter& prm) :ZFSFilter(prm) {}


    int mixHor( double range2, double range1 );
    int mixVert( double range2, double range1 );
    int mixDiag( double range2, double range1 );

    bool checkMinRef( double ref );
    bool checkMaxRef( double ref );
    bool checkIntDiffRange( double m, double rfinc );
    bool checkRangeFilter( double m );
//    bool checkMixedPixelFilterSingle( double d, double e, double f );
    bool checkMixedPixelFilter( bool pre, bool next, double a, double b, double c,  double d, double e,
                                double f, double g, double h, double i );

    bool checkSinglePix( bool maskA, bool maskB, bool maskC,
                         bool maskD,             bool maskF,
                         bool maskG, bool maskH, bool maskI,
                         double a, double b, double c,
                         double d, double e, double f,
                         double g, double h, double i );
    bool checkSinglePixLine( bool maskA, bool maskC, double a, double b, double c );
    bool checkSingleMaskPixel( bool maskA, bool maskB, bool maskC, bool maskD, bool maskF, bool maskG, bool maskH, bool maskJ, bool preNext );

//    bool checkConnectedPixel( double rg_a, double rg_b, int i_a, int i_b);

    /*
    int checkConnectedNeighbours( bool maskA, bool maskB, bool maskC,
                                  bool maskD,             bool maskF,
                                  bool maskG, bool maskH, bool maskI,
                                  int rf_a, int rf_b, int rf_c,
                                  int rf_d, int rf_e, int rf_f,
                                  int rf_g, int rf_h, int rf_i,
                                  double rg_a, double rg_b, double rg_c,
                                  double rg_d, double rg_e, double rg_f,
                                  double rg_g, double rg_h, double rg_i );
                                  */

    bool checkSpikes(const double& rg_a, const double& rg_b, const double& rg_c);
    bool checkSpikes(const double& rg_a, const double& rg_b, const double& rg_c, const double& rg_d, const double& rg_e);

    void overRfFilter(int pixels, uint32_t* rfCur, uint8_t* maskTyp, int& nFilteredOut);
    
//    double noniusJumpCorrectionSingle(double* rgCurrent, const unsigned int CurPixelPos, const unsigned int  filterWindowSize);
//    double noniusJumpCorrection( double* rgPre, double* rgCurrent, double* rgNext, const unsigned int i, const unsigned int sizeWide);
    bool noniusFilter( double rgm4, double rgm3, double rgm2, double rgm1, double rgcurrent, double rgp1, double rgp2, double rgp3, double rgp4, int curRf );
    bool jumpFilter( double rgm1, double rg, double rgp1, double rgpre, double rgnext );
//    bool jumpFilterSingle( double rgm1, double rg, double rgp1 );

//    void smallConnectedObjectsSingle( const double* const rgCurrent, const uint32_t* const rfCurrent, const bool* const maskIn, bool* const maskOut, const int Npixel );
//    void largeConnectedObjectsSingle( const double* const rgCurrent, const uint32_t* const rfCurrent, const bool* const maskIn, bool* const maskOut, const int Npixel );

/*    void smallObjectsFilter(const double* const rgPre, const uint32_t* const rfPre,
                             const double* const rgCurrent, const uint32_t* const rfCurrent,
                             const double* const rgNext   , const uint32_t* const rfNext   ,
                             const bool* const maskIn, bool* const maskOut, const int Npixel);
*/

//    void getNoniusMultiplier( const double& dD, const double& D1, const int D1_search_range[], const double& D2, const int D2_search_range[], int32_t& multiplier1, int32_t& multiplier2, double& dDrem );


//    void setMixedPixelFilterParameter( const double& a_mix_rad, const double& a_vert_angle_sampling_rad, const double& a_hor_angle_sampling_rad ); // required when subsampling is necessary (filter chain)


};

#include "mixpix_inl.h"
