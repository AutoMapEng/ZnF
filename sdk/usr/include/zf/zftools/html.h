#pragma once
#include "zftools_api.h"
#include <zfcore/compiler.h>

#include <string>

//=================================================================================================================

namespace zftools
{

class ZFTOOLS_API Html
{
    public:
        std::string s;
        std::string t;

    public:
        Html( const char* );
        Html() {}

        void Start( const char* script=NULL, const char* body=NULL, bool css=true );
        void End( bool div = true );

        void Title( const char* t );
        void H1( const char* t_ ) { Title(t_); }
        void H2( const char* t );
        void H3( const char* t );
        void HR();
        void Image( const char* file, int w=0, int h=0 );
        void Str( const char* str, int col=0 );
        void Str( const std::string&, int col=0 );

        const char* operator()() { return s.c_str(); }
        const Html& operator += (const char* ss ) { s += ss; return *this; }
        const Html& operator += (const char ch ) { s += ch; return *this; }
        const Html& operator += (const std::string& ss ) { s += ss; return *this; }
};

//=================================================================================================================

class ZFTOOLS_API HtmlTab
{
    public:
        std::string s;
        bool end;

    public:
        HtmlTab( const char* param=NULL );

        void Add( const char* param, const char* a, const char* b, const char* c=NULL, const char* d=NULL, const char* e=NULL, const char* f=NULL, const char* g=NULL, const char* h=NULL );
        void Add( const char* str )  { s+=str; }

        HtmlTab& operator += ( const char* str )  { s+=str; return *this; }

        const char* Str();
};

//=================================================================================================================

} // end namespace zftools
