#pragma once

#include "ZfsPrjBasic.h"

#include <string>
#include "Project.h"

namespace zfprj
{
	namespace simulator
	{

        /**
        \brief create a new project just for simulation and automatic tests
        */
		class ZFSPRJ_API ProjectSimulator
		{
            ZFProject proj;

		public:
            ProjectSimulator(const std::wstring& absPrjPathName, bool recreate);

            std::wstring ProjectPath() const;
            std::wstring ProjectFileName() const;

            /// just add the entry to the project, the scan itself may does not exist
			bool AddScan( const std::wstring& scanName );

            /// create a simulated scan and add it to the project
            bool AddSimulatedScan(const std::wstring& scanName, const std::wstring& simParams);

            /// add a target to the project
            bool AddTarget( const std::wstring& scanName, const std::wstring& targetName, double x, double y, double z, int flags=0 );

            /// write the project to the disk
            bool Store();

            ZFProject& prj() { return proj; }

        };


	}
} // end namespace
