#pragma once
#include "../zfsapi.h"

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <zfs/Math/auxilliary/types.h>

namespace tinyxml2
{
    class XMLNode;
    class XMLElement;
    class XMLDocument;
}

namespace zfsfilter
{
    // =========================================================================================================================================================
    // =========================================================================================================================================================
    // Disable warning messages 4251: // compiler warning for members which are
    // private -> switch off warnings (see also:
    // http://stackoverflow.com/questions/767579/exporting-classes-containing-std-objects-vector-map-etc-from-a-dll)
#pragma warning(push)
#pragma warning(disable : 4251)

    typedef std::vector<std::string> ParamElement_t;
    typedef std::pair<std::wstring, int> ProcessItemKey;

    class ZFS_API ParamElement {
    private:
        ParamElement_t m_ParamElementData;

    public:
        enum class ParamType: uint8_t {
            PARAM_NAME = 0,
            PARAM_VALUE = 1,
            PARAM_UNIT = 2,
            PARAM_MIN = 3,
            PARAM_MAX = 4,
            PARAM_DEFAULT = 5,
            PARAM_DESCRIPTION = 6,
            NPARAM_ENUM_ELEMENTS = 7 // last element gives the number of elements in the
                                     // enum (for vector alloc)
        };

        ParamElement()
            : m_ParamElementData(std::vector<std::string>(toUType(ParamType::NPARAM_ENUM_ELEMENTS), "")) {
        }

        ParamElement(const std::string &a_ParamName, const std::string &a_ParamValue,
            const std::string &a_ParamUnit)
            : m_ParamElementData(std::vector<std::string>(toUType(ParamType::NPARAM_ENUM_ELEMENTS), "")) {
            setParameterValue(a_ParamValue);
            setParameterName(a_ParamName);
            setParameterUnit(a_ParamUnit);
        }

        void setParameterName(std::string a_ParamName) {
            if (!m_ParamElementData.empty())
                m_ParamElementData[toUType(ParamType::PARAM_NAME)] = a_ParamName;
        }
        void setParameterUnit(std::string a_ParamUnit) {
            if (!m_ParamElementData.empty())
                m_ParamElementData[toUType(ParamType::PARAM_UNIT)] = a_ParamUnit;
        }

        void setParameterValue(std::string a_ParamValue,
            ParamType param = ParamType::PARAM_VALUE) {
            if (!m_ParamElementData.empty())
                m_ParamElementData[toUType(param)] = a_ParamValue;
        }

        void setParameterValue(const int &a_ParamValue) {
            char buffer[256];
            sprintf(buffer, "%i", a_ParamValue);
            m_ParamElementData[toUType(ParamType::PARAM_VALUE)] = buffer;
        }

        void setParameterValue(const float &a_ParamValue) {
            char buffer[256];
            sprintf(buffer, "%0.7f", a_ParamValue);
            m_ParamElementData[toUType(ParamType::PARAM_VALUE)] = buffer;
        }

        void setParameterValue(const double &a_ParamValue) {
            char buffer[256];
            sprintf(buffer, "%0.15f", a_ParamValue);
            m_ParamElementData[toUType(ParamType::PARAM_VALUE)] = buffer;
        }

        template <class T> T getParameterValue() const {
            return (T)atof(m_ParamElementData[toUType(ParamType::PARAM_VALUE)].c_str());
        }
        std::string getParameterValueStr(
            ParamElement::ParamType param = ParamElement::ParamType::PARAM_VALUE) const {
            if ((size_t)param < m_ParamElementData.size()) {
                return m_ParamElementData[toUType(param)];
            }
            else {
                return "";
            }
        }
        std::string getParameterName() const {
            return m_ParamElementData[toUType(ParamType::PARAM_NAME)];
        }
        std::string getParameterUnit() const {
            return m_ParamElementData[toUType(ParamType::PARAM_UNIT)];
        }

        std::string getParameterMinimum() const {
            return m_ParamElementData[toUType(ParamType::PARAM_MIN)];
        }

        std::string getParameterMaximum() const {
            return m_ParamElementData[toUType(ParamType::PARAM_MAX)];
        }

        std::string getParameterDefault() const {
            return m_ParamElementData[toUType(ParamType::PARAM_DEFAULT)];
        }
    };

    // =========================================================================================================================================================
    typedef std::list<ParamElement> ParamListData_t;

    class ZFS_API ParamList {
    private:
        ParamListData_t m_ParamListData;

    public:
        ParamList() {};
        ParamList(const ParamList &a_ParamList);

        // -- modifiy list
        void addParameter(const std::string &a_ParameterName,
            const std::string &a_ParameterValue,
            const std::string &a_ParameterUnit);
        bool deleteParameter(const std::string &a_ParameterName);
        bool changeParameter(const std::string &a_ParameterName,
            const std::string &a_ParameterValue,
            const std::string &a_ParameterUnit);

