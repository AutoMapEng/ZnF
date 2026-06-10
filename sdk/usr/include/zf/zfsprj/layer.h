#ifndef _ZFPRJ_LAYERS_H_
#define _ZFPRJ_LAYERS_H_

/**
    classes for layers
*/

#include "ZfsPrjBasic.h"

#include "obj.h"
#include <zfcore/filename.h>

class ZFProject;

namespace zfprj
{
#include <string>
#include <list>

// color used for point to point measurements when no project is available
#define DEFAULT_LAYER_COLOR_R 57
#define DEFAULT_LAYER_COLOR_G 255
#define DEFAULT_LAYER_COLOR_B 120

typedef union union_bitfeld
{
    unsigned long value;
    struct bitfeld_16
    {
        unsigned int bit00 : 1;		//layer listview
        unsigned int bit01 : 1;		//shoot listview
        unsigned int bit02 : 1;		//viewfromeye listview
        unsigned int bit03 : 1;
        unsigned int bit04 : 1;
        unsigned int bit05 : 1;
        unsigned int bit06 : 1;
        unsigned int bit07 : 1;
        unsigned int bit08 : 1;
        unsigned int bit09 : 1;
        unsigned int bit10 : 1;
        unsigned int bit11 : 1;
        unsigned int bit12 : 1;
        unsigned int bit13 : 1;
        unsigned int bit14 : 1;
        unsigned int bit15 : 1;
        unsigned int bit16 : 1;
        unsigned int bit17 : 1;
        unsigned int bit18 : 1;
        unsigned int bit19 : 1;
        unsigned int bit20 : 1;
        unsigned int bit21 : 1;
        unsigned int bit22 : 1;
        unsigned int bit23 : 1;
        unsigned int bit24 : 1;
        unsigned int bit25 : 1;
        unsigned int bit26 : 1;
        unsigned int bit27 : 1;
        unsigned int bit28 : 1;
        unsigned int bit29 : 1;
        unsigned int bit30 : 1;
        unsigned int bit31 : 1;
    } bitfeld;
} ubf32;




class Layers;

//--------------------------------------------------------------------------------------------------------
/**
\brief A group of ZFPrjObj objects with same display properties.

Each ZFPrjObj belongs to a Layer which can be used to hide or set the color
for whole groups of objects.
*/
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class ZFSPRJ_API Layer
{
    friend class Layers;

protected:
    Layers* parent;
    zf::FileName name;	// Sichtbarer User Name
    std::string id;		    // unter dieser kennung wird layer im project gespeichert
    objlst_t objs;		    // objecte die diesem layer zugeordnet sind
    bool visible;
    int color;
    bool mExport;		    //Flag ob der Layer beim Export beruecksichtigt werden soll
    union_bitfeld mOpenFlag;		//Flag ob der Layer geoeffnet oder geschlossen dargestellt werden soll

public:
    Layer( Layers* parent, const std::wstring& n=L"", const char* id=NULL, bool vis=true );
    virtual ~Layer();

    Layers* Parent() { return parent; }
    const Layers* Parent() const { return parent; }

    const wchar_t* NameW() const { return name.w_str(); }
    void SetName( const std::wstring& name );

    void AddObj( ZFPrjObj* );
    void DelObj( ZFPrjObj* );

    const char* ID() const { return id.c_str(); }
    objlst_t& Objs() { return objs; }
    const objlst_t& Objs() const { return objs; }
    int N() const { return (int)objs.size(); }

    bool IsVisible() const { return visible; }
    void SetVisible( bool v ) { visible=v; }

    void setExport( bool bo ) {mExport = bo; }
    bool getExport() const { return mExport; }

    void setOpenFlag( unsigned long );
    unsigned long getOpenFlag() const;
    void setOpenFlag( bool b, int pos );
    bool getOpenFlag( int pos ) const;

    unsigned char red() const { return (color>>16)&0xff; }
    unsigned char green() const { return (color>>8)&0xff; }
    unsigned char blue() const { return color & 0xff; }
    void SetColor( int r, int g, int b ) { color = ((r&0xff)<<16) + ((g&0xff)<<8) + (b&0xff); }
    void SetColor( int c ) { color = c; }
};
//--------------------------------------------------------------------------------------------------------

class Messungen;
class MessLayer;
class ZFPolygons;
class ZFPlanes;
class ZFShootChannels;

typedef std::list<Layer*> layer_lst;

/// A node that holds all the Layer objects of a project.
class ZFSPRJ_API Layers
{
    friend class ZFProject;

protected:
    ZFProject* prj;
    layer_lst layers;
    Layer* active;

    Layer* AddLayer( const std::wstring& name, const char* id, bool vis );
    void Free();

public:
    Layers( ZFProject* p );
    virtual ~Layers();
    void Init(); // alle loeschen und einen anlegen und activieren

    void Load();
    void Store( double versNr );

    ZFProject* Project() { return prj; }

    int N();    // anzahl layers
    layer_lst& LayersArray() { return layers; }
    Layer* ActiveLayer();
    bool SetActiveLayer( const std::wstring& name );
    Layer* Find( const std::wstring& name );
    Layer* FindByID( const char* id );
    Layer* AddLayer( const std::wstring& name );
    bool DeleteLayer( const std::wstring& name );
    void DeleteAllLayers();

    void SetAllVisible( bool v=true );

    bool MoveObj( ZFPrjObj* o, const std::wstring& fromLayer, const std::wstring& toLayer ); // EIn object von einem layer in einen anderen verschieben
};

#pragma warning(pop)
//--------------------------------------------------------------------------------------------------------

} // end namespace zfprj

#endif //_ZFPRJ_LAYERS_H_
