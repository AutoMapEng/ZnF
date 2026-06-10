#pragma once
#include "zfsapi.h"
#include "compiler.h"

#include <string>
#include <utility>
#include <memory>
#include <stdint.h>

#include "zfrgb.h"
#include "Math/zfvec.h"


class ZFSLinebyline;



namespace zfs
{
    #define LAYER_FOLDER    ".layer"
    #define LAYER_LFOLDER   L".layer"
    #define LAYER_EXT       ".zfl" ///< file extension used for layer files
    #define LAYER_LEXT      L".zfl" ///< file extension used for layer files

    class ZFLayerStorer;
    class ZFLayerStorerMemmap;

    /**
    \brief a layer can hold additional data for each pixel like a normal vector
           The underlaying code can maybe implement different kind of storer for example tiff or memorymapped
           Order is like in the linebyline format (zfs raw format order)
           When scan are splitted into severel parts, each part has its own layer file
           Layer could be the colorinformation, temperature, masks, quality or normal vector for example
           The code works for profilerscans as well. So it allows to store masks or color for example
           The typical filename would be <scanname.<layername>.zfl
    */
    class ZFS_API ZFLayer
    {
        friend class ZFLayerStorer;
        friend class ZFLayerStorerMemmap;

    public:
        ZFLayer(uint64_t ele);
        virtual ~ZFLayer();

        // !!! achtung reihenfolge oder nummern duerfen nicht nachtraeglich geaendert werden, nur anfuegen erlaubt !!!
        enum typ_t: uint8_t
        {
            TYP_UNDEF         = 0,
            TYP_Bit           = 1,
            TYP_I8            = 20,
            TYP_I16           = 21,
            TYP_I32           = 22,
            TYP_Float         = 30,
            TYP_Double        = 31,
            TYP_FloatPair     = 32,
            TYP_DoublePair    = 33,
            TYP_Vec_Float     = 40,
            TYP_Vec_Double    = 41,
            TYP_Vec_24bit     = 42,
            TYP_I24compressed = 43,
            TYP_Vec_16bit     = 44,
            TYP_I16Pair       = 50,
            TYP_RGB_byte      = 60,
            TYP_Cube_24bit    = 70
        };

        /// create a empty file
        static bool create(uint64_t w, uint64_t h, uint64_t typeCreate, uint64_t sz, const std::wstring& file, bool initialValue=false);

        /// create a abs file name
        static std::wstring buildLayerName(const std::wstring& zfsAbsFile, const std::wstring& layerName, const std::wstring& subName=L"");

        /// check whether layer already exists
        static bool exist( const std::wstring& zfsAbsFile, const std::wstring& layerName, const std::wstring& subName = L"");



        /// create a layer file depending on elementsize and type
        virtual bool createLayer(uint64_t w, uint64_t h, const std::wstring& file, bool initialValue = false);

        virtual typ_t type() const { return typ_t::TYP_UNDEF; }

        bool isType(const ZFLayer&) const;

        /// check for same dimension (width and height), elementsize does not matter
        bool hasSameDim(const ZFLayer&) const;

        /// create and open
        bool createAndOpen(uint64_t w, uint64_t h, const std::wstring& file, bool initialValue = false);

        /// open the file
        enum open_t { READWRITE, READONLY };
        bool open(const std::wstring& fn, open_t enableWriting);
        bool isOpen() const;
        bool canWrite() const;
        /// close the file
        void close();

        /// fille the data
        void fillWithZero();

        /// access
        char *ptr(int u, int v);
        const char *const_ptr(int u, int v) const;
        bool isIn(int u, int v) const { return u >= 0 && u < width && v >= 0 && v < height; }

        int32_t getWidth() const { return width; }
        int32_t getHeight() const { return height; }
        uint64_t getElementsize() const { return elementsize; } 

        const std::wstring& fileName() const;

    protected:
        std::shared_ptr<ZFLayerStorer> storer;  ///< the implemented storer
        std::wstring layername;
        int32_t width; // 2.1 GLines or GPixel should serve a very long time 
        int32_t height;
        uint64_t elementsize;       ///< the size of each element in the file
    };

    //=====================================================================================

    /*
        !! template funktioniert nicht da sizeof() mit pack(1) nicht den richtigen Wert liefert 
    */


    class ZFS_API ZFLayerBit : public ZFLayer
    {
    public:
        ZFLayerBit() : ZFLayer(1) {}
        virtual typ_t type() const override { return TYP_Bit; }

        enum class value_t : bool
        {
            VALID = true,
            INVALID = false
        };

        bool getVal(int u, int v) const;
        void setVal(int u, int v, bool newval);

        /// fill all with val
        void fill(bool val);

        /// count how many are val
        uint64_t count(bool val) const;
        std::pair<int, int> find(bool val) const;

    };

    //-----------------------------------------------

    class ZFS_API ZFLayerI8 : public ZFLayer
    {
    public:
        ZFLayerI8() : ZFLayer(1) {}
        virtual typ_t type() const override { return TYP_I8; }

        uint8_t getVal(int u, int v) const;
        void setVal(int u, int v, uint8_t newval);
    };

    //-----------------------------------------------

    class ZFS_API ZFLayerI16 : public ZFLayer
    {
    public:
        ZFLayerI16() : ZFLayer(2) {}
        virtual typ_t type() const override { return TYP_I16; }

        uint16_t getVal(int u, int v) const;
        void setVal(int u, int v, uint16_t newval);
    };

