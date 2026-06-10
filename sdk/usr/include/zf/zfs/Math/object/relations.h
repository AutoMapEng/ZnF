#pragma once

#include <vector>
#include <list>
#include <set>

#include <zfs/zfsapi.h>

#include <zfs/Math/math/tuple.h>

namespace zfs { 
    namespace math {
        /** \class nmRelations
        *   \brief class for mapping/resolving a table of tuples/pair relationsships
        *   \author juergen holzner
        *   class for mapping/resolving a table of tuples/pair relationsships/links to two vectors of corresponding lists.
        *   E.g. a Table; [[1,2];[1,3];[2,4];[2,5];[2,3];[3,7]] is mapped to two tables of lists containing corresponding/linked elements
        *   from the input table, e.g.
        *   list 1: [ [ 1 , 2 ]      ,  [ 3 ] ]
        *                  /\            /\
        *                  ||            ||
        *                  \/            \/
        *   list 2: [ [ 2 , 3 , 4, 5],  [ 7 ] ].
        */
        class ZFS_API nmRelations
        {
        public:
            nmRelations() {};
            nmRelations(const std::vector<Tuple2<int>>& aRelTable);

            void getPartnerRelTables(std::vector<std::list<int>>& aPartnerTable1, std::vector<std::list<int>>& aPartnerTable2) const;
            void getPartnerRelTable1(std::vector<std::list<int>>& aPartnerTable1) const;
            void getPartnerRelTable2(std::vector<std::list<int>>& aPartnerTable2) const;

            // auxilliary
            std::set<int> UniqueElementsRelTable(const int aTupleIdx);

        private:
            const std::vector<Tuple2<int>> m_RelTable;

            std::vector<std::list<int>>    m_Partner1RelLists;
            std::vector<std::list<int>>    m_Partner2RelLists;

            void           resolveRelTable();
            void           findPartners(const int aElem, std::list<int>& PartnerTable1, std::list<int>& PartnerTable2);
            std::list<int> corrPartnersInRelList(const int aElemSearchVal, const int aTableSide);
            std::list<int> corrPartnersInRelList(std::list<int> aListOfElemSearchVals, const int aTableSide);
        };
    } // end namespace math
} // end namespace zfs