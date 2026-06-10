#pragma once

/**
 * @brief Scan to Panorama image mapper
 *
 * Mapps scan cooridnates into panorama images and back.
 * Panorama image is defined to cover horizontally 360° and vertically 180°.
 *
 * ** Coordinate Systems
 * * Scan
 * The scan is addressed using pixel/line tuple. Here we use QPoint for this
 * where the x coordinate corresponds to pixel and y to line. Please note that
 * QRect/QPoint are designed to address QImages and co. These coorinate systems
 * run from top to bottom!
 *
 * * Panorama
 * The image data is addressed using column/row tuple. Here also we use QPoint
 * for this. The column coordinate (X) goes from left to right and the row (Y)
 * cooridnate from top to bottom.
 */

#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <vector>

#include <QRect>
#include <QPoint>

#include "ZfsToolsAPI.h"

#if (defined(_MSC_VER) && _MSC_VER <= 1800)
#define SPM_NO_CXX11
#define SPM_NOEXCEPT

#include <QString>
#else
#define SPM_NOEXCEPT noexcept
#endif

class ZFSLinebyline;
class ZFScan;

namespace zfstools
{
namespace priv {
#ifdef SPM_NO_CXX11
    static std::string to_string(const QPoint& p) {
        return QString("%1/%2").arg(p.x(), p.y()).toStdString();
    }
#else
    static std::string to_string(const QPoint& p) {
        return std::to_string(p.x()) + "/" + std::to_string(p.y());
    }
#endif
}

/**
 * @brief Defines a 1:1 mapping between scan coordinates and panorama coorinates
 */
class ScanPanoMapping
{
public:
    ScanPanoMapping()
        : m_front(false)
    { }

    ScanPanoMapping(const QRect& scan, const QPoint& pano, bool front)
        : m_front(front)
        , m_s(scan)
        , m_p(pano, scan.size().transposed())
    { }

    /**
     * @brief Check weather the scan area contains the point p
     * @param p Scan coords [pix, line]
     * @return true if point is contained within scan rect
     */
    bool scan_contains(const QPoint& p) const
    {
        return m_s.contains(p);
    }

    /**
     * @brief Check weather the panorama area contains the point p
     * @param p Panorama coords [col, row]
     * @return true if point is contained within pano rect
     */
    bool pano_contains(const QPoint& p) const
    {
        return m_p.contains(p);
    }

    const QRect& scan_area() const
    {
        return m_s;
    }

    /**
     * @brief Change scan area
     *
     * The size of the panorama area is ajusted to match those of the scan area.
     * But note: At this point it's not clear weather the panorama area should
     * be anchored on top left or bottom right coordinate. So you may need to
     * replace the panorama.
     *
     * @param r Scan data area [pix/line]
     */
    void set_scan_area(const QRect& r)
    {
        m_s = r;
        m_p.setSize(r.size().transposed());
    }

    const QRect& pano_area() const
    {
        return m_p;
    }

    /**
     * @brief Replace top left coordinate of panorama area
     *
     * Size doesn't change
     *
     * @param p Point to place [col,row]
     */
    void move_pano_area_tl(const QPoint& p)
    {
        m_p.moveTopLeft(p);
    }

    /**
     * @brief Replace bottom left coordinate of panorama area
     *
     * Size doesn't change
     *
     * @param p Point to place [col,row]
     */
    void move_pano_area_bl(const QPoint& p)
    {
        m_p.moveBottomLeft(p);
    }

    /**
     * @brief Map scan coords to panorama coords
     * @param s Scan coords [pix, line]
     * @return Panorama coords [col, row]
     */
    QPoint to_pano(const QPoint& s) const
    {
        const auto off = s - m_s.topLeft();
        if (m_front) {
            return QPoint(m_p.left() + off.y(), m_p.bottom() - off.x());
        } else {
            return QPoint(m_p.left() + off.y(), m_p.top() + off.x());
        }
    }

    /**
     * @brief Map panorama coords to scan
     * @param p panorama coords [col,row]
     * @return scan coords [pix, line]
     */
    QPoint to_scan(const QPoint& p) const
    {
        const auto off = p - m_p.topLeft();
        if (m_front) {
            return QPoint(m_s.right() - off.y(), m_s.top() + off.x());
        } else {
            return QPoint(m_s.left() + off.y(), m_s.top() + off.x());
        }
    }

    /**
     * @brief Check if both rects are of the same size. This is essential for
     *        working 1:1 mapping
     *
     * @return true if rects are of same size
     */
    bool is_valid() const SPM_NOEXCEPT
    {
        return m_s.size().transposed() == m_p.size();
    }

    bool is_on_front() const SPM_NOEXCEPT
    {
        return m_front;
    }

    bool is_on_back() const SPM_NOEXCEPT
    {
        return !is_on_front();
    }

private:
    bool  m_front;  ///< scan data is vertically at front (vert <= 180°)
    QRect m_s;      ///< scan area
    QRect m_p;      ///< panorama area
};


/**
 * @brief Error thrown by ScanToPanoMapper if point isn't mappable
 */
class MapError: public std::invalid_argument
{
public:
    MapError(const std::string& s)
        : invalid_argument(s)
    { }
};


/**
 * @brief Map scan coords to panorama coords and back
 */
class ZFSTOOLS_API ScanToPanoMapper
{
public:
    ScanToPanoMapper()
        : m_sub_fac(1)
    { }

    ScanToPanoMapper(const std::vector<ScanPanoMapping>& m, int sub)
        : m_sub_fac (std::max(1, sub))
        , m_mappings (m)
    { }

