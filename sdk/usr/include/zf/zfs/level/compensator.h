#pragma once

#include "../zfsapi.h"

#include <vector>
#include <string>
#include <tuple>

#include <zfcore/zfmath.h>

#include "../flags.h"
#include "../Math/zfvec.h"
#include "../trmatrix.h"

#include "level_errors.h"

#define SAVE_LEVEL_3POINT_SCANHEADER


class ZFScanBasic;
class ZFSLinebyline;
class ZFTRMatrix;
class ZFSHeader;
namespace zfs
{
	namespace level
	{
		class levelParams;
	}
}

namespace zfs
{

/** /brief Level as Dynamischer Compensator
           Moveing while scanning will be compensated

		Compensator:
			uses levelParams in calcLevel.h
				uses registerLevel in level_register.h
*/
	class ZFS_API Compensator
	{
	public:
		bool flag_level_corr{};					///< true if compensator is present / possible
		std::vector<long> x_level_corr_glob;	///< [µrad] global Scannersystem
		std::vector<long> y_level_corr_glob;	///< [µrad] global Scannersystem

		std::vector<int> dynamic_error;			///< Lines between tilting changes are too big (>0,007grad), are saved

		double dist_kos_tripod{};				///< [meter] distance between mirror and tripod rotating center
		double m_horz_deg{};					///< the horz angle scanned

		double Cxx[4][4]{};						///< [rad], Covariance Matrix (ny,nx,sy,sx) ==> n_Neigung,s_Spielpunkt der Libelle (Offset) 
		double level_x[4]{};					///< [rad], (ny,nx,sy,sx) ==> n_Neigung,s_Spielpunkt der Libelle (Offset)
		bool levelOutRange{};					///< if level during scan out of level range => true, otherwise false

	protected:
		bool isUsed{ true };						///< switch on/off temponarely
		level::eErrorCode m_error{};							///< error of calculation
		std::string info;						///< some internal information
		zfs::zfs_flags_t* pFlags{};				///< point to flags fiel of scan
		std::string m_hardware;					///< the hardware used from zfs-header index 9011
		double m_workingrange{ 0.5 };			///< the working range of the level system from zfs-header 9013


	public:
		Compensator() = default;

		// Init Compensator
		bool Init(ZFScanBasic* z, bool level_fl, bool comp_fl, bool updateZfsHeader=true );		// return false if compensator is not used
		bool Init(ZFSLinebyline* z, bool level_fl, bool comp_fl, bool updateZfsHeader = true);		// return false if compensator is not used
		bool Init_(bool use, const ZFSHeader* zh, zfs::level::levelParams& Level_Corr, level::eErrorCode, int& levelMatrixStatus, ZFTRMatrix& levelMatrix, ZFTRMatrix* matrix, zfs::zfs_flags_t&, bool level_fl, bool comp_fl);

		// neue umschlagswerte in den zfs-header schreiben falls welche vorhanden
		bool mofifyZfsHeader(ZFSHeader* hd);

		bool IsPresent() const;
		bool IsUsed() const;
		void Use(bool u);

		// return the errorcode
		level::eErrorCode error() const { return m_error; }

		// Neigung
		std::pair<double, double> inclination_rad() const { return { level_x[1], level_x[0] }; }
		std::pair<double, double> inclination_deg() const { return { zf::rad2deg(level_x[1]), zf::rad2deg(level_x[0]) }; }

		std::pair<double, double> spielpunkt_rad() const { return { level_x[3], level_x[2] }; }
		std::pair<double, double> spielpunkt_deg() const { return { zf::rad2deg(level_x[3]), zf::rad2deg(level_x[2]) }; }

		/// The hardware used. See firmware for ID
		const std::string& hardware() const { return m_hardware; }

		double working_range() const { return m_workingrange; }

		double horz_deg() const { return m_horz_deg; }

		ZFTRMatrix matrix() const;

		/// Compensate a xyz value
		bool Compensate(int ln, zfs::math::Vec3d& vec) const { return Compensate(ln, vec.x(), vec.y(), vec.z()); }
		bool Compensate(int ln, double& x, double& y, double& z) const;

		const char* Info() const { return info.c_str(); }

		enum {
			ZFSHEADER_9009_UMSCHLAG = 1,
			ZFSHEADER_9009_DYNCOMP = 2,
			ZFSHEADER_9009_DISABLED = 4
		};

		// for automatic tests
		std::string toJson(int space) const;

	public:
		int m_9009{};
		std::vector< std::tuple<double, double, double> >m_22000_deg;	///< the values should be stored in the zfs-header to simulate a l3point measurement. 3 * x,y,horz

	protected:
		void setupBasicsViaZFSHeader(const ZFSHeader*);
};

//######################################################################################

inline bool Compensator::IsUsed() const
{
    return flag_level_corr && isUsed;
}


//===============================================================================================================

/**
/brief class for temponarly enable/disable compensator
*/
class ZFS_API CompensatorUsage
{
	Compensator* comp;
	bool last;

public:
	CompensatorUsage( Compensator* c, bool en ) : comp(c) { last=comp->IsUsed(); comp->Use(en); }
	virtual ~CompensatorUsage() { comp->Use(last); }
};


//===============================================================================================================

/**
@brief DynCompensatorInfo  Read out information stored in the zfs header. Does not check if calculation is possible
*/
ZFS_API std::wstring DynCompensatorInfo(const ZFSHeader*);

} // end namespace zfs

