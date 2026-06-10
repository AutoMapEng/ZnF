#ifndef ZFTOOLS_PLOT_H
#define ZFTOOLS_PLOT_H

#ifdef ZFTOOLS_PLOT
#include "zftools_api.h"


#include <zfcore/zffile.h>
#include <zfcore/filename.h>

#include <string>


struct gdImageStruct;


ZFTOOLS_API std::string getStartPath();

//=====================================================================================================

// kapselt basic plot

class ZFTOOLS_API Plot
{
protected:
    int wd, hg; // gesamt
    int xx, yy; // einzug
    int cols;
    gdImageStruct* im;
    std::string times;
    int Col(unsigned int);
    int Idx2Col(int idx);

public:
    Plot();
    virtual ~Plot();

    int Hg() { return hg; }
    int Wd() { return wd; }
    int W() { return wd - xx; }
    int H() { return hg - yy; }
    bool Create(int w, int h, int c = 0xffffff, bool transparent = true, unsigned int cols = 16);
    bool CreateGrayImage(int w, int h);
    int Colors();
    int Color(int idx);
    void Clear(unsigned int col);
    void Rec(int x1, int y1, int x2, int y2, unsigned int col);
    void Frame(unsigned int col); // frame around image
    void Grid(int w, int h);
    void Set(int x, int y, unsigned int col); // SetPixel
    int Get(int x, int y); // GetPixel
    void SetGray(int x, int y, unsigned int col);
    void Line(int x1, int y1, int x2, int y2, unsigned int col = 0, bool aa = false);
    void DLine(int x1, int y1, int x2, int y2, unsigned int col = 0, bool aa = false);
    void Pt(int x, int y, unsigned int col = 0);
    bool Save(const std::string& fn, int jpg = 0);
    bool Save(const std::wstring& fn, int jpg = 0);
    void Destroy();
    enum { STR_LEFT, STR_CENTER, STR_RIGHT, STR_CENTER_UP};
    enum { ROT_0, ROT_90 };
    void String(int x, int y, const char*s, unsigned int col = 0, int just = STR_CENTER, bool rot = false, int large = 3);//size 3, 2, 1
    int gdImageStringTTFGiant(int x, int y, unsigned char* s, int size = 18);
    void Graph(int ex, int ey);
    void GGrid(int x, int y, const char* sx, const char* sy, double maxx, double maxy, double gridx, double gridy);
    void GGrid(int nx, int ny);
    void GSet(int x, int y, unsigned int col = 0);
    void GLine(int x1, int y1, int x2, int y2, unsigned int col = 0);

    enum
    {
        WHITE = 0xffffff,
        BLACK = 0,
        RED = 0xff0000,
        DRED = 0x800000,
        GREEN = 0x00ff00,
        DGREEN = 0x008000,
        BLUE = 0x0000ff,
        DBLUE = 0x000080,
        GRAY = 0xc0c0c0,
        DGRAY = 0x808080,
        CYAN = 0x00ffff,
        DCYAN = 0x008080,
        MAGENTA = 0xff00ff,
        DMAGENTA = 0x800080,
        YELLOW = 0xFFC023,//ffff00
        BROWN = 0x808000,
        LRED = 0xff8080,
        LGREEN = 0x80ff80,
        LBLUE = 0x8080ff,
    };
};

//=====================================================================================================

// Advanced Plot

class ZFTOOLS_API APlot
{
protected:
    bool ok{};
    std::string times;
public:
    Plot plot;

    int dimx{};
    int dimy{};
    int ox{};
    int oy{};
    int oy2{};

    double xfrom{};
    double xto{};
    double yfrom{};
    double yto{};

#define maxId  20

    unsigned int col[maxId]{};

protected:
    int y2{};
    double scalex{};
    double scaley{};
    int nachkommaX{};
    int nachkommaY{};

    int n[maxId]{};
    int lastx[maxId]{};
    int lasty[maxId]{};

public:
    APlot();
    virtual ~APlot();