    ScanToPanoMapper(std::vector<ScanPanoMapping>&& m, int sub)
        : m_sub_fac (std::max(1, sub))
        , m_mappings (std::move(m))
    { }

    /**
     * @brief Map scan coords to panorama coords
     *
     * Does not throw, instead sets ok to true if successfull
     *
     * @note Returned coordinates may be well outside of a created image!
     *
     * @param p  Scan coords [pixel/line]
     * @param [out] ok set to true if mapping was successfull
     * @return Pano coords [col/row] if successfull, else default
     *         constructed QPoint
     */
    QPoint to_pano(const QPoint& p, bool& ok) const SPM_NOEXCEPT
    {
        const auto* map = get_to_pano_mapping(p);

        ok = map;
        return map ? map->to_pano(p) / m_sub_fac
                   : QPoint ();
    }

    /**
     * @overload
     *
     * @note Returned coordinates may be well outside of a created image!
     *
     * @param p Scan coordinates [pixel/line]
     * @return Pano coords [col/row]
     * @throws MapError if mapping is not possible
     */
    QPoint to_pano(const QPoint& p) const
    {
        auto ok = false;
        const auto tmp = to_pano(p, ok);

        if (!ok) {
            throw MapError(std::string("unable to map scan coords ") +
                           priv::to_string(p) + " to panorama");
        }

        return tmp;
    }

    /**
     * @brief Map Panorama coordinates back to scan
     *
     * Does not throw.
     *
     * @param p   Pano coords [col/row]
     * @param [out] ok set to true if mapping was successfull
     * @return Scan coords [pixel/line] if successfull, else default constructed
     *         QPoint
     */
    QPoint to_scan(QPoint p, bool& ok) const SPM_NOEXCEPT
    {
        p *= m_sub_fac;

        const auto* map = get_to_scan_mapping(p);

        ok = map;
        return map ? map->to_scan(p)
                   : QPoint ();
    }

    /**
     * @overload
     *
     * @param p Pano coords [col/row]
     * @return Scan coords [pixel/line]
     * @throws MapError if mapping was not possible
     */
    QPoint to_scan(const QPoint& p) const
    {
        auto ok = false;
        const auto tmp = to_scan(p, ok);

        if (!ok) {
            throw MapError(std::string("unable to map panorama coords ") +
                           priv::to_string(p) + " to scan");
        }

        return tmp;
    }

    /**
     * @brief Test weather scan coords p are mappable
     * @param p Scan coords [pixel/line]
     * @return true if coords are mappable
     */
    bool is_to_pano_mappable(const QPoint& p) const SPM_NOEXCEPT
    {
        return get_to_pano_mapping(p);
    }

    /**
     * @brief Test weather pano coords p are mappable to scan
     * @param p Pano coords [col/row]
     * @return true if coords are mappable
     */
    bool is_to_scan_mappable(const QPoint& p) const SPM_NOEXCEPT
    {
        return get_to_scan_mapping(p * m_sub_fac);
    }

private:
    const ScanPanoMapping*
        get_to_pano_mapping(const QPoint& p) const SPM_NOEXCEPT;
    const ScanPanoMapping*
        get_to_scan_mapping(const QPoint& p) const SPM_NOEXCEPT;

    int m_sub_fac;                           ///< image subsample factor
    std::vector<ScanPanoMapping> m_mappings; ///< mapping regions
};

/**
 * @brief Produce the mappings from a scan opened via LineByLine
 *
 * Cases not handled by now:
 *
 * * Scans with 2 vertical selections
 * * _rgb.zfs scans
 *
 * @param lbl Opened scan
 * @param [out] raw_pano_size The size of the panorama image this module
 *                            internally calculates
 * @return vector of mappings
 */
ZFSTOOLS_API std::vector<ScanPanoMapping> make_mappings(
        ZFSLinebyline& scan, QSize* raw_pano_size=nullptr);

/** @overload */
ZFSTOOLS_API std::vector<ScanPanoMapping> make_mappings(
        ZFScan& scan, QSize* raw_pano_size=nullptr);

/**
 * @brief Return the Panorama size for a given scan and subsample
 *
 * @note make_mappings() returns the same size with sub=1. This function
 *       accesses the scan and is possibly slow. If you need to set up the
 *       mappings anyway, you may use make_mappings() to get the size and the
 *       mappings in one go.
 *
 * @param lbl Opened scan
 * @param sub Subsample of panorama image (>= 1)
 * @return Size of panorama image [cols/rows]
 */
ZFSTOOLS_API QSize get_pano_size(ZFSLinebyline& lbl, int sub=1);

/**
 * @brief Convert image pixel coordinates into image angles
 *
 * Coordinate 0/0 is the top left point of image!
 *
 * @param img_size Size of the image (width and hight > 0!)
 * @param pos Coordinates within the image [col/row]
 * @return A pair of angles (horz,vert) [rad]
 */
ZFSTOOLS_API std::tuple<double, double>
pano_pos_to_angle(const QSize& img_size, const QPoint& pos);

/**
 * @brief Convert image angle coordinates into image col/row
 *
 * @param img_size Size of the image (width and hight > 0!)
 * @param horz Horizontal angle (0..360] [rad]
 * @param vert Vertical angle (0..180] [rad]
 * @return Image coordinates [col/row]
 */
ZFSTOOLS_API QPoint pano_angle_to_pos(
        const QSize& img_size, double horz, double vert);

} // namespace zfstools
