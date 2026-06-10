#pragma once

#include <exception>

#include "filterapi.h"

#include <zfs/Math/auxilliary/exception.h>

#include "multithreading_base.h"
#include "Matrix.h"

namespace zfs
{
    namespace math
    {
        template<> class Point<int>;
    }
}

namespace zfsfilter {
#pragma warning( push )
#pragma warning( disable : 4512 ) // Object is for processing of data not as a container, copying is not really useful -> disable warning
    // Warning appears due to constness of data members: m_Lines, m_Columns, m_TotalPointNo, m_Feature, m_Invalids

// ================================================================================================================================================
    template<class T> inline T abs(const T& value) { return (value > 0) ? value : -value; }
    // ================================================================================================================================================
    struct SegmentationException :public std::exception
    {
        std::string m_ExceptionString;
        SegmentationException(std::string a_ExceptionString) : m_ExceptionString(a_ExceptionString) {}
        ~SegmentationException() NOEXCEPT {}
        const char* what() const NOEXCEPT { return m_ExceptionString.c_str(); }
    };
    // ================================================================================================================================================
    /** @class Segmentation
     *  @brief  Class for segmenting data
     *
     *  @author Juergen Holzner
     *
     */
    template<class T>
    class FILTERDLL_API Segmentation : public MultithreadingBase
    {
    public:
        // -- enum -- //
        typedef enum GROUP_DATA_DET_OPT_SET
        {
            GDDO_UNDEF = 0,
            GDDO_MIN = 1,
            GDDO_MAX = 2,
            GDDO_MEAN = 4
        } GROUP_DATA_DET_OPT_TYPE;

    protected:
        /** useful typedefs **/
        typedef std::vector<std::list<zfs::math::TwoTypeTuple2<int, int*>>> VectorOfRefIntIntTuple2ListType; /// <Vector of Lists containing Tuples (for combination of segmentations)
        typedef std::vector<zfs::math::PointGroupDataListType::iterator> VectorOfPointGroupDataListIteratorsType;

        /** class constants **/
        static const double            m_c_UseProcessorPortion;             ///< preset processor portion to use

        /** private members **/
        const Matrix<T>*               m_Feature;                           ///< pointer to input matrix
        const Matrix<bool>*            m_Invalids;                          ///< indicates where the feature is valid

        bool                           m_bSegmented;                        ///< indicator that segmentation has been carried out
        bool                           m_bRemindPointGroupsInLists;         ///< indicates whether the found points and their feature should be stored in lists for further processing
        bool                           m_bMultithreaded;                    ///< conduct calculations multithreaded?
        bool                           m_bGroupSegmentedDataWithSameProps;  ///< (additionally) group together data with same characteristics, e.g. mean
        bool                           m_bOutputProgress;                   ///< output progress

        zfs::math::neighbourhood_t        m_SegmentationNeighbourhood;        ///< segmentation neighbourhood

        int                            m_Lines;                             ///< lines of input matrix
        int                            m_Columns;                           ///< columns of input matrix

        size_t                         m_TotalPointNo;                      ///< total number of pixels/points in matrix

        int                            m_pg_start_m;
        int                            m_pg_start_n;                        ///< search indices for pg start position

        T                              m_dFeatureMax;                       ///< criterion for finding point groups

        Matrix<bool>                   m_NeighborsAlreadyCalculated;        ///< for bookkeeping
        Matrix<int>                    m_Segmentation;                      ///< matrix representing point groups/segmentation

        zfs::math::PointGroupDataListType m_PointGroupDataList;                ///< segmentation (meta) information stored in a list

        /** private member functions **/

        void basic_intialization();

        zfs::math::PointGroupDataType     find_point_group(const int& a_point_group_idx, const int& a_group_data_det_selection);

        void                           do_segmentation();
        void                           GroupPointGroupsAccordingToSameCharacteristics();

        bool                           segmentation_predicate(const T& feature_value_to_check, const T& ref_feature_value, const T& threshold);

        /*- helping functions -*/
        int  MaximumSegmentIdx()                             const;
        int  SegmentIdxOfPoint(const zfs::math::Point<int>& aP) const;
        int* PtrToSegmentIdxOfPoint(const zfs::math::Point<int>& aP);

        void AddOffsetToSegmentIdices(const int& aOffset);
        void IteratorOfPointGroupWhichContainsPoint(const zfs::math::Point<int>& aPoint, zfs::math::PointGroupDataListType::iterator& aIterator);
        int  NoOfUniqueSegmentIdcs(std::list<std::list<zfs::math::PointGroupData*>>& aListOfListsOfPointGroupsWithSameIdx, std::list<int>& aUniqueIndices);