    // first step
    void Def(double fx, double tx, double fy, double ty);
    // second step
    bool Create2(int dx, int dy, int ofx = -1, int ofy = -1, int ofxRight = -1, int ofyTop = -1, bool transparent = false, unsigned int col = 16);
    bool Create(int dx, int dy, int ofx = 100, int ofy = 100, bool transparent = false, unsigned int col = 16);
    void Grid(double sx, double sy, bool txt = true, unsigned int col = Plot::GRAY, int sz = 3);
    void GridDef(int nachkommaX, int nachkommaY);

    bool Ok() { return ok; }

    void LineDef(int id, unsigned int col);
    void Line(int id, double x, double y); // id 0..
    void Line(int id, double x1, double y1, double x2, double y2); // id 0..
    bool Save(const std::string& fn, int f = 0);
    bool Save(const std::wstring& fn, int f = 0);
    void Cross(double x, double y, unsigned int col = Plot::BLACK, int sz = 3);
    void Pt(double x, double y, unsigned int col = Plot::BLACK, int sz = 1); // size 1 or 2(2x2)
    void Dot(double x, double y, unsigned int col = Plot::BLACK, int sz = 1); 
    void String(double x, double y, const char*s, unsigned int col = 0, int just = Plot::STR_LEFT, int size = 3);
    void Rec(double x1, double y1, double x2, double y2, unsigned int col = 0);
};

class ZFTOOLS_API PlotTrueColor
{
protected:
    bool ok{};
    int currentRgb{};
    std::string times;
    int wd{}, hg{}; // gesamt
    int xx{}, yy{}; // einzug
    unsigned int backCol{};

    int Col(int);
    int Idx2Col(int idx);

public:
    gdImageStruct* im{};

    int dimx{};
    int dimy{};
    int ox{100};
    int oy{100};

    int xfrom{0};
    int xto{360};
    int yfrom{5};
    int yto{-5};

    enum { STR_LEFT, STR_CENTER, STR_RIGHT };
    enum
    {
        WHITE = 0xffffff,
        BLACK = 0,
        RED = 0xff0000,
        DRED = 0x800000,
        GREEN = 0x00ff00,
        DGREEN = 0x008000,
        BLUE = 0x0000ff,
        DBLUE = 0x000080,
        GRAY = 0xc0c0c0,
        DGRAY = 0x808080,
        CYAN = 0x00ffff,
        DCYAN = 0x008080,
        MAGENTA = 0xff00ff,
        DMAGENTA = 0x800080,
        YELLOW = 0xffff00,
        BROWN = 0x808000,
    };

protected:
    int y2{};
    double scalex{};
    double scaley{};

protected:
    bool Create_(int w, int h, unsigned int c = 0xffffff, bool transparent = true);
    void Line_(int x1, int y1, int x2, int y2, unsigned int col = 0, bool aa = false);
    void checkPoint(int& x, int& y);

public:
    PlotTrueColor();
    virtual ~PlotTrueColor();

    // first step
    void Def(int fx, int tx, int fy, int ty);
    // second step
    bool Create(int dx, int dy, int ofx = 100, int ofy = 100, int backCol = 0xffffff);

    bool Ok() const { return ok; }
    int Hg() const { return hg; }
    int Wd() const { return wd; }
    int W() const { return wd - xx; }
    int H() const { return hg - yy; }
    int Color(int idx);
    void Clear(unsigned int col);
    void Line(int x1, int y1, int x2, int y2, unsigned int col = 0, bool aa = false); // id 0..
    std::string Save(std::string& fn, int jpg = 0);
    std::string Save(std::wstring& fn, int jpg = 0);
    std::string Save(zf::FileName& fn, int jpg = 0);
    void Pt(int x, int y, unsigned int rgb);
    void Set(int x, int y, unsigned int rgb) { Pt(x, y, rgb); }
    void String(int x, int y, const char*s, unsigned int rgb = 0, int just = Plot::STR_LEFT, int size = 0);
    void SetColor(unsigned int rgb);
    void Frame(unsigned int col); // frame around image

    int GetPixel(int x, int y);
    int Get(int x, int y) { return GetPixel(x, y); }
    void Clear();
};

//=====================================================================================================
#endif //USE_ZFTOOLS_PLOT
#endif // ZFTOOLS_PLOT_H
