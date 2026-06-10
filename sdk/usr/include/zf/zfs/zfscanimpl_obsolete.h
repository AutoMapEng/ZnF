#pragma once


//================================================================================================================




/**
*	ILAR compressed encoder
*/

#ifdef SUPPORT_ZFS18

class ZFS_API ZFScanZFS18 : public ZFScanZFS16
{
    DEFINE_ZFBASE(ZFScanZFS16)

public:

    virtual const char* DataTyp() const override { return "ZFSIlar"; };

    virtual bool Angle0( double& henc, double& lenc ) const override;
    virtual bool XYZ0( double& x, double& y, double &z ) const override;
    virtual bool XYZ0( double rg, double henc, double lenc, double& x, double& y, double &z ) const override;
};

#endif // SUPPORT_ZFS18

//================================================================================================================

/**
*	ILAR uncompressed encoder, additional coarse range byte
*/

#ifdef SUPPORT_ZFS200

class ZFS_API ZFScanZFS200 : public ZFScanZFS17
{
    DEFINE_ZFBASE(ZFScanZFS17)

public:

    virtual const char* DataTyp() const override { return "ZFSIlar"; };
};

#endif // SUPPORT_ZFS200

//================================================================================================================

/**
*	ILAR compressed encoder, additional coarse range byte
*/

#ifdef SUPPORT_ZFS201

class ZFS_API ZFScanZFS201 : public ZFScanZFS18
{
    DEFINE_ZFBASE(ZFScanZFS18)

public:

    virtual const char* DataTyp() const override { return "ZFSIlar"; };

    virtual bool Angle0( double& henc, double& lenc ) const override;
    virtual bool XYZ0( double& x, double& y, double &z ) const override;
    virtual bool XYZ0( double rg, double henc, double lenc, double& x, double& y, double &z ) const override;

	char getRgc() const;
};

#endif // SUPPORT_ZFS200

