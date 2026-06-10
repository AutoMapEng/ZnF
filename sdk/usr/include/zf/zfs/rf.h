#pragma once

#include "zfsapi.h"
#include "zfrgb.h"

#include <string>
#include <list>


class ZFScan;
class ZFSLinebyline;
class ZFLineAccess;
namespace zfs { class PData; }


/**
	\brief Holds parameter for setting the view-mode of a ZFScan.
*/
class ZFS_API ZFRfParam
{
public:
	// view mode
	enum {	M_LINEAR,	///< linear scaliert
		M_UNAHE,	///< optimal grayvalues
		M_RG,       ///< use range for grayview
		M_RGB,      ///< show rgb channel if colorimage
		M_RAINBOW,  ///< Regenbogen see RM_...
		M_RFCOMP,	///< mittels rf compensation wenn scanner es unterstuetzt
		M_RGB_OVERLAY, ///< overlay of custom image
		M_THERMAL_OVERLAY,	///< overlay with thermal image
		M_BLACK,		///< no intensity, just mask is selected
        M_QUALITY       ///< display a quality green to red image, depending on intensity and resolution
	};

	/// rainbow modes
	enum
	{
		RM_RF,		///< intensity linear
		RM_RFCOMP,	///< intensity compensated
		RM_RG,		///< range
		RM_X,		///< 3d x-value
		RM_Y,		///< 3d y-value
		RM_Z,		///< 3d z-value
		RM_NOISE    ///< noise image depending on intensity
	};

	/// color scale bound indices
	enum 
	{ 
		B_RF,       ///< Bounds for reflection view modes
		B_RG,       ///< Bounds for range view modes
		B_NOISE,    ///< Bounds for noise modes
        B_QUALITY,  ///< Bounds for quality view
		_B_         ///< End of sequence
	};

	int mod{ M_LINEAR };					///< current mode
	bool mask{true};					///< show filter masks
	bool minmax{true};				///< show min max intensity
	int rainMod{ RM_RF };                ///< rainbow mode
	double rainFrom{0}, rainTo{ 5000000 };	///< rainbow

public:
	ZFRfParam() = default;
	virtual ~ZFRfParam() {}
};



/**
	\brief Class for printing output image of scan
*/
class ZFS_API ZFRf : public ZFRfParam
{
public:
	enum
	{
		DETECT_UNAHE = 1,                       ///< used for optimized view depending on histogram
		DETECT_NORFCOMP = 2,                    ///< used for normalized intensity. Use this flag if you did not need that

		DEFAULT_DETECT_FLAGS = (DETECT_UNAHE)
	};

protected:
	bool ok{};
	ZFScan* zfs{};
	ZFSLinebyline* zflbl{};

	bool supportRgb{};    ///< color
	bool supportRfcomp{}; ///< compensated intensity
	bool supportUnahe{};  ///< grayvalue with histogram
	bool supportRange{};  ///< gray from range

	int  modOptimal{ M_LINEAR };        ///< Standardmode best mode too display
	static int modDefault;  ///< default mode which should be used if possible, may is not the optimal mode (default:M_RFCOMP)

	bool norgbgray{true}; ///< if no color print gray
	std::list<std::wstring> notUsedMasks; ///< list of the names of all not used masks
	std::wstring overlayImageFile;
	int overlayX{}, overlayY{}; // Startposition of the image

	int deep{32};       ///< needed bytes for viewing  default 32

	int rfMin{ 30000 }, rfMax{ 5000000 };
    double rgMin{0}, rgMax{ 10000. };  ///< min max range for qualty calculation 
	uint32_t colMin{ 0xff800000 }, colMax{ 0xffffff00 };

	// scale bounds for the scalable view-modes
	double from[_B_]{};
	double to[_B_]{};

	#define RAINBOW_COLS  (4*256)
    zfs::math::rgb colBogen[RAINBOW_COLS];

	int nLut{};
	uint32_t* lut{};

	void Reset();
	void CreateRainbow( int typ=1 );

	double fac[_B_]{};

	// noise view
	double noise_param_a{ 1.1807 };
	double noise_param_b{ -0.0037372 };

	int m_detectFlagsUsed{};

public:
	ZFRf();
	virtual ~ZFRf();

	ZFRf( ZFScan* );        // clone
	ZFRf( ZFSLinebyline* ); // clone
	ZFRf( ZFRf& );          // clone

	void Set( ZFRf& );	// copy
	void Set( ZFRf&, ZFSLinebyline* );	// copy

	void SetZfs( ZFScan* ); // for internal usage only
	ZFScan* Zfs() const { return zfs; }
	ZFSLinebyline* Zflbl() const { return zflbl; }

	void Detect( ZFScan*, int detectFlags=DEFAULT_DETECT_FLAGS, bool force=false );		///< detect and setup optimal viewing mode
	void Detect( ZFSLinebyline*, int detectFlags=DEFAULT_DETECT_FLAGS, bool force=false );		///< detect and setup optimal viewing mode
	void Detect( int detectFlags=DEFAULT_DETECT_FLAGS, bool force=false );		///< detect and setup optimal viewing mode
	bool Detected() const { return ok; }
	bool SupportRGB() const { return supportRgb; }
	bool SupportRfcomp() const { return supportRfcomp; }
	bool SupportUnahe() const { return supportUnahe; }
	bool SupportRange() const { return supportRange; }

