#pragma once
#include <string>

#include <zfcore/filename.h>
#include <zfcore/tinyxml2.h>
#include <stdio.h>

#include "zftools_api.h"

class ZFTRMatrix;

class ZFTOOLS_API QuickXML2 {
    zf::FileName text;

public:
    static tinyxml2::XMLError loadFile(tinyxml2::XMLDocument* document,
        FILE* file);
    static tinyxml2::XMLError loadFile(tinyxml2::XMLDocument* document,
        const std::string& filename, const char* mode = "rb");
    static tinyxml2::XMLError loadFile(tinyxml2::XMLDocument* document,
        const std::wstring& filename);

    static tinyxml2::XMLError saveFile(tinyxml2::XMLDocument* document,
        FILE* filename);
    static tinyxml2::XMLError saveFile(tinyxml2::XMLDocument* document,
        const std::string& filename);
    static tinyxml2::XMLError saveFile(tinyxml2::XMLDocument* document,
        const std::wstring& filename);

    static void setAttributeUTF8(tinyxml2::XMLElement *element, const char *key,
        const char *txt);
    static void setAttributeUTF8(tinyxml2::XMLElement *element,
        const wchar_t *key, const wchar_t *txt);
    static void setAttributeUTF8(tinyxml2::XMLElement *element, const char *key,
        const wchar_t *txt);
    static void setAttributeUTF8(tinyxml2::XMLElement *element,
        const wchar_t *key, const char *txt);

    /// creates a new text element via new
    static tinyxml2::XMLText *setXMLTextUTF8(tinyxml2::XMLDocument* doc, std::string txt, bool cpToUtf8=false); // linux need cpToUtf8=false
    static tinyxml2::XMLText *setXMLTextUTF8(tinyxml2::XMLDocument* doc, std::wstring txt);
    static tinyxml2::XMLText *setXMLTextUTF8(tinyxml2::XMLDocument* doc, const char *txt, bool cpToUtf8=false); // linux need cpToUtf8=false
    static tinyxml2::XMLText *setXMLTextUTF8(tinyxml2::XMLDocument* doc, const wchar_t *txt);

    const char *getAttribut(const tinyxml2::XMLElement *element, const char *key);
    const std::string getAttributString(const tinyxml2::XMLElement *element,
        const char *key);
    const double getAttributDouble(const tinyxml2::XMLElement *element,
        const char *key);
    const int getAttributInt(const tinyxml2::XMLElement *element,
        const char *key);
    const wchar_t *getAttributW(const tinyxml2::XMLElement *element,
        const char *key);
   // const std::wstring getAttributWString(const tinyxml2::XMLElement *element,
   //     const char *key);
    const char *getText(const tinyxml2::XMLElement *element);
    const wchar_t *getTextW(const tinyxml2::XMLElement *element);

    const char *getValue(const tinyxml2::XMLNode *element);
    const char *getValue(const tinyxml2::XMLElement *element);
    const wchar_t *getValueW(const tinyxml2::XMLNode *element);
    const wchar_t *getValueW(const tinyxml2::XMLElement *element);


};