        void ApplyModifyingLUTToSegmentationMatrix(const std::vector<int>& lut_group_idx, const int& start_line, const int& end_line);

        /*- multithreading -*/
        /*- main for multithreading -*/
        void                   do_segmentationMT();

        static void            segmentationThreadWorker( Segmentation**                 ptpSegmentation,
                                                         Matrix<T>* const               pFeature,
                                                         Matrix<bool> * const           pInvalids,
                                                         const T &                      dFeatureMax,
                                                         const zfs::math::neighbourhood_t& SegmentationNeighbourhood,
                                                         const bool &                   bRemindPointGroupsInLists);

        /*- aux for multithreading -*/
        void combineSegmentationMTResults( std::vector<Segmentation*>& IndividualMTSegmentationList,
                                           const std::vector<zfs::math::Range<int>>& LineRangeList);

        bool combineTwoSections( Segmentation* const       pSection1,
                                 Segmentation* const       pSection2,
                                 const zfs::math::Range<int>& LineRangeSection1,
                                 const zfs::math::Range<int>& LineRangeSection2);

        bool combineTwoSections2( Segmentation* const       pSection1,
                                  Segmentation* const       pSection2,
                                  const zfs::math::Range<int>& LineRangeSection1,
                                  const zfs::math::Range<int>& LineRangeSection2);

        bool pointInPointGroupSection( zfs::math::PointGroupDataListType::iterator      & a_PointGroupListSection_it,
                                       int                                           & a_idx_pg_section,
                                       int                                           & a_idx_pt_in_pg_section,
                                       const zfs::math::Point<int>                      & a_point,
                                       const zfs::math::PointGroupDataListType::iterator& a_PointGroupListSection_end_it);

        void CollectMutualSegmenationInformationMatrix1Matrix2( const Matrix<T>* const                pFeatureMatrix1,
                                                                const Matrix<T>* const                pFeatureMatrix2,
                                                                const std::vector<int*>&               SegmentationIndicesOfFirstLinePointsMatrix2,
                                                                VectorOfRefIntIntTuple2ListType&      PositionsInMatrix1HavingMatchingSegmentationPredicateMatrix2);

        void ApplyModifyingLUTToSegmentationMatrixMT(const std::vector<int>& lut_group_idx);
        static void ApplyModifyingLUTToSegmentationMatrixThreadWorker(Segmentation* pSegmenation, const std::vector<int>& lut_group_idx, const int& start_line, const int& end_line);


        /** multithreading end **/


    public:

        /** public members **/

        // -- constructors -- //

        // -- empty segmentation object
        Segmentation(); 
        
        Segmentation( const Matrix<T>*        a_Feature,
                      const Matrix<bool>*     a_Invalids,
                      const T&                a_dFeatureMax,
                      zfs::math::neighbourhood_t a_NeighbourhoodType,
                      bool                    a_bRemindPointGroupsInLists,
                      bool                    a_bGroupSegmentedDataWithSameProps,
                      bool                    a_bOutputProgress,
                      bool                    a_bMultithreaded);
        
        void Init( const Matrix<T>*        a_Feature,
                   const Matrix<bool>*     a_Invalids,
                   const T&                a_dFeatureMax,
                   zfs::math::neighbourhood_t a_NeighbourhoodType,
                   bool                    a_bRemindPointGroupsInLists,
                   bool                    a_bGroupSegmentedDataWithSameProps,
                   bool                    a_bOutputProgress,
                   bool                    a_bMultithreaded);

        void segment();

        // -- accessors -- //

        // -- getter -- //
        const Matrix<int>&                     SegmentationMatrix()     const;
        const zfs::math::PointGroupDataListType&  PointGroupsDataList()    const;
        bool                                   segmented()              const;
        const Matrix<T>* const                 FeatureMatrix()          const;
        const Matrix<bool>*  const             InvalidsMatrix()         const;
        zfs::math::PointGroupDataListType* const  PointGroupsDataListPtr();
        Matrix<int>* const                     SegmentationMatrixPtr();
    };
    // ===================================================================================================================================================
    bool WritePointGroupDataListToFile(const std::wstring& FileName, const zfs::math::PointGroupDataListType& aPointGroupDataList);
    // ===================================================================================================================================================
    // -- helper function
    void test_MultithreadedSegmentation(const char* aFeatureMatrixAscFileName);
    // ===================================================================================================================================================

#pragma warning( pop )
} // namespace zfsfilter