        // -- access to parameter
        ParamListData_t getParameterList() const;
        ParamListData_t &getAccessParameterList();

        template <class T>
        T getParameterValue(const std::string &a_ParameterName) const;
        std::string getParameterValueStr(
            const std::string &a_ParameterName,
            ParamElement::ParamType param = ParamElement::ParamType::PARAM_VALUE) const;
        std::string getParameterUnit(const std::string &a_ParameterName) const;

        template <class T>
        bool setParameterValue(const std::string &a_ParameterName,
            const T &a_ParameterValue);
        template <class T>
        bool setParameterValue(const std::string &a_ParameterName,
            ParamElement::ParamType param,
            const T &a_ParameterValue);
        bool setParameterUnit(const std::string &a_ParameterName,
            const std::string &a_ParameterUnit);
    };
    // =========================================================================================================================================================

    // -- access to parameter
    template <class T>
    T ParamList::getParameterValue(const std::string &a_ParameterName) const {
        bool bFoundParameter = false;
        T ParameterValue = T(0);

        ParamListData_t::const_iterator ParamList_it = m_ParamListData.begin();
        for (; ParamList_it != m_ParamListData.end() && !bFoundParameter;
            ++ParamList_it) {
            if ((*ParamList_it).getParameterName() == a_ParameterName) {
                ParameterValue = (*ParamList_it).getParameterValue<T>();
                bFoundParameter = true;
            }
        }
        return ParameterValue;
    }

    template <class T>
    bool ParamList::setParameterValue(const std::string &a_ParameterName,
        const T &a_ParameterValue) {
        bool bSuccess = false;

        ParamListData_t::iterator ParamList_it = m_ParamListData.begin();
        for (; ParamList_it != m_ParamListData.end(); ++ParamList_it) {
            if ((*ParamList_it).getParameterName() == a_ParameterName) {
                (*ParamList_it).setParameterValue(a_ParameterValue);
                bSuccess = true;
                break;
            }
        }
        return bSuccess;
    }

    template <class T>
    bool ParamList::setParameterValue(const std::string &a_ParameterName,
        ParamElement::ParamType param,
        const T &a_ParameterValue) {
        bool bSuccess = false;

        ParamListData_t::iterator ParamList_it = m_ParamListData.begin();
        for (; ParamList_it != m_ParamListData.end(); ++ParamList_it) {
            if ((*ParamList_it).getParameterName() == a_ParameterName) {
                (*ParamList_it).setParameterValue(a_ParameterValue, param);
                bSuccess = true;
                break;
            }
        }
        return bSuccess;
    }

    // =========================================================================================================================================================
    /** \class ProcessItemParameterConfiguration
    * \brief  Class for representing a processing item (e.g. a filter, a scan data layer) configuration (processing item (e.g. a filter, a scan data layer) parameter).
    *
    * \author Juergen Holzner
    *
    * Class for representing a processing item (e.g. a filter, a scan data layer) configuration (processing item (e.g. a filter, a scan data layer) parameter).
    *
    */
    class ZFS_API ProcessItemParameterConfiguration {
    private:
        std::string m_ProcessItemName;
        std::string m_ProcessItemDescription;
        std::string m_ProcessItemActive;
        ParamList m_ProcessItemParameterList;

    public:
        ProcessItemParameterConfiguration() {};

        bool is_active() const;
        void set_active();
        void set_inactive();
        void set_status(const std::string &a_status);

        std::string get_status() const;

        template <class T>
        T getParameterValue(const std::string &a_ParameterName) const;
        std::string getParameterValueStr(
            const std::string &a_ParameterName,
            ParamElement::ParamType param = ParamElement::ParamType::PARAM_VALUE) const;
        std::string getParameterUnit(const std::string &a_ParameterName) const;
        std::string getProcessItemName() const;
        std::string getProcessItemDescription() const;
        ParamList getParameterList() const;

        void addParameter(const std::string &a_ParameterName,
            const std::string &a_ParameterValue,
            const std::string &a_ParameterUnit);
        void deleteParameter(const std::string &a_ParameterName);

        void setProcessItemParameterList(const ParamList &a_Parameter_List);
        void setProcessItemName(const std::string &a_ProcessItemName);
        void setProcessItemDescription(const std::string &a_ProcessItemDescription);

        template <class T>
        bool setParameterValue(const std::string &a_ParameterName,
            const T &a_ParameterValue);
        template <class T>
        bool setParameterValue(const std::string &a_ParameterName,
            ParamElement::ParamType param,
            const T &a_ParameterValue);

