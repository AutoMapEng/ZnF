#pragma once

#include "../zfsapi.h"
#include "zfi_zfs.h"

#include <string>
#include <map>
#include <vector>
#include <memory>

#include <stdint.h>

#define ZFI_MMAP

#ifdef ZFI_MMAP


namespace boost {
    namespace iostreams {
        class mapped_file;
    }
}

namespace zfs
{
    //######################################################################################################################

    /**
    * @brief a single item in the zfi container
    */
    class ZFS_API ZfiItem
    {
    public:
        ZfiItem(const std::wstring& _name, const char* _pos, uint64_t _rel, uint64_t _size, int _id = -1, int _zfi_id = ZFI_ID)
            : name(_name)
            , pos(_pos)
            , rel(_rel)
            , size(_size)
            , id(_id)
            , zfi_id(_zfi_id)
        { }

        ZfiItem()
            : pos(0)
            , size(0)
        { }

#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::wstring name;	///< mapped name
#pragma warning(pop)
        const char* pos;	///< position (memory pointer)
        uint64_t rel;       ///< position in file
        uint64_t size;		///< size of the item
        int id;             ///< position index in the container
        int zfi_id;         ///< zfi id of the item (i.e. ZFI_ID, ZFI_ID_REMOVED)
    };


    //######################################################################################################################

    /**
    * @brief Holds all items in a zfi container for fast access
    *        Uses file mapping for access
    */
    class ZFS_API Zfi
    {
    public:
        typedef std::pair<int, int> itemkey_t;
        typedef std::map<itemkey_t, ZfiItem> itemmap_t;

        /**
         * @brief Create Zfi-Object without associated file
         */
        Zfi();

        /**
         * @brief Create Zfi-Object and open sepcified @c zfiFile
         * On success isOpen() returns true
         * @param zfiFile Path to file to open
         */
        Zfi(const std::wstring& zfiFile);

        /**
         * @brief Open the file and get a list of items in the container
         * @param zfiFile the name of the ZFI container
         * @return true if opening succeeded, false otherwise
         */
        bool open(const std::wstring& zfiFile);

        /**
         * @brief Close Zfi file and release all resources
         */
        void close();

        /**
         * @brief Test weather Zfi file is Open
         * @return true if open
         */
        bool isOpen() const;

        /**
         * @brief Get the ZFI filename
         *
         * @return the ZFI filename
         */
        const std::wstring& getFileName() const;

        /**
         * @brief Search for items in the ZFI container. The found items are returned in the provided list.
         *
         * @param list          the result list (in/out)
         * @param search        the name of the file to be searched
         * @param zfi_id        the zfi id (ZFI_ID or ZFI_ID_REMOVED)
         * @param mode          the MatchMode for the name
         * @param startIndex    the start index in the ZFI file
         *
         */
        void find(std::vector<ZfiItem>& list, const std::wstring& search, int zfi_id = ZFI_ID, MatchMode mode = MatchMode::MATCH_EXACT, int startIndex = 0) const;

        /**
         * @brief Get all items of a HRD series.
         *
         * @param list          the result list (in/out)
         * @param id            the id of the image
         * @param zfi_id        the zfi id (ZFI_ID or ZFI_ID_REMOVED)
         * @param startIndex    the start index in the ZFI file
         */
        void getHdrSerie(std::vector<ZfiItem>& list, int id, int zfi_id = ZFI_ID, int startIndex = -1) const;

        /**
         * @brief Get a pointer to the memory for the item
         *
         * @return the pointer so the memory
         */
        const char* ptr(const ZfiItem* it) const;

        /**
         * @brief Get a list of all items in the container
         *
         * @param zfi_id    the zfi id (ZFI_ID or ZFI_ID_REMOVED)
         *
         * @return a vector containing the names of the corresponding items
         */
        std::vector<std::wstring> getList(int _zfi_id = ZFI_ID) const;

        /**
         * @brief Get a map of all items in the container
         *
         * @param zfi_id the zfi id (ZFI_ID or ZFI_ID_REMOVED)
         *
         * @return the map containing the corresponding items
         */
        zfi_str_map_t getMap(int _zfi_id = ZFI_ID) const;

        /**
         * @brief Get all items. Do not change them!
         *
         * @return the items
         */
        const itemmap_t& getItems() const;

        /**
         * @brief Get the item count in the container.
         *
         * @param zfi_id the zfi id (ZFI_ID or ZFI_ID_REMOVED)
         * @return the item count
         */
        int itemsInContainer(int _zfi_id = ZFI_ID) const;

    private:
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        std::wstring zfiFileName;				///< absolute file name of the zfi container
        itemmap_t items;                        ///< the list of items in the container
        std::shared_ptr<boost::iostreams::mapped_file> file;     ///< memory mapping
#pragma warning(pop)
    };

    //######################################################################################################################

    /**
    \brief try to repair a zfi file
    */
    ZFS_API std::string ZfiRepair(const std::wstring& zfi_container_filename);

} // end namespace

#endif // ZFI_MMAP
