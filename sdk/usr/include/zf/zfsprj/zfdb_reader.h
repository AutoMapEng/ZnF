#pragma once

#include "c_interface.h"

#include <string>
#include <sstream>
#include <functional>

namespace zfprj
{
	enum open_status
	{
		OPEN_FAILED = 0,
		OPEN_DB_SUCCESS,
		OPEN_PRJ_SUCCESS
	};

	/**
	 * Struct for converting a Lambda to a Function Pointer
	 */
	struct Lambda {	
		template<typename Tret, typename T>
		static Tret lambda_ptr_exec(void* data) {
			return (Tret)(*(T*)fn<T>())(data);
		}

		template<typename Tret = void, typename Tfp = Tret(*)(void*), typename T>
		static Tfp ptr(T& t) {
			fn<T>(&t);
			return (Tfp)lambda_ptr_exec<Tret, T>;
		}

		template<typename T>
		static void* fn(void* new_fn = nullptr) {
			static void* fn;
			if (new_fn != nullptr)
				fn = new_fn;
			return fn;
		}
	};

	/**
	 * C++11 class for access zfdb/zfmdb database via the c_interface.
	 */
	class ZFDBReader
	{
	public:
		using InfoWriterCallback = std::function<void(void* dbInfo)>;
		using PointWriterCallback = std::function<void(void* userPointData)>;
		std::wstring csNames{};

		ZFDBReader()
		{
			zfdb = nullptr;
			zfprj = nullptr;
			uDbFn = L"";
			csNames = L"";
		}

		virtual ~ZFDBReader()
		{
			Close(zfdb, zfprj);
			zfdb = nullptr;
			zfprj = nullptr;
			uDbFn = L"";
			csNames = L"";
		}

		//! @brief Gets the version number of the library
		//! @param Pointer to an empty ZfdbVersion struct which will be filled
		void getVersion(ZfdbVersion* dbVers)
		{
			zfdbGetVersion(dbVers);
		}

		//! @brief Opens a Database File. Returns a pointer of type void to the database object.
		//! The pointer is stored in the Database Reader class and is used in subsequent functions.
		//! If a Project File is provided opens that as well but it is ignored if the accompanying
		//! database is a zfdb file. The reason being that, a zfdb is never referred to as a scan in
		//! a project file, only zfmdbs. The underlying assumption is that the user wishes to reproject
		//! the points in the database to a coordinate system available in the .zfprj file. Since this
		//! cannot be done without the information stored in a zfmdb, the .zfprj file is ignored when
		//! just a zfdb is provided.
		//! @param fn : Absolute Path to the Database File
		//! @param prjFile : Optional Project File
		//! @returns 1 if DB is open, 2 if Project is also open, 0 if DB could not be opened
		int open(const std::wstring& fn, const std::wstring& prjFile = {});		

		//! @brief Getter for the Cloud
		zfcloud getCloud()
		{
			return this->zfdb;
		}

		//! @brief Getter for the Project
		zfproj getProject()
		{
			return this->zfprj;
		}

		//! @brief Retrieves all pertinent info about a Database
		//! and prints out on the console using callback mechanism.
		//! For ZFMDB files also retrieves info about each ZFDB it contains.
		//! @param callback : (optional) Pointer to callback function
		//! @param mdbInf : A Pointer of void type to MdBInfo struct which will be filled
		void getInfo(void* mdbInf, InfoWriterCallback callback = nullptr);

		//! @brief Retrieves the next denser (higher) or sparser (lower)
		//! Level of Detail for a specified point-spacing.
		//! @param spacing : The specified Point-spacing
		//! @param sparser : Flag indicating if the next higher level of detail is desired
		//! @returns The LOD
		int getLOD(const double spacing, bool denser = false);

		//! @brief Retrieves the Number of points inside a given Level of Detail
		//! @param lod : The specified LOD
		//! @returns The Number of points in the given LOD
		uint64_t getNumPoints(const int32_t lod)
		{
			return zfdbGetNumPoints(this->zfdb, lod);
		}

		//! @brief Retrieves points inside a given Level of Detail, projects them to a coordinate
		//! system of user choice and returns the point using callback mechanism.
		//! When no Project file is provided the coordinate system of the returned points is same
		//! as what is stored in the Database. However, if a project file is provided then the points will
		//! be projected to a coordinate system of user choice. In case the Global Matrix of the
		//! chosen coordinate system is an Identity Matrix then the points will be projected using
		//! the registration matrix at the Scan Position after taking the orientation and
		//! translation of the zfdb into consideration. If no coordinate system is chosen the 'Default'
		//! system is applied (the Matrix of which may not always be Unity). If the chosen coordinate
		//! system does not exist in project the current/active system in the project is applied.
		//! @param lod : The specified LOD
		//! @param cs : (optional) The specified coordinate system name
		//! @param callback : Pointer to callback function
		void getPoints(const int32_t lod, const std::wstring& cs, PointWriterCallback callback)
		{
			if (!callback) return;

			void (*funPtr)(void*) = Lambda::ptr(callback);

			if (funPtr != nullptr)
			{
				UserData ud{};
				ud.db = this->zfdb;
				if (this->zfprj)
					ud.prj = this->zfprj;

				const std::wstring def = L"Default";
				if (cs.empty())
					zfdbGetPoints(ud, (this->uDbFn).c_str(), lod, def.c_str(), funPtr);
				else
					zfdbGetPoints(ud, (this->uDbFn).c_str(), lod, cs.c_str(), funPtr);
			}
		}

	private:
		zfproj zfprj{};
		zfcloud zfdb{};
		std::wstring uDbFn{};
		
	};	

	int ZFDBReader::open(const std::wstring& fn, const std::wstring& prjFile)
	{
		int rc{};
		std::wstring prjFn = prjFile;
		const std::wstring inFilename = fn.substr(fn.find_last_of(L"/") + 1);
		const size_t p(inFilename.find_last_of('.'));
		const std::wstring file_extension = inFilename.substr(p, fn.size() - 1);

		if (file_extension == L".zfdb")
			prjFn = L""; // Ignoring Project incase of zfdb
		else if (prjFile.size() > 0 && file_extension == L".zfmdb")
			this->uDbFn = inFilename.substr(0, p); // Filename without extension is only needed to search a ZFMDB in a Project

		auto data = zfdbOpen(fn.c_str(), prjFn.c_str());

		if (!data || !data->db)
			return OPEN_FAILED;

		rc = OPEN_DB_SUCCESS;
		this->zfdb = data->db;
		if (data->prj)
		{
			rc = OPEN_PRJ_SUCCESS;
			this->zfprj = data->prj;
		}

		delete data;
		return rc;
	}

	void ZFDBReader::getInfo(void* mdbInf, InfoWriterCallback callback)
	{
		if (callback)
		{
			GetInfoCallback* funPtr = callback.target<GetInfoCallback>();

			if (callback != nullptr)
				zfdbInfo(this->zfdb, *funPtr, mdbInf);
		}
		else
		{
			zfdbInfo(this->zfdb, nullptr, mdbInf);
		}
		
		if(this->zfprj)
			zfprjGetCoordinateSystems(this->zfprj, static_cast<void*>(&this->csNames));
	}

	int ZFDBReader::getLOD(const double spacing, bool denser)
	{
		return zfdbGetLODforSpacing(this->zfdb, spacing, denser);
	}
	
}