	void DisableRfcompSupport();
	void DisableUnaheSupport();
	void DisableRGBSupport();
	void DisableRangeSupport();
	void EnableRGBSupport();
	void EnableRangeSupport();

	void NoColorGray( bool );       // If no color use gray value
	bool NoColorGray() const { return norgbgray; }

	int CreateLUT();				// create colortable for 8bit deep. neccesarry after masks added/removed
	int LUTn() const { return nLut; }		// number of colors if deep = 8
	int LUTa( int )const ;				// access color
    zfs::math::rgb LUTa_rgb( int ) const;			// access color

	static void SetModDefault( int );

    int SetDefaultMode();
	int SetMod( int, int rainbowmod=RM_RF );	// set viewing mode
	int SetMod( const ZFRfParam& p );
	int SetRainbowMod( int rainbowmod );
	int SetModOptimal();						// best default viewing mode
	int SetModNoRGB();              ///< Set to best gray-scale mode if rgb or overlay mode is active
	int SetBestGrayscaleMode();     ///< Set to best gray-scale mode.
	int Deep() const { return deep; }	// needed bytes for active viewing mode
	int GetMod() const;
	ZFRfParam GetModParam() const;
	int GetRainbowMod() const;
	int GetModOptimal() const { return modOptimal; }
	void ShowMask( bool );                  ///< Changes the visibility of all masks.
	void ShowMask( const wchar_t* name, bool show );
	void ShowMask( const wchar_t* name );   ///< Toggles the visibility of the mask.
	bool MaskShown( const wchar_t* name ) const;  ///< Returns true if a individual mask is visible.
    bool MaskShown(const std::wstring& name) const { return MaskShown(name.c_str()); }
    bool IsMask() const;  ///< Returns true if master show switch for masks is on.
	void ShowMinmax( bool );
	bool IsMinmax() const { return minmax; }

	int RfMin() const { return rfMin; }	// minimum intensity
	int RfMax() const { return rfMax; }	// maximum intensity
	double RfMinPercent() const;
	void SetRfMin( int );
	void SetRfMinPercent( double );
	void SetRfMax( int );

	int currentBoundType() const; /**< Returns the bound type (B_RF, B_RG, B_NOISE, _B_) used by the currently set view mode. _B_ is returned when the bound have no effect on the current view mode. */
	static int boundTypeOfViewMode( int primaryViewMode, int rainbowViewMode);
	void SetBounds( int boundIndex, double from, double to );	/**< Set color scale bounds for linear viewing, range and rainbow modes. boundIndex must be one of B_RF ... _B_*/
	void GetBoundsForBoundType( int boundIndex, double& lowerBound, double& upperBound) const;   /**< This can be used to retrieve all color scale bounds. boundIndex must be one of B_RF ... _B_*/
	double GetBoundFrom( int b ) const;
	double GetBoundTo( int b ) const;

    /// color for minimum intensity
	int ColMin() const { return colMin; }
    /// color for maximum intensity
	int ColMax() const { return colMax; }			
	void SetColMin( int c ) { colMin=c; }
	void SetColMax( int c ) { colMax=c; }

	const char* Info();

    /// return rgb (for 8bit mode, r g and b are the same)
    zfs::math::rgb rgb() const;
    /// independent from deep
	ZFrgb rgbrgb() const; 

	// for linebyline if other line should be used than current otherwise use just rgb()
    zfs::math::rgb rgb(const ZFLineAccess*) const;	// return rgb (for 8bit mode, r g and b are the same)
    zfs::math::rgb rgbrgb(const ZFLineAccess*) const; // independent from deep
    zfs::math::rgb rgb(const ZFLineAccess*, zfs::PData& px) const;
    zfs::math::rgb rgbrgb(const ZFLineAccess*, zfs::PData& px) const;

	//Overlay for thermal camera
	static std::wstring getFullFilenameThermalOverlayImage( std::wstring scanFilename );
	static bool hasThermalOverlayImage( std::wstring scanFilename );
	bool hasThermalOverlayImage() const;

	//Overlay
    /// delete current image
    void deleteOverlayImage(); 
    /// use x,y to set position (does not work for lbl)
	void SetOverlayImage( const std::wstring& imagefile, int x=0, int y=0, bool force=false );
    /// the file name of the overlay image
	std::wstring OverlayImage() const;
    /// load standard image if possible (only works with ZFScan, not with Linebyline)
	void DisableOverlayImage(); 
	bool IsOverlayImage() const;  // always false
	int OverlayX() const { return overlayX; }
	int OverlayY() const { return overlayY; }

	ZFRf& operator=( const ZFRf& rf );

	std::string Dump() const;

protected:
    zfs::PData getPixel(const ZFLineAccess* curLine) const;
};



namespace zfs
{
    /**
    @brief convert the intensity (24bit) to a compessed 16bit value
    */
    inline uint16_t rf24to16(uint32_t rf)
    {
        if (rf > 65535)
            return static_cast<uint16_t>((rf >> 8) | 1);
        else
            return static_cast<uint16_t>(rf & 0xfffe);
    }

    /**
    @brief convert the compressed intensity as it is stored (16bit) to a real number (24bit)
    */
    inline uint32_t rf16to24(uint16_t rf)
    {
        if (rf & 1)
            return static_cast<uint32_t>(rf & ~1) << 8;
        else
            return static_cast<uint32_t>(rf);
    }
}