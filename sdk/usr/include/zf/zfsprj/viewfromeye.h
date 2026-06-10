#ifndef _ZFPRJ_VIEWFROMEYE_H
#define _ZFPRJ_VIEWFROMEYE_H

#include "ZfsPrjBasic.h"
#include "obj.h"

#include <zfs/zfvector.h>

#include <string>

namespace zfprj {
    class ViewFromEyes;

    //=============================================================================================
    /// An object that represents the viewing position of a witness in a crime scene.
    class ZFSPRJ_API ViewFromEye : public ZFPrjObj {
        DEFINE_ZFBASE(ZFPrjObj)

            friend class ViewFromEyes;

    private:
        ZFVec3d mPos; //VFE Position des Bodenpunktes in Projektkoordinaten.

    public:
        double mHeight;

        bool mPlaceHuman;
        int mR;
        int mG;
        int mB;

        bool mShow;

    public:
        ViewFromEye(ViewFromEyes* p);
        ViewFromEye(ViewFromEyes* p,
            const std::wstring& name, const ZFVec3d& pos,
            double height, bool human, int r, int g, int b, bool show);

        void getXYZ( ZFVec3d& pos) const;

        std::string Xml()  const override { return "ViewFromEye"; }
        virtual const QPixmap* GetPixmap();
        static void clearPixmap();
    };

    //=============================================================================================

    /// The node that holds all ViewFromEye objects.
    class ZFSPRJ_API ViewFromEyes : public ZFPrjObjNode {
        DEFINE_ZFBASE(ZFPrjObjNode)

    private:
        static int nameCounter;                 ///< This is used for generating the default name of a plane.
        static std::wstring defaultNamePrefix;  ///< This is used for generating the default name of a plane.

    public:
        ViewFromEyes(ZFProject*);

        virtual bool Load();
        virtual bool Store(double versNr);

        QString Typ() const override { return QObject::tr("View from eyes"); }
        std::string Xml() const override { return "ViewFromEyes"; }  // xml sektion where to store

        ViewFromEye* AddViewFromEye( const std::wstring& name, zfs::math::Vec3d& pos, double height, bool human, int r, int g, int b, bool show);
        bool DelViewFromEye(const std::wstring& name);

        ViewFromEye* FindViewFromEyeByText(const std::wstring& text);
        bool deleteObj(const std::wstring& text);

        void Cleanup();

        static void setDefaultNamePrefix(std::wstring prefix) { defaultNamePrefix = prefix; }
        static std::wstring getDefaultName();
    };

    //=============================================================================================


} // end namespace zfprj

#endif //_ZFPRJ_VIEWFROMEYE_H
