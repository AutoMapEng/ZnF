#pragma once

#include <string>
#include <list>

#include "ZfsPrjBasic.h"

namespace zfprj
{

    // ======================================================
    /**
    \class StatisticsParameterNameMapping
    \brief for mapping of long , short names and units
    \author    Holzner
    \date  02.05.2016
    */
    class ZFSPRJ_API StatisticsParameterNameMapping
    {
    public:
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::string  ShortName;
        std::wstring LongName;
        std::string  Unit;
#pragma warning(pop)
        StatisticsParameterNameMapping():ShortName( "" ),
            LongName( L"" ),
            Unit( "" )
        {}

        StatisticsParameterNameMapping( const std::string& aParamShortName, const std::wstring& aParamLongName, const std::string& aUnit )
            : ShortName( aParamShortName ),
            LongName( aParamLongName ),
            Unit( aUnit )
        {}

    };
    // =======================================================================================
    /**
    \class StatisticsParameterNameList
    \brief keeps the list of parameter name mappings used in laser control
    \author  Juergen  Holzner
    \date  23.05.2016
    */
    class ZFSPRJ_API StatisticsParameterNameMappingList
    {
    public:
        static const StatisticsParameterNameMapping TFSPN_TargetType       ;
        static const StatisticsParameterNameMapping TFSPN_NPtsUsedFit      ;
        static const StatisticsParameterNameMapping TFSPN_TargetCntrStdDevX;
        static const StatisticsParameterNameMapping TFSPN_TargetCntrStdDevY;
        static const StatisticsParameterNameMapping TFSPN_TargetCntrStdDevZ;
        static const StatisticsParameterNameMapping TFSPN_FracUsedTargArea ;
        static const StatisticsParameterNameMapping TFSPN_RadPMinDev       ;
        static const StatisticsParameterNameMapping TFSPN_RadPMaxDev       ;
        static const StatisticsParameterNameMapping TFSPN_RadPMeanDev      ;
        static const StatisticsParameterNameMapping TFSPN_RadPStdDev       ;
        static const StatisticsParameterNameMapping TFSPN_Comment          ;

        StatisticsParameterNameMappingList();

        // -- list operations required
        const std::list<StatisticsParameterNameMapping>::const_iterator begin() const
        {
            return m_StatisticsParameterNameMappingList.begin();
        }

        const std::list<StatisticsParameterNameMapping>::const_iterator end() const
        {
            return m_StatisticsParameterNameMappingList.end();
        }

        void push_back(const StatisticsParameterNameMapping & aElement)
        {
            return m_StatisticsParameterNameMappingList.push_back( aElement );
        }

    protected:
        // -- 
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::list<StatisticsParameterNameMapping> m_StatisticsParameterNameMappingList;
#pragma warning(pop)
    };

    extern ZFSPRJ_API StatisticsParameterNameMappingList STATISTICS_PARAMETER_NAME_MAPPING_LIST;

    // =======================================================================================
    /**
    \class StatisticsParameterNameList
    \brief keeps the list of parameter name mappings used in laser control
    \author  Juergen  Holzner
    \date  23.05.2016
    */
    class ZFSPRJ_API TargetTypeNameMappingList
    {
    public:
        static const StatisticsParameterNameMapping TFSTN_StdTargetName;
        static const StatisticsParameterNameMapping TFSTN_SphTargetName;

        TargetTypeNameMappingList();

        // -- list operations required
        const std::list<StatisticsParameterNameMapping>::const_iterator begin() const
        {
            return m_TargetTypeNameMappingList.begin();
        }

        const std::list<StatisticsParameterNameMapping>::const_iterator end() const
        {
            return m_TargetTypeNameMappingList.end();
        }

        void push_back( const StatisticsParameterNameMapping & aElement )
        {
            return m_TargetTypeNameMappingList.push_back( aElement );
        }

    protected:
        // -- 
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::list<StatisticsParameterNameMapping> m_TargetTypeNameMappingList;
#pragma warning(pop)
    };

    extern ZFSPRJ_API TargetTypeNameMappingList TARGET_TYPE_NAME_MAPPING_LIST;
} //namespace zfprj
