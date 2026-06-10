#pragma once

#include "ZfsPrjBasic.h"
#include "vecname.h"
#include "targets.h"


class ZFTRMatrix;

#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
class ZFSPRJ_API VecNameList : public vecnamelst_t
{
public:
    double standardDeviation;
    double average;
    double max;
    double min;
    int nr;					///< Anzahl der Targets gesammt
    int nrDisabled;			///< Anzahl der nicht genuetzten Targets

public:
    VecNameList() : vecnamelst_t() { clearValues(); }
    VecNameList(const vecnamelst_t& v);
    VecNameList(const targetlist_t& v);
    VecNameList(const VecNameList& v);

    void push_back( const VecName& v );
    void push_backOrg( const VecName& v );
    void clearValues()
    {
        standardDeviation = 0;
        average = 0;
        max = 0.;
        min = 10000000.;
        nr = 0;
        nrDisabled = -1;
        clear();
    }

    /**
    *   Transformiert alle beinhaltenden Targets in der Liste nach m.
    */
    void world(const ZFTRMatrix& m);

    /**
    *   Addiert alle glichen Targets in den Listen
    *   \param v => hinzuzufuegende Liste
    *   \param used => True = Es sollen nur aktivierte Targets hinzugefuegt werden.
    */
    void add(const vecnamelst_t& v, bool used );
    void add(const targetlist_t& v, bool used );

    /**
    *   Subtrahiert die Addierten Targets
    *   \param removeSingel => Alle Targets die nrFound 1 oder 0 haben werden aus der Liste entfernt.
    */
    void subb( bool removeSingel = false );

    /**
    *   Berechnet aus den zwei Target listen die satistischen kenngroessen
    *   In der Rueckgabeliste stehen die Berechneten distanzen und die statistischen Werte
    *   \param v = Liste mit der Verglichen werden soll
    *   \param addNotUsedAndDisabled = Nicht benutzte und ausgeschaltete Targets werden am Ende der liste hinzugefuegt.
    */
    VecNameList statistic(bool useEnabled, const VecNameList& v, bool addNotUsedAndDisabled = true);
    /**
    *   Berechnet aus den zwei Target listen die satistischen kenngroessen
    *   In der Rueckgabeliste stehen die Berechneten distanzen und die statistischen Werte
    *   \param v = Liste mit der Verglichen werden soll
    *   \param addNotUsedAndDisabled = Nicht benutzte und ausgeschaltete Targets werden am Ende der liste hinzugefuegt.
    */
    VecNameList statistic(bool useEnabled, const vecnamelst_t& v, bool addNotUsedAndDisabled = true);

    static void statistic(const std::list<VecNameList>& list, double& standardDeviation, double& average, double& max, double& min, int& nr, int& notUsed);

protected:
    static void addValues(double val, double& Summe1, double& Summe2, double& average, double& max, double& min);
};
#pragma warning(pop)