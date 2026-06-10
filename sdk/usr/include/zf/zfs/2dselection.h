#pragma once

#include "zfsapi.h"

#include <vector>
#include <list>


//================================================================================================================

//
// helper class for holding point/rectangle of selection
//
class ZFS_API ZFPoint
{
public:
    int x{};
    int y{};

    ZFPoint() = default;
    ZFPoint( int _x, int _y ) { x=_x; y=_y; }
};

// rectangle selection in 2D space
class ZFS_API ZFRect
{
public:
    int left{};
    int top{};
    int right{};
    int bottom{};

    ZFRect() = default;
    ZFRect( int x1, int y1, int x2, int y2 ) { left=x1; top=y1; right = x2; bottom = y2; }
    ZFRect( const ZFRect& r ) { left=r.left; top=r.top; right=r.right; bottom=r.bottom; }

    int X() const       { return left; }
    int Y() const       { return top; }
    int Width() const   { return right-left+1;}
    int Height() const  { return bottom-top+1;}

    /// swap values left-right or top-bootom if neccesarry
    void normalize();

    /// adjust the extension (left will be max width-1, same for height). Check that left and top are not below zero
    void adjustExtension(int width, int height);
};

//================================================================================================================

/**
\brief Abstract class for a 2D selection in a ZFScan.
*/
class ZFS_API ZF2DSelection
{
public:
    ZF2DSelection() = default;
    virtual ~ZF2DSelection() {}

    enum{ LINE, RECT, POLY, FREE, USERDEFINED };

	virtual ZF2DSelection* copy() = 0;
	virtual bool isPointInPolygon( double x, double y ) = 0;
    virtual int Typ() const = 0;
    virtual void BoundingBox( ZFRect& ) const = 0;

    virtual void Normal() {}

    // userprogramm can implement drawing and mousehandling
    virtual void Draw() {}
};


//----------------------------------------------------------------------------------------------------------------

//
// Rectangle selection
//
class ZFS_API ZF2DSelectionRect : public ZF2DSelection
{
public:
    int left{};
    int top{};
    int bottom{};
    int right{};

public:
    ZF2DSelectionRect() : ZF2DSelection() { left=top=right=bottom=0; }
    virtual ~ZF2DSelectionRect() {}

    int Typ() const override { return RECT; }

	ZF2DSelection* copy() override { ZF2DSelectionRect* s = new ZF2DSelectionRect; s->left = left; s->top = top; s->bottom = bottom; s->right = right; return (ZF2DSelection*)s; }
	bool isPointInPolygon ( double x, double y ) override;
    void Normal() override;
    int Width() { return right - left + 1; }
    int Height() { return bottom - top + 1; }
    int Width() const { return right - left + 1; }
    int Height() const { return bottom - top + 1; }


    virtual void BoundingBox( ZFRect& b) const override { b.top=top; b.left=left; b.right=right; b.bottom=bottom; }
};

//================================================================================================================


//
// Polygon selection
//
typedef std::vector <ZFPoint> ptlist;

class ZFS_API ZF2DSelectionPoly : public ZF2DSelection
{
protected:
    ptlist              list;
    ptlist::iterator    it;
    ZFRect box;

public:
    ZF2DSelectionPoly() : ZF2DSelection() {;}
    virtual ~ZF2DSelectionPoly();

    int Typ() const override { return POLY; }

	ZF2DSelection* copy() override { ZF2DSelectionPoly* poly = new ZF2DSelectionPoly; poly->list = list; poly->it = list.begin(); poly->box = box; return (ZF2DSelection*)poly; }
    virtual void Add( const ZFPoint& p );
	virtual void Add( const std::vector<ZFPoint> &points );
    virtual void Add( ZF2DSelectionPoly* polySelection );
    virtual void AddFront( const ZFPoint& p );
    virtual void AddFront( ZF2DSelectionPoly* polySelection);
	void deleteLast();
	void exchangeLast(ZFPoint p);	// replaces the last point in the selection with p
    int Points() { return (int)list.size(); }
    bool Check(); // check if selection is ok (it is not ok if lines have intersections)
    ZFPoint* First() { if( Points()==0 ) return NULL; it = list.begin(); return &(*it); }
    ZFPoint* Last() { if(Points()==0) return NULL; it = list.end(); it--; return &list.back(); }
    ZFPoint* Next()  { it ++; if( it==list.end() ) return NULL; return &(*it); }
    ZFPoint* Previous() { if( it == list.begin() ) return NULL; it--; return &(*it); }
    ZFPoint& At(int index){ return list[index];}
    void Connect(); // close loop
    void Connect(int maxLength); // closes the loop and inserts additional points if the x-distance between the first and last point is larger than maxLength
    void updateBoundingBox(); // recalc the bounding box
    void BoundingBox( ZFRect& b) const override;
	bool isPointInPolygon( double x, double y ) override;
    ptlist& getPointList() { return list; }

	virtual void clear()
	{
		list.clear();
		it = list.begin();
		box.left=box.right=box.top=box.bottom=0;
	}
};

//================================================================================================================


//
// Free hand selection
//
class ZFS_API ZF2DSelectionFree : public ZF2DSelectionPoly
{
public:
    virtual ~ZF2DSelectionFree(){}

    using ZF2DSelectionPoly::Add;
    virtual void Add( ZF2DSelectionFree* freeSelection );
    using ZF2DSelectionPoly::AddFront;
    virtual void AddFront( ZF2DSelectionFree* freeSelection);

    int Typ() const override { return FREE; }
};

//================================================================================================================


//
// List for holding all selections
//
class ZFS_API ZF2DSelections
{
public:

public:
    typedef std::list <ZF2DSelection*> sellist_t;

    sellist_t    sellist;
    sellist_t::iterator sellist_it;

public:
    ZF2DSelections() {}
	ZF2DSelections( ZF2DSelections& selList );
    ~ZF2DSelections();

    int  N() const {if((int)sellist.size()>0) //AW
					return (int)sellist.size();
					else return 0;} // count
    void Add( ZF2DSelection* selection );       ///< Add selection to list. Selections takes ownership of the sel object!
    void Add( const sellist_t& selections );           ///< Add a list of selections to the list. Selections takes ownership of the selections.
    void ClearAll();                            // clears all selections
    ZF2DSelection* First();
    ZF2DSelection* Last();
    ZF2DSelection* Next();
    ZF2DSelection* Previous();
    ZF2DSelection* At( int index );     // returns the selection at index
    sellist_t takeAll();    ///< Ownership of all selections goes to the caller.
    void Delete( int index ); // deletes the element at index, reduces the number returned by N() by 1.
    void DeleteLast();
    void BoundingBox( ZFRect& ) const;
};

//================================================================================================================
