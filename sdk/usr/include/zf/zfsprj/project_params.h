#pragma once

#include "ZfsPrjBasic.h"

#include <map>

//===============================================================================

namespace tinyxml2 {
    class XMLNode;
}

//===============================================================================

typedef std::map<std::string, std::wstring> prjparam_t;
typedef prjparam_t::iterator prjparam_it;
typedef prjparam_t::const_iterator prjparam_it_const;

// user defined params
class ZFSPRJ_API ProjectParams {
public:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
    prjparam_t param;
#pragma warning(pop)
    tinyxml2::XMLNode* mNode{};

    bool writeLock{};
    bool dirty{};

public:
    ProjectParams() : mNode(NULL), writeLock(false), dirty(false) {};

    /// if params node exist, it will read existing params otherwise the node will be created
    void setNode(tinyxml2::XMLNode *n);

    bool hasParam(const char *) const;

    const wchar_t *Param(const char *, const wchar_t *def) const;
    int Param(const char *, int def) const;
    double Param(const char *, double def) const;
    bool Param(const char *, bool def) const;

    void SetParam(const char *, const wchar_t *val, bool save = true);
    void SetParam(const char *, int val, bool save = true);
    void SetParam(const char *, double val, bool save = true);

    void DeleteParam(const char *key);

    /// save params. Node has to be created before
    void saveParams( bool force=false );
    /// load params from the params node. Typically use setNode instead
    bool readParams(tinyxml2::XMLNode *node);

    size_t getParameterCount() const;

    bool isEmpty() const;

    /**
     * @brief (Re)Set a write lock for the project params. This can be used to
     *        store the parameters only once while changing a bunch of params.
     *
     * @param isLocked the flag if the params are locked or not.
     */
    void setParamWriteLock(bool isLocked);

    /**
    * @brief Flag if the parameters have been changed
    */
    bool isDirty() const;

protected:
    void clear();
};
