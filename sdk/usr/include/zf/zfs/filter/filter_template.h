#pragma once
#include "../zfsapi.h"

#include <string>
#include <vector>


namespace zfs
{
	class FilterParams;


	/*
	* search in the path for a filter template configuration file
	* each device type has its own template file
	* searches in the directory for files called template.<type>.<profiler|imager>.xml/.ini.
	* 
	* The template .ini and .xml are not always identical. The .ini file is used for language translation 
    * and therefore contains definitions of all filters and parameters, even those which are incompatible 
    * with the scanner of current scan. Hence it is strongly recommended to load from template .xml. 
    * This means if buildFilterTemplateName(model_code, usedProfiler) is used to create the template name 
    * then a name.ReplaceExt(".ini", ".xml") must be done to access the template .xml. 
    *
    * Incompatibilty with scanner is set by setVisible() method. In template .xml only visible items 
    * are stored. To maintain consistency, when loading is done from template .ini the invisible items 
    * are skipped.
    *
    * \return FilterParams
	*/
	ZFS_API bool loadFilterParamsFromTemplate(const std::wstring& path, int type, bool profiler, FilterParams&, bool createTemplateIfNotExist = false);
	ZFS_API bool loadFilterParamsFromTemplate(const std::wstring& scanPath, const std::wstring& zfsScanFile, FilterParams& prm, bool createTemplateIfNotExist = false);

	// create the basic filter templates for supported scanner and devices like 5010,5016,9020
	ZFS_API std::vector<std::wstring> createFilterTemplates(const std::wstring& filterTemplatePath, bool recreate=false);

	ZFS_API std::wstring buildFilterTemplateName(int type, bool profiler);

}