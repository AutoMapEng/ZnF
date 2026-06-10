#pragma once

#include "ZfsPrjBasic.h"

#include <string>
#include <map>
#include <cstdint>

#include <zfs/Math/zfvec.h>


namespace zfprj
{

    // ======================================================

#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle

    /**
    * \brief    Target statistic
    *           Holds all values a targetfitting will return.
    *           The values are stored as well.
    */
    class ZFSPRJ_API Target_Statistic
    {

    public:
        Target_Statistic() = default;


        void set(const std::string& key, double value, const std::wstring& unit=L"");
        void set(const std::string& key, int value, const std::wstring& unit = L"");
        void set(const std::string& key, const zfs::math::Vec3d& value, const std::wstring& unit = L"");


        double getDouble(const std::string& key, double def = {}) const;
        int getInt(const std::string& key, int def = {}) const;
        zfs::math::Vec3d getVec(const std::string& key, const zfs::math::Vec3d& def = {}) const;
        std::wstring getUnit(const std::string& key) const;


        /// parse all settings from a string
        void parse(const std::string&);

        /// parse from a xml string
        void parseXML(const std::string&);



        /// return a one line string
        std::string toString() const;

        /// return a xml string representation
        std::string toXML() const;

        /// return a html based string to display it in a dialog
        std::string toHTML() const;



        // the value stored for a key
        struct target_statistic_value
        {
            enum class type_t : uint32_t
            {
                UNDEFINED, DOUBLE, INT, VECTOR
            };
            type_t t{ type_t::UNDEFINED };

            //value
            double d{};
            int i{};
            zfs::math::Vec3d v{};
            std::wstring unit; ///< optional unit

            target_statistic_value() = default;
            target_statistic_value(double v, const std::wstring& u) : t(type_t::DOUBLE), d(v), unit(u) {}
            target_statistic_value(int v, const std::wstring& u) : t(type_t::INT), i(v), unit(u) {}
            target_statistic_value(const zfs::math::Vec3d& vec, const std::wstring& u) : t(type_t::VECTOR), v(vec), unit(u) { }
            target_statistic_value(const target_statistic_value&) = default;
        };

        /// get the value type of a entry
        target_statistic_value::type_t getType(const std::string& key) const;


        /// the most importand keys
        static const char* key_type;  // target type [int]
        static const char* key_quality; // percent [double]
        // 3D target
        static const char* key_correlation; // [double]
        static const char* key_coverage; // [double]
        static const char* key_precision; // [double]

    protected:
        std::map< std::string, target_statistic_value > m_list; ///< statistics values as a key list (map)
    };

    //########################################################################################################################################
    //########################################################################################################################################

    inline void Target_Statistic::set(const std::string& key, double value, const std::wstring& unit)
    {
        if (!key.empty())
        {
            m_list[key] = { value, unit };
        }
    }
    inline void Target_Statistic::set(const std::string& key, int value, const std::wstring& unit)
    {
        if (!key.empty())
        {
            m_list[key] = { value, unit };
        }
    }
    inline void Target_Statistic::set(const std::string& key, const zfs::math::Vec3d& value, const std::wstring& unit)
    {
        if (!key.empty())
        {
            m_list[key] = { value, unit };
        }
    }

    inline Target_Statistic::target_statistic_value::type_t Target_Statistic::getType(const std::string& key) const
    {
        if (key.empty()) return target_statistic_value::type_t::UNDEFINED;
        const auto it = m_list.find(key);
        if (it == m_list.end()) return target_statistic_value::type_t::UNDEFINED;
        return it->second.t;
    }

    inline double Target_Statistic::getDouble(const std::string& key, double def) const
    {
        if (key.empty()) return def;
        const auto it = m_list.find(key);
        if (it == m_list.end()) return def;
        switch (it->second.t)
        {
        case target_statistic_value::type_t::DOUBLE: return it->second.d; break;
        case target_statistic_value::type_t::INT: return it->second.i; break;
        case target_statistic_value::type_t::VECTOR: return it->second.v.length(); break;
        default: return def;
        }
    }
    inline int Target_Statistic::getInt(const std::string& key, int def) const
    {
        if (key.empty()) return def;
        const auto it = m_list.find(key);
        if (it == m_list.end()) return def;
        switch (it->second.t)
        {
        case target_statistic_value::type_t::DOUBLE: return static_cast<int>(it->second.d); break;
        case target_statistic_value::type_t::INT: return it->second.i; break;
        case target_statistic_value::type_t::VECTOR: return static_cast<int>(it->second.v.length()); break;
        default: return def;
        }
    }
    inline zfs::math::Vec3d Target_Statistic::getVec(const std::string& key, const zfs::math::Vec3d& def) const
    {
        if (key.empty()) return def;
        const auto it = m_list.find(key);
        if (it == m_list.end()) return def;
        switch (it->second.t)
        {
        case target_statistic_value::type_t::VECTOR: return it->second.v; break;
        default: return def;
        }
    }

    inline std::wstring Target_Statistic::getUnit(const std::string& key) const
    {
        if (key.empty())
            return {};
        const auto it = m_list.find(key);
        if (it == m_list.end())
            return {};
        return it->second.unit;
    }



#pragma warning(pop)

    // ======================================================

} // end namespace
