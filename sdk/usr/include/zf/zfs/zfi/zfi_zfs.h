#pragma once
/**
    !!! There is a faster memory mapped version as well. zfi_mmap.h
    Please use memory mapped version whenever possible
*/

#include "../zfsapi.h"
#include "../compiler.h"
#include "../Tools/progress.h"
#include "../file_extensions.h"

#include <string>
#include <vector>
#include <map> 
#include <cstdint>


#define ZFI_EXT	ext_zfi			        ///< file extension for imagecontainer
#define ZFI_EXT_L	wext_zfi			///< file extension for imagecontainer

#define ZFI_ID	        0x11223344		///< file id
#define ZFI_ID_REMOVED	0x11223355		///< file id for deleted files

namespace zfs
{
    /// Return the zfi name of the coresponding scan ant test is exists
    ZFS_API std::wstring ZfiOfZfs(const std::wstring& zfs, bool checkExists = true, bool isThermal = false);


    /**
     * @brief Mode definition, how the given name is matched.
     */
    enum MatchMode {
        MATCH_EXACT,       // name must match exactly
        MATCH_INDEX,       // index must match
        MATCH_INDEX_HDR,    // index and hdr id must match
        MATCH_THUMB        // index must match and name contains ";thumb"
    };

    ZFS_API bool nameMatches(const std::wstring& name1, const std::wstring& name2, MatchMode mode);

    ZFS_API int getIdFromName(const std::wstring& name);

    typedef std::wstring zfi_str_t;
    typedef std::vector< zfi_str_t > zfi_str_list_t;

    typedef std::map<int, zfi_str_t > zfi_str_map_t;

    /**
    * @brief Restore the original entries.
    *
    * @param zfi name of the zfi file
    * @return true if the original images has been restored.
    */
    ZFS_API bool ZfiRestore(const std::wstring& zfi);

    ZFS_API zfi_str_map_t ZfiMap(const std::wstring& zfi, bool absolutepath = true, int id = ZFI_ID);             ///< Return map of all images in the container

    ZFS_API zfi_str_list_t ZfiList(const std::wstring& zfi, bool absolutepath = true, int id = ZFI_ID);               ///< Return list of all images in the container
    ZFS_API zfi_str_list_t ZfiExtract(int kenn, const std::wstring& zfi, const std::wstring& subfolder = L"", zf::progress_f cb=nullptr );      ///< Extract all files in the container
    ZFS_API zfi_str_list_t ZfiHdrSerie(const std::wstring& zfi, int id);                                          ///< List of all images depending on one HDR serie
    ZFS_API zfi_str_map_t ZfiHdrSerieMap(const std::wstring& zfi, int id, int zfi_id);                              ///< List of all images with index depending on one HDR serie and the zfi id

    ZFS_API std::string ZfiReadTextFile(int kenn, const std::wstring& zfi, const std::wstring& file, int maxSize);	///< read file from container as a textfile

    /**
    * @brief Reads the given file as raw data.
    *
    * The caller owns the data and has to delete the data structure!
    *
    * @param zfi            the name of the zfi container
    * @param ifn            the name of the data to be loaded
    * @param length         outputs the number of bytes read from ZFI
    * @param mode           matching mode for the name
    * @param zfs_id         the ID to search for
    *
    * @return the raw data or NULL if the entry was not found
    */
    ZFS_API unsigned char* ZfiLoadRawData(const std::wstring& zfi, const std::wstring& ifn, int& length, MatchMode mode = MATCH_EXACT, int zfs_id = ZFI_ID);

    /**
     * @brief Variant of ZfiLoadRawData returning the data as std::vector
     *
     * @param zfi            the name of the zfi container
     * @param ifn            the name of the data to be loaded
     * @param mode           matching mode for the name
     * @param zfs_id         the ID to search for
     *
     * @return the raw data or empty vector if not found
     */
    ZFS_API std::vector<uint8_t> ZfiLoadRawData(const std::wstring& zfi, const std::wstring& ifn, MatchMode mode = MATCH_EXACT, int zfs_id = ZFI_ID);

    /**
    * @brief Marks the given image as removed. To achieve this, the id is set to ZFI_ID_REMOVED.
    *
    * @param zfi_container  the name of the zfi container
    * @param imageName      the name of the image to remove
    * @param mode           matching mode for the name
    *
    * @return true if the image was removed, false otherwise
    */
    ZFS_API bool ZfiRemove(const std::wstring& zfi_container, const std::wstring& imageName, MatchMode mode = MATCH_EXACT);
    ZFS_API bool ZfiChangeId(const std::wstring& zfi_container, const std::wstring& imageName, MatchMode mode = MATCH_EXACT, int zfi_id = ZFI_ID_REMOVED, int startIndex = 0, int stopIndex = -1);

    ZFS_API bool ZfiFind(const std::wstring& zfi_container, const std::wstring& imageName, MatchMode mode = MATCH_EXACT, int zfi_id = ZFI_ID, int startIndex = 0, int stopIndex = -1);

    /**
     * @brief Add file to ZFI container
     *
     * Appends content of file2add to zfi_container. If zfi_container doesn't
     * exist, is is created.
     *
     * UTF-16 variant
     *
     * @param zfi_container the name of the zfi container to add the file to
     * @param file2add      the file to add to the container
     * @param delsource     if true the source file is deleted (after sucessfull
     *                      add)
     * @return true if file was added successfully
     */
    ZFS_API bool ZfiAdd(const std::wstring& zfi_container,
        const std::wstring& file2add,
        bool delsource,
        int id = ZFI_ID);

    /**
     * @brief Add file to ZFI container
     *
     * Appends content of file2add to zfi_container. If zfi_container doesn't
     * exist, is is created.
     *
     * UTF-8 variant
     *
     * @param zfi_container the name of the zfi container to add the file to
     * @param file2add      the file to add to the container
     * @param delsource     if true the source file is deleted (after sucessfull
     *                      add)
     * @return true if file was added successfully
     */
    ZFS_API bool ZfiAdd(const std::string& zfi_container,
        const std::string& file2add,
        bool delsource,
        int id = ZFI_ID);

    /**
    *@brief Add item to zfi container. Create container if it does not exist
    *@param filename the filename to be stored
    *@param memory pointer to memory
    *@param sizeOfMemoryToAdd bytes in the memory to be stored in the zfi
    */
    ZFS_API bool ZfiAdd(const std::string& zfi_container,
        const std::string& filename,
        const char* memory,
        int sizeOfMemoryToAdd,
        int id = ZFI_ID);

    ZFS_API bool ZfiAdd(const std::wstring& zfi_container,
        const std::wstring& filename,
        const char* memory,
        int sizeOfMemoryToAdd,
        int id = ZFI_ID);


    /**
    @brief final remove entries by creating a new zfi container with all not deleted files and replace it with current
    @param backup if a backup of orginal should be created
    @return true if success
    */
    ZFS_API bool ZfiUnify(const std::wstring& zfi_container, bool backup );

} // end namespace zfs