    //-----------------------------------------------

    class ZFS_API ZFLayerI32 : public ZFLayer
    {
    public:
        ZFLayerI32() : ZFLayer(4) {}
        virtual typ_t type() const override { return TYP_I32; }

        uint32_t getVal(int u, int v) const;
        void setVal(int u, int v, uint32_t newval);
    };

    //-----------------------------------------------

    class ZFS_API ZFLayerFloat : public ZFLayer
    {
    public:
        ZFLayerFloat() : ZFLayer(sizeof(float)) {}
        virtual typ_t type() const override { return TYP_Float; }

        float getVal(int u, int v) const;
        void setVal(int u, int v, float newval);

    };

    //-----------------------------------------------

    class ZFS_API ZFLayerDouble : public ZFLayer
    {
    public:
        ZFLayerDouble() : ZFLayer(sizeof(double)) {}
        virtual typ_t type() const override { return TYP_Double; }

        double getVal(int u, int v) const;
        void setVal(int u, int v, double newval);

    };

    //-----------------------------------------------

    class ZFS_API ZFLayerI16Pair : public ZFLayer
    {
    public:
        ZFLayerI16Pair() : ZFLayer(2*2) {}
        virtual typ_t type() const override { return TYP_I16Pair; }

        std::pair<uint16_t,uint16_t> getVal(int u, int v) const;
        void setVal(int u, int v, const std::pair<uint16_t,uint16_t>& newval);

        void setVal( int u, int v, uint16_t newval_1, uint16_t newval_2 );
    };

    //-----------------------------------------------

    class ZFS_API ZFLayerFloatPair : public ZFLayer
    {
    public:
        ZFLayerFloatPair() : ZFLayer(sizeof(float)*2) {}
        virtual typ_t type() const override { return TYP_FloatPair; }

        std::pair<float,float> getVal(int u, int v) const;
        void setVal(int u, int v, const std::pair<float,float>&);

    };

    //-----------------------------------------------

    class ZFS_API ZFLayerDoublePair : public ZFLayer
    {
    public:
        ZFLayerDoublePair() : ZFLayer(sizeof(double)*2) {}
        virtual typ_t type() const override { return TYP_DoublePair; }

        std::pair<double,double> getVal(int u, int v) const;
        void setVal(int u, int v, const std::pair<double,double>&);

    };

    //-----------------------------------------------

    class ZFS_API ZFLayerVec3f : public ZFLayer
    {
    public:
        ZFLayerVec3f() : ZFLayer(sizeof(float)*3) {}
        virtual typ_t type() const override { return TYP_Vec_Float; }


        zfs::math::Vec3f getVal(int u, int v) const;
        void setVal(int u, int v, const zfs::math::Vec3f&);
    };

    //-----------------------------------------------

    class ZFS_API ZFLayerVec3d : public ZFLayer
    {
    public:
        ZFLayerVec3d() : ZFLayer(sizeof(double) * 3) {}
        virtual typ_t type() const override { return TYP_Vec_Double; }

        zfs::math::Vec3d getVal(int u, int v) const;
        void setVal(int u, int v, const zfs::math::Vec3d&);
    };

    //================================================================================

    /// a vector with 24bit for each value for storing range very compressed
    class ZFS_API ZFLayerVec3_24bit : public ZFLayer
    {
    public:
        ZFLayerVec3_24bit() : ZFLayer(3 * 3) {}
        virtual typ_t type() const override { return TYP_Vec_24bit; }

        zfs::math::Vec3d getVal(int u, int v) const;
        void setVal(int u, int v, const zfs::math::Vec3d&);
    };

    //================================================================================

    /// a vector with 24bit for each value for storing range very compressed
    class ZFS_API ZFLayerCube_24bit : public ZFLayer
    {
    public:
        int depth;

        ZFLayerCube_24bit(int depth_= 8) : ZFLayer(depth_* uint64_t(3)), depth(depth_) {}
        virtual typ_t type() const override { return TYP_Cube_24bit; }

        double getVal(int u, int v, int d) const;
        void setVal(int u, int v, int d, const double&);
    };

    //================================================================================

    /// a vector with 16bit for each value for storing a float from 0..1 very compressed
    class ZFS_API ZFLayerVec3f_16bit : public ZFLayer
    {
    public:
        ZFLayerVec3f_16bit() : ZFLayer(sizeof(int16_t) * 3) {}
        virtual typ_t type() const override { return TYP_Vec_16bit; }

        zfs::math::Vec3f getVal(int u, int v) const;
        void setVal(int u, int v, const zfs::math::Vec3f&);
    };

    //================================================================================

    /// a value with 24bit for storing range and intensity very compressed
    class ZFS_API ZFLayerI24_compressed : public ZFLayer
    {
    public:
        ZFLayerI24_compressed() : ZFLayer(2) {}
        virtual typ_t type() const override { return TYP_I24compressed; }

        int getVal(int u, int v) const;
        void setVal(int u, int v, int);
    };


    //================================================================================

    /// RGB 0..255
    class ZFS_API ZFLayerRGB_byte : public ZFLayer
    {
    public:
        ZFLayerRGB_byte() : ZFLayer(3) {}
        virtual typ_t type() const override { return TYP_RGB_byte; }

        zfs::math::rgb getVal(int u, int v) const;
        void setVal(int u, int v, zfs::math::rgb);
    };


    //================================================================================
}
