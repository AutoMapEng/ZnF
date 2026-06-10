#pragma once

#include "zfsapi.h"
#include "config.h"

#include "trmatrix.h"

#include <vector>

class ZFProject;

namespace zfs
{
    /**
    @brief  World handles world transformation
            World matrix will be managed by a callback handler
    */
    class ZFS_API ZFWorld : public ZFTRMatrix
    {
        DEFINE_ZFBASE(ZFTRMatrix)

        friend class ZFProject;

    public:
        ZFWorld();

        enum k_t {
            K_LOCAL,			///< no registration same as XYZ0
            K_ZFS,				///< registration stored in the zfs file header (default)
            K_PRJ_NOGLOBAL,		///< should be same as K_ZFS  (works only if chWorldHandler is installed who set the matrix, otherwise fallback to K_ZFS )
            K_PRJ_GLOBAL		///< with supirior koordinate system (works only if chWorldHandler is installed who set the matrix, otherwise fallback to K_ZFS )
        };

        virtual bool XYZ0(double& x, double& y, double &z) const = 0;
        virtual bool XYZ_(double& x, double& y, double &z) const = 0;

        bool    XYZ(double& x, double& y, double &z) const; 
        bool    XYZ(double& x, double& y, double &z, k_t k) const;

        /// return the matrix
        ZFTRMatrix Matrix(k_t k) const;			    

        /// return the matrix
        ZFTRMatrix InverseMatrix(k_t k) const;	

        /// return a pointer to the Project or zero if no project
        ZFProject* Project();				    

        typedef bool (*cbWorldMatrixHandler_f)(ZFBase* obj,ZFBase* context);
        static void registerWorldMatrixHandler(cbWorldMatrixHandler_f);

        // old interface
        const ZFTRMatrix* PrjMatrix(); ///< return the global world matrix
        /**
        \brief Transform a point from world coordinates to scanner coordinates.

        This will calculate the inverse matrix for each call, so do not use it in tight loops.
        */
        void pointToScanSystem(double& x, double& y, double& z) const;
        /**
        *   Applies the project matrix to the scanner position and returns the transformed position.
        *   \param[out] x => Return value for the x-axis value
        *   \param[out] y => Return value for the y-axis value
        *   \param[out] z => Return value for the z-axis value
        *   \param global => True the scanner position will be returned in global coordinates including the superior coordinate system.
        *                    False the scanner position will be returned in the global coordinate system without the superior coordinate system.
        */
        void getScannerPositionInProjectCoordinates(double& x, double& y, double& z, bool global = false) const;

    protected:
        static std::vector<cbWorldMatrixHandler_f> cbWorldMatrixHandler;
        void CallMatrixHandler(ZFBase*);

    public:
        ZFProject*  m_project;		///< pointer to the current project if one is open
        const ZFTRMatrix* m_matReg;		///< pointer to the matrix stored in the project file
        const ZFTRMatrix* m_matRegGlobal; ///< pointer to the global matrix stored in the project file
    };


} // end namespace

