#pragma once
#include "zfxyzapi.h"

#include <string>
#include <map>

namespace zfxyz
{
    /**
     * @brief Item of ZFS-Header
     */
    struct ZFXYZ_API ZFSHeaderItem
    {
        enum typ_t { T_UNDEF, T_INT, T_DOUBLE, T_2FLOATS, T_STRING };
    private:
        typ_t typ;          ///< Data type of the header item

        std::string sVal;   ///< String representation (set if type is string)
        union
        {
            int iVal;           ///< integer representation (set if type is integer)
            double dVal;        ///< double representation (set if type is a double)
            struct
            {
                float fVal1, fVal2; ///< 2 floats
            } f;
        } v;

    public:
        ZFSHeaderItem()
            : typ(T_UNDEF) {}
        ZFSHeaderItem(int value) { setValue(value); }
        ZFSHeaderItem(double value) { setValue(value); }
        ZFSHeaderItem(const std::string &value) { setValue(value); }
        ZFSHeaderItem(const std::wstring &value) { setValue(value); }
        ZFSHeaderItem(float f1, float f2) { setValue(f1, f2); }

        /**
         * @brief Set Item to value
         *
         * @param value The value to set
         */
        void setValue(int value);
        void setValue(double value);
        void setValue(const std::string &str);
        void setValue(const std::wstring &str);
        void setValue(float f1, float f2);

        /**
         * @brief Get value of item
         *
         * @note Default value is 0 for numeric values or "" for string
         *
         * @param success If pointer is provided it will be set to true or false
         * @return The value on success. Else default value
         */
        int getInt(bool *success = 0) const;
        double getDbl(bool *success = 0) const;
        std::string getStr(bool *success = 0) const;

        /**
         * @brief Get the two float values
         * @param f1 Float value 1
         * @param f2 Float value 2
         * @return true on success else false
         */
        bool get2Flt(float &f1, float &f2) const;

        /**
         * @brief Returns the type of the Item
         * @return Type of the Item
         */
        typ_t getType() const;
    };

    typedef std::map<int, ZFSHeaderItem> zfsheader_items_t;

    /**
     * @brief ZFSHeader String based access
     */
    class ZFXYZ_API ZFSHeaderStr
    {
    public:
        zfsheader_items_t m_items;

    public:
        ZFSHeaderStr();
        ~ZFSHeaderStr();

        void Clr() { m_items.clear(); }

        int GetInt(int idx) const;
        double GetDbl(int idx) const;
        std::string GetStr(int idx) const;
        bool Get2Flt(int idx, float &value1, float &value2) const;

        void SetInt(int idx, int value);
        void SetDbl(int idx, double value);
        void SetStr(int idx, const std::string &str);
        void SetStr(int idx, const std::wstring &str);
        void Set2Flt(int idx, float value1, float value2);

        void remove(int idx);

        /**
         * @brief Encode specific index to the string representation
         *
         * @param idx Index to encode
         * @return Item as encoded string or empty string if item isn't available
         */
        std::string toStr(int idx) const;

        /**
         * @brief Fills ZFSHeaderStr object form String-Buffer
         *
         * Object will contain only Elements from buffer. Previous content will
         * be discarded.
         *
         * @param buf Contains ZFSHeader as String encoded
         * @param len Length of the buffer
         * @return Number of Header elements parsed or -1 on error
         */
        int fromStrBuf(const char *buf, size_t len);
    };
} // namespace
