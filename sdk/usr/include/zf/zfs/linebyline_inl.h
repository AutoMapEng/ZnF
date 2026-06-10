


/**
*	Set the pixel of the current line.
*	\param pix pixelnumber
*	\param l not used
*	\return Returns true if successful.
*/
inline bool ZFSLinebyline::Set( int32_t pix, int /*l*/ )
{
    return zeile->Set( pix );
}



/**
*	Returns reflection of current pixel. You have to use Set( int32_t pix, int32_t l = 0 ) before.
*/

inline double ZFSLinebyline::RfComp() const
{
    return RfComp( L() );
}

inline double ZFSLinebyline::RfNorm1() const
{
    const double rf_ = RfNorm();
    return rf_ > 1. ? 1. : rf_;
}

inline int32_t ZFSLinebyline::MaxRf() const
{
    return L()->Info()->maxRf;
}

inline int32_t ZFSLinebyline::MinRf() const
{
    return L()->Info()->minRf;
}

inline zfs::math::rgb ZFSLinebyline::rgb()
{
    return rf.rgbrgb(zeile);
}

inline zfs::math::rgb ZFSLinebyline::rgb(const ZFLineAccess* l)
{
    return rf.rgbrgb(l);
}

inline zfs::math::rgb ZFSLinebyline::rgb( const ZFLineAccess* l, zfs::PData& px )
{
    return rf.rgbrgb(l,px);
}


/**
*	Returns the range offset in meter.
*/

inline double ZFSLinebyline::MOffset() const
{
    return zeile->MOffset();
}

inline double ZFSLinebyline::MScale() const
{
    return zeile->Info()->scaleM;
}

inline int32_t ZFSLinebyline::Rf( ZFLineAccess* l ) const
{
    return l ? l->Rf() : zeile->Rf();
}

/**
* angle for current line and pixel (use set before)
*/
inline void ZFSLinebyline::Angle0( int32_t u, int32_t v, double& henc_rad, double& lenc_rad )
{
    SkipTo( v );
    Set( u );
    if( zeile )
        model->Angle( zeile, zeile->Rgm(), henc_rad, lenc_rad );
}

inline void ZFSLinebyline::Angle0(double& henc_rad, double& lenc_rad)
{
    model->Angle(zeile, zeile->Rgm(), henc_rad, lenc_rad);
}

inline void ZFSLinebyline::XYZ0( double henc, double lenc, double rg, double& x, double& y, double &z ) const
{
    model->XYZ0( henc, lenc, rg, x, y, z );
}

/**
* Calculate form 2D pixel to 3D point for the current pixel without translation/rotation but with
* calibration if available.
*
* Use Set( int32_t pix, int32_t l ) to select the pixel.
*
* \param [out] x,y,z Coordinates for current pixel
* \param l Line to use, if nullptr use current line
*/
inline void ZFSLinebyline::XYZ0( double& x, double& y, double& z, const ZFLineAccess* l ) const
{
    const auto lp = l ? l : zeile;
    model->XYZ0( lp, lp->Rgm(), x, y, z );

    PostXYZ0Action( x, y, z, l?l:zeile);
}

inline void ZFSLinebyline::PreXYZ0( double& x, double& y, double& z )
{
    model->XYZ0( linePre, linePre->Rgm(), x, y, z );

    PostXYZ0Action( x, y, z, linePre );
}
inline void ZFSLinebyline::NextXYZ0( double& x, double& y, double& z )
{
    model->XYZ0( lineNext, lineNext->Rgm(), x, y, z );

    PostXYZ0Action( x, y, z, lineNext );
}

inline void ZFSLinebyline::PostXYZ0Action( zfs::math::Vec3d& v, const ZFLineAccess* l ) const
{
    Compensator( v, l );
}
inline void ZFSLinebyline::PostXYZ0Action(double& x, double& y, double& z, const ZFLineAccess* l) const
{
    Compensator(x, y, z, l);
}



// ----------------------------------------------------------------------------------------------------------

inline int32_t ZFSLinebyline::Rgmm(ZFLineAccess* l) const
{
    return l ? (int32_t)l->Rgmm() : (int32_t)zeile->Rgmm();
}

/**
*	Returns the range in ? as char.
*	If Typ isn't 201 returns 0.
*/

inline char ZFSLinebyline::getRgc() const
{
#ifdef ZFS_LINEACCESS_5003
    if( typ == 201 )
    {
        return static_cast<ZFLineAccess201*>( zeile )->getRgc();
    }
#endif
    return 0;
}

