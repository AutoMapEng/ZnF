#pragma once
#include <tuple>
#include <vector>
#include <stdint.h>

#include "zfsapi.h"

class ZFSHeader;
class ZFSLinebyline;
class ZFScan;

/**
    Map from physical to a visable image and back depending on mode
*/
class ZFS_API ZFScanMap
{
public:
    ZFScanMap();

    // detect mapping mode
    void DetectApprox(const ZFSHeader& hd);
    void Detect(const ZFSHeader& hd, int shift );
    void Detect(const ZFSHeader& hd, ZFScan*);
    void Detect(const ZFSHeader& hd, ZFSLinebyline*);

    bool valid() const { return map_width && map_height; }

    enum mode_t {
        MAP_NO,     ///< no transformation, physical mode
        MAP_RIGHT,
        MAP_LEFT,
        MAP_ROTFLIP_MIN, ///< show the minimal (default) both sides are filled
        MAP_BOTH,    ///< flip and rotate with offset (default mode)
        MAP_ROTFLIP_MAX, ///< one side may has no pixel
        MAP_UNDEF = -1,
    };

    /// force physical mode for example
//    void forceMode(mode_t);
//    mode_t forceMode() const;

    /// change the mode, width and high will be updated
    void changeMode(mode_t newMode);

    /// change to the default mode, maybe the mode set by forceMode
    void changeToDefaultMode();

    /// set/get mirror
    void setMirrorPix(bool pix);
    void setMirrorLin(bool lin);
    bool isMirrorPix() const { return mirrorPix; }
    bool isMirrorLin() const { return mirrorLin; }


    typedef std::pair<int32_t, int32_t> pxln_t; ///< pixel and line
    typedef std::pair<int32_t, int32_t> uv_t;   ///< u and v of the image

    /**
    There is maybe a not filled area in the view where are no pixels are available
    */
    bool physical2image(const pxln_t, uv_t&) const;

    /**
    ! when number of pixel is not even, not all position can be mapped. In this case it will return false
    */
    bool image2physical(const uv_t, pxln_t&) const;

    /// return physical number of pixel
    int ZFSPixel() const { return pixel; }

    /// return physical number of lines
    int ZFSLines() const { return lines; }

    /// current mode
    mode_t Mode() const { return mapmod; }

    /// return the default mode
    mode_t defaultMode() const;

    /// the mapped width
    int W() const { return map_width; }

    /// the mapped height
    int H() const { return map_height; }

    int HRound() const { return hRound; }

    /// the shift offset
    int HOffset() const { return highOffset1; }

    /// the first pixel of the second part
    int FirstSecond() const { return firstsecond; }

    /// return a string repesentation of the mode
    static std::string mode2string(mode_t m);

protected:
    void buildLUT();
    mode_t detectBestMode(const ZFSHeader&);

protected:
    int pixel{};          ///< orginal zfs file
    int lines{};          ///< orginal zfs file
    int pixelHalf{};      ///< half of pixel
    int pixelHalfRound{}; ///< half of pixel + 0.5

//    mode_t forcemod{};
    mode_t defaultmod{};
    mode_t mapmod{};      ///< current
    bool mirrorPix{};     ///< mirror pixel
    bool mirrorLin{};     ///< mirror line

    int map_width{};      ///< width gemappte ansicht
    int map_height{};     ///< height
    int hRound{};         ///< Width gemappte ansicht gerundet

    int highOffset1{};    ///< shift offset
    int firstsecond{};    ///< the first pixel of the second part = half + highoffset1

    std::vector<int32_t> lut_pysPx2lin;
};
