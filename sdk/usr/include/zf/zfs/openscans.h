#ifndef __ZFS_OPENSCNAS_H__
#define __ZFS_OPENSCNAS_H__

#include "zfsapi.h"

#include <list>
#include <string>

namespace zfs
{

	/**
	@brief Locale Liste der geoeffneten scans im ZFScan Mode
	Diese Liste dient dazu ein weiteres oeffnen des scans zu verhindern
	Im gegensatz zur scanlist werden keine callbacks oder sonstige aktionen behandelt
	*/
	class ZFS_API OpenScanList
	{
		std::list<std::wstring> m_openScanList;

	public:

		/// check is scan is already open
		bool isAlreadyInList(const std::wstring& fn);

		/// add scan to the list of open files
		bool add(const std::wstring& fn);

		/// remove the scan from the list
		bool remove(const std::wstring& fn);

		/// number of open scans
		unsigned int size();

		/// get list of open scans
		std::list<std::wstring> getList();
	};

	extern ZFS_API OpenScanList openScans;


} // end namespace
#endif //__SCANLIST_H__
