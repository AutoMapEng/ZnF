#ifndef COORDINATETRANSFORMATION_H
#define COORDINATETRANSFORMATION_H

#pragma once
#include "zfsPrj.h"


enum ZFSPRJ_API eRegDir { REG_NORTH, REG_SOUTH };

class ZFSPRJ_API CoordTransform
{
	double m_a;		//roße Halbachse  	a  	[m]  
	double m_f;
	double m_if;	//inverse Abplattung 	1/f 	[-] 	
	double m_e;		//Exzentrizität 	e 	[-] 	= (2f - f2)1/2
	double m_e2;	//2. Exzentrizität 	e' 2 	[-] 	= e2 /(1 - e2)
	double m_lam0;	//Gitterdefinition 	Referenzmeridian 	lam0 	[°] 	
	double m_FE;	//'false Easting' 	FE 	[m] 	500000
	int m_Z;		//Zonennummer 	Z 	[m] 	= (lam0/3) × 1000000
	double m_k0;	//Skalierungsfaktor 	k0 	[-] 	1.000
	double m_lam0Rad;
	double m_phy0rad;
	double m_FN;
	double m_eq;

	double m_dx;
	double m_dy;
	double m_dz;
	double m_rx;
	double m_ry;
	double m_rz;
	double m_s;

public:
	CoordTransform( int lam0, eRegDir dir );
	~CoordTransform(void);

	void gogr2NEUTM( double phy, double lambda, double& E, double& N );
	void geogrCoor2NE( double phy, double lambda, double& E, double& N );
	void geoz2geogr( double X, double Y, double Z, double& phy, double& lambda, double& h );
	void geogr2Geoz( double phy, double lambda, double h, double& X, double& Y, double& Z );
	void helmertTransformation( double Xa, double Ya, double Za, double& Xt, double& Yt, double& Zt );

};

#endif
