#include <string>

#include <zfsfilter/filterapi.h>
#include <zfssim/createzfs/createzfs.h>

namespace zfsfilter
{
	FILTERDLL_API bool generateSimulationTestScan( const std::wstring& test_base_directory,
		                                           const std::wstring& test_name, 
		                                           zfssim::createzfs_param_t::typ_t scanner_typ, 
		                                           zfs::resolution_t resolution,
		                                           bool dual_layer,
		                                           bool overwrite,
		                                           std::wstring& test_scan_name, 
		                                           std::wstring& test_scan_file_name, 
		                                           std::wstring& test_scan_file_path );
}