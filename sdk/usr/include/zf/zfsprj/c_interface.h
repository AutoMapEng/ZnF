#pragma once
#ifndef ZFDB_INTERFACE
#define ZFDB_INTERFACE

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


# ifdef ZFSPRJ_EXPORTS
#  define ZFSPRJ_API __declspec(dllexport)
# else
#  define ZFSPRJ_API __declspec(dllimport)
# endif

/*
* SDK for reading Z+F database files
* 
* It uses a C only interface. So it should work with any 64bit compiler.
* 
* 
* The .zfdb is the database with local stored points and optional attributes like color, time, ...
* organized in level of details (LOD). LOD=0 has all the points. Beginning with LOD=1 the points are stored with a spacing
* LOD=0 can be empty
*
* A multicloud (.zfmdb) holds several clouds (.zfdb). 
* In the multicloud the registration matrix is stored for each zfdb
*
* It is not possible to open a zfdb/zfmdb several times at the same time
* 
* Very large scans are split into several databases and stored as a multicloud.
* The same happens for profiler data or import from other formats.
*/

/** required for enabling the library
* \return 1 if ok otherwise 0
*/
ZFSPRJ_API int ZFDBDLL_SetLicense(const char* customer, const char* expire, const char* license);
ZFSPRJ_API void ZFDBDLL_DestroyLicense();

//============================================================================================================================
	
//! @brief Struct containing Version related metadata.
typedef struct
{
	int32_t maj;	///< major version numer of the module
	int32_t min;	///< minor version numer of the module
	int32_t patch;	///< patch version numer of the module
}ZfdbVersion;

//! @brief Gets the version number of the library
//! @param Pointer to an empty ZfdbVersion struct which will be filled
ZFSPRJ_API void zfdbGetVersion(ZfdbVersion*);

//============================================================================================================================

enum
{
	ZFDB_ATTR_COLOR	= 1,
	ZFDB_ATTR_RF = 2,
	ZFDB_ATTR_TIME = 4,
	ZFDB_ATTR_MASK = 8,
	ZFDB_ATTR_BACKLINK = 0x10,
};

//============================================================================================================================

//! @brief Pointer of type void to the ZF Project Object
typedef void* zfproj;

//! @brief Pointer of type void to the ZF Database Object
typedef void* zfcloud;

//! @brief Struct containing the user provided data
typedef struct
{
	zfcloud db;
	zfproj prj;
}UserData;

//! @brief Opens a Database File. Returns a pointer of type void to the database object.
//! The pointer is stored in the ZFDBReader class and is used in subsequent functions.
//! If a Project File is provided opens that as well but it is ignored if the accompanying
//! database is a zfdb file. The reason being that, a zfdb is never referred to as a scan in
//! a project file, only zfmdbs. The underlying assumption is that the user wishes to reproject
//! the points in the database to a coordinate system available in the .zfprj file. Since this
//! cannot be done without the information stored in a zfmdb, the .zfprj file is ignored when
//! just a zfdb is provided.
//! @param fn : Absolute Path to the Database File
//! @param zfprj : Optional Project File
//! @returns A structure containing the opened Database and Project
ZFSPRJ_API UserData* zfdbOpen(const wchar_t* fn, const wchar_t* zfprj);

//============================================================================================================================

//! @brief Struct containing metadata related to a Level of Detail
typedef struct
{
	int32_t available;		///< lod is available
	double spacing;			///< the point spacing [meter]
	uint64_t points;		///< the number of points in this level of detail
}ZfdbLod;

//! @brief Struct containing info about a ZFDB
typedef struct
{
	int32_t min_lod_lvl;	///< min level of detail. Typically 0 or 1
	int32_t max_lod_lvl;    ///< max level of detail. Typically 9

	ZfdbLod lods[20];		///< a list of the first 20 (typically 10) LOD's stored in the database

	const char* fn;			///< zfdb filename or id

	double box_min[3];		///< Bounding Box of DB
	double box_max[3];

}dBInfo;

//! @brief Struct containing info about a ZFMDB
typedef struct
{
	size_t nClouds;			///< number of Clouds in Multicloud
	uint64_t pointCount;	///< total number of points in Multicloud

	int32_t min_lod_lvl;	///< min level of detail. Typically 0 or 1
	int32_t max_lod_lvl;    ///< max level of detail. Typically 9

	double finest_spacing;	///< finest spacing
	double box_min[3];		///< Bounding Box of MDB
	double box_max[3];

}MdBInfo;

//! @brief Pointer to callback function
typedef void (*GetInfoCallback)(void* dbInf);

//! @brief Retrieves all pertinent info about a Database
//! and prints out on the console using callback mechanism.
//! For ZFMDB files also retrieves info about each ZFDB it contains.
//! @param userData : The open database
//! @param cb : (optional) Pointer to callback function
//! @param mdbInf : A Pointer of void type to MdBInfo struct which will be filled
ZFSPRJ_API void zfdbInfo(zfcloud userData, GetInfoCallback cb, void* mdbInf);

//============================================================================================================================

//! @brief Retrieves all coordinate systems present in the project as a single wide string
//! separated by  $ sign. The active or current system is preceeded by * sign.
//! @param prj : The open project
//! @param csNames : A pointer of void type to the wide string
ZFSPRJ_API void zfprjGetCoordinateSystems(zfproj prj, void* csNames);

//============================================================================================================================

//! @brief Retrieves the next denser (higher) or sparser (lower)
//! Level of Detail for a specified point-spacing.
//! @param userData : The open database
//! @param spacing : The specified Point-spacing
//! @param sparser : Flag indicating if the next lower level of detail is desired
//! @returns The LOD
ZFSPRJ_API int zfdbGetLODforSpacing(zfcloud userData, const double spacing, bool denser);

//============================================================================================================================

//! @brief Retrieves the Number of points inside a given Level of Detail
//! @param userData : The open database
//! @param lod : The specified LOD
//! @returns The Number of points in the given LOD
ZFSPRJ_API uint64_t zfdbGetNumPoints(zfcloud userData, const int32_t lod);

//============================================================================================================================

//! @brief Struct containing metadata about a single point
typedef struct
{
	double x, y, z;			///< x,y,z as meter not registered
	unsigned char r, g, b;	///< gray value or color information
	double time;			///< (optional) time
	uint32_t rf;				///< (optional) intensity or gray value
	unsigned char mask;		///< (optional) point is marked as noise
}ZfdbPoint_t;

//! @brief Pointer to callback function
typedef void (*GetPointCallback)(void* userPointData);

//! @brief Retrieves points inside a given Level of Detail, projects them to a coordinate
//! system of user choice and returns the point using callback mechanism.
//! When no Project file is provided the coordinate system of the returned points is same
//! as what is stored in the Database after taking the orientation and translation of the
//! zfdb into consideration. However, if a project file is provided then the points will
//! be projected to a coordinate system of user choice. In case the Global Matrix of the
//! chosen coordinate system is an Identity Matrix then the points will be projected using
//! the registration matrix at the Scan Position but again after taking the orientation and
//! translation of the zfdb into consideration. If no coordinate system is chosen the 'Default'
//! system is applied (the Matrix of which may not always be Unity). If the chosen coordinate
//! system does not exist in project the current/active system in the project is applied.
//! @param userData : The open database and project (if provided)
//! @param mdbName : The zfmdb file name without path and extension. Empty incase of zfdb or no project.
//! @param lod : The specified LOD
//! @param coordSys : The specified coordinate system name
//! @param cb : Pointer to callback function
ZFSPRJ_API void zfdbGetPoints(UserData userData, const wchar_t* mdbName, const int32_t lod, const wchar_t* coordSys, GetPointCallback cb);

//============================================================================================================================

//! @brief Closes the database
//! @param userData : The open database
ZFSPRJ_API void Close(zfcloud userData, zfproj userPrj);

//============================================================================================================================

#ifdef __cplusplus
} // extern "C"
#endif

#endif// ZFDB_INTERFACE