        bool setParameterUnit(const std::string &a_ParameterName,
            const std::string &a_ParameterUnit);
    };
    // =========================================================================================================================================================
    template <class T>
    T ProcessItemParameterConfiguration::getParameterValue(
        const std::string &a_ParameterName) const {
        return m_ProcessItemParameterList.getParameterValue<T>(a_ParameterName);
    }

    template <class T>
    bool ProcessItemParameterConfiguration::setParameterValue(
        const std::string &a_ParameterName, const T &a_ParameterValue) {
        return m_ProcessItemParameterList.setParameterValue(a_ParameterName,
            a_ParameterValue);
    }

    template <class T>
    bool ProcessItemParameterConfiguration::setParameterValue(
        const std::string &a_ParameterName, ParamElement::ParamType param,
        const T &a_ParameterValue) {
        return m_ProcessItemParameterList.setParameterValue(a_ParameterName, param,
            a_ParameterValue);
    }

    // =========================================================================================================================================================
    typedef std::list<ProcessItemParameterConfiguration>
        ProcessItemParameterConfigurationData_t;

    /** \class ProcessItemParameterConfigurationList
    * \brief  Class for handling processing item (e.g. a filter, a scan data layer) configurations (file,parameter) of filters
    * for filtering of data based on a chain of filters.
    *
    * \author Jürgen Holzner
    *
    * Class for handling processing item (e.g. a filter, a scan data layer) configurations (file,parameter) of filters for
    * filtering of data based on a chain of filters.
    *
    */
    class ZFS_API ProcessItemParameterConfigurationList {
    private:
        // -- member
        ProcessItemParameterConfigurationData_t
            m_ProcessItemParameterConfigurationData; //< configuration data: active:
                                                //yes/no, parameter settings, processing item (e.g. a filter, a scan data layer)
                                                //names and sequence
        std::string m_ConfigType;               //< "user" or "default"
        std::string m_ScannerType;              //< "9012", ...
        std::wstring m_Group;                   //< Scan group
        bool m_bFileError; //< occurred during reading or writing (true/false)

                           // -- helper
        void addProcessItemParameterConfiguration(
            const std::string &a_ProcessItemName,
            ProcessItemParameterConfiguration &a_ProcessItemParamList);
        void changeProcessItemParameterConfiguration(
            const std::string &a_ProcessItemName,
            ProcessItemParameterConfiguration &a_ProcessItemParamList);

    public:
        // -- constructors
        ProcessItemParameterConfigurationList();
        ProcessItemParameterConfigurationList(const std::wstring &a_FileName);

        // -- file access
        bool readProcessItemParameterConfigListFromFile(const std::wstring &a_FileName);
        bool readProcessItemDescribtionFromFile(const std::wstring &a_FileName);
        bool
            writeProcessItemParameterConfigListToFile(const std::wstring &a_FileName) const;

        void initProcessItemConfigFromXML(tinyxml2::XMLNode *ParamsNode);
        tinyxml2::XMLElement *getProcessItemConfigAsXML(tinyxml2::XMLDocument* doc) const;

        // -- getters
        ProcessItemParameterConfiguration
            getProcessItemParameterConfiguration(const std::string &a_ProcessItemName) const;
        ProcessItemParameterConfigurationData_t
            getProcessItemParameterConfigurationList() const;
        std::string getScannerType() const;
        std::string getConfigType() const;
        std::wstring getGroup() const;
        std::list<std::string> getListOfProcessItems() const;
        ProcessItemParameterConfigurationList getConfigurationOfActiveProcessItems() const;
        bool FileReadStatusOK() const;
        bool Empty() const;

        // -- setters
        void setProcessItemParameterConfiguration(
            const ProcessItemParameterConfiguration &a_ProcessItemParameterConfiguration);
        void setScannerType(const std::string &a_ScannerType);
        void setConfigType(const std::string &a_ConfigType);
        void setGroup(const std::wstring &a_Group);

        // -- modifiers
        void addProcessItemParameterConfiguration(
            const ProcessItemParameterConfiguration &a_ProcessItemParameterConfiguration);
        void insertProcessItemParameterConfiguration(
            const ProcessItemParameterConfiguration &a_ProcessItemParameterConfiguration,
            const int index);
        bool deleteProcessItemParameterConfiguration(
            const ProcessItemParameterConfiguration &a_ProcessItemParameterConfiguration);
        ProcessItemParameterConfigurationData_t &
            getAccessProcessItemParameterConfigurationList();

        void swapProcessItemParameterConfigurations(const std::string &filterName1,
            const std::string &filterName2);
    };
    // =========================================================================================================================================================

    // -- switch on the warnings as before again
#pragma warning(pop)
} // end namespace
