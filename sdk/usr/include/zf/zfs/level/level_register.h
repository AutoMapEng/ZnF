#pragma once
#include "../zfsapi.h"

#include <string>

#include <zfcore/compiler.h>
#include "../trmatrix.h"
#include "level_errors.h"
#include "../Math/zfvec.h"


#define DIST_KOS_TRIPOD	(-337.3/1000.)	    // distance between mirrorcenter and level-rotations center [mm]

class ZFSHeader;


namespace zfs
{
    class Compensator;

    namespace level
    {

        /**
        \brief Class for calculating transformationmatrix from tilt measurements for levelcompensator
        -> Tilt measurementvalues and the levelcalibrationvalues are read form the ZFSheader.
        -> Levelcalibration is calculated. Here the tiltvalues will get corrected with the Parameters levelRot_rad and levelAxis.
        -> Rotation matrix R is calculated by one of the three modes (Standard: EV) and the corrected tiltvalues.
        -> The transformation matrix M2 is calculated by dist and R. M2 is composed of tree transformations.
            -mirrorcenter to tripod (dist)
            -tripod to horizontal tripod (R)
            -horizontal tripod to mirrorcenter (-dist)
        \param ZFSHeader
                Level calibration parameters
                22960 = angle between level axes [deg] (Standard: 90)
                22961 = level rotation around z-axis [deg] (Standard: 0)
                1. measurement
                22022 = angle encoder [deg]
                22020 = level-x [deg]
                22021 = level-y [deg]
                2. measurement
                22122 = angle encoder [deg]
                22120 = level-x [deg]
                22121 = level-y [deg]
                3. measurement
                22222 = angle encoder [deg]
                22220 = level-x [deg]
                22221 = level-y [deg]
            dist -> distance between mirrorcenter and level-rotations center
            mode -> NORMALS, MEAN, EV (Standard)
        \return true/false (if true, result in M2 -> tranformation matrix for leveling)
        */
        class ZFS_API registerLevel
        {
        public:

            enum mode
            {
                NORMALS,    //!< mode NORMALS calculates the rotationmatrix over the angle of the normalvectors of each tilt measurements
                MEAN,       //!< mode MEAN calculates the rotationmatrix directly over the meanvalue of all tilt measurements
                EV          //!< (Standard) mode EV calculates the rotationmatrix over the angle of the eigenvectors of each tilt measurements
            };


            registerLevel() = default;
            virtual ~registerLevel() {}

            const double* getlevel_roll_in() const { return level_roll_in; }
            const double* getlevel_pitch_in() const { return level_pitch_in; }
            const double* getlevel_lenc_in() const { return level_lenc_in; }


            /**
            \brief calculating transformationmatrix from tilt measurements for levelcompensator
            -> Tilt measurementvalues and the levelcalibrationvalues are read form the ZFSheader.
            -> Levelcalibration is calculated. Here the tiltvalues will get corrected with the Parameters levelRot_rad and levelAxis.
            -> Tilt measurementvalues from first and third position are checked for movements, if the movement is bigger than
               max_tilt_changes an error (ERROR_Movement) will be returned and the transformation matrix M2 will be not calculated.
            -> Rotation matrix R is calculated by one of the three modes (Standard: EV) and the corrected tiltvalues.
            -> The transformation matrix M2 is calculated by dist and R. M2 is composed of three transformations.
                -mirrorcenter to tripod (dist)
                -tripod to horizontal tripod (R)
                \param ZFSHeader
                    Level calibration parameters
                    15523 = level rotation around z-axis [deg] (Standard: 0)
                    15524 = angle between level axes [deg] (Standard: 90)
                    1. measurement
                    22022 = angle encoder [deg]
                    22020 = level-x [deg]
                    22021 = level-y [deg]
                    2. measurement
                    22122 = angle encoder [deg]
                    22120 = level-x [deg]
                    22121 = level-y [deg]
                    3. measurement
                    22222 = angle encoder [deg]
                    22220 = level-x [deg]
                    22221 = level-y [deg]
                /param dist -> distance between mirrorcenter and level-rotations center
                /param max_tilt_changes -> maximal allowed tilt changes between level measurements [deg] (Standard: 0.1)
                /param mode -> NORMALS, MEAN, EV (Standard)
                /param useAll Flag ob auch 5006 Scanner verwendet werden sollen
                /return true/false (if true, result in M2 -> tranformation matrix for leveling)
            */
            eErrorCode init(const ZFSHeader* head, const zfs::Compensator& co, double dist = DIST_KOS_TRIPOD, int mode = registerLevel::EV, double max_tilt_changes = 0.1 );

            // setup for recalculation
            eErrorCode setup(const ZFSHeader* head, const zfs::Compensator& co, bool load_22022, double dist = DIST_KOS_TRIPOD, int mode = registerLevel::EV, double max_tilt_changes = 0.1);

            ZF_DEPRECATED("Bitte alternative funktion mit zfs::Compensator als Param benutzen")
            eErrorCode init(const ZFSHeader* head, bool useAll, double dist = DIST_KOS_TRIPOD, int mode = registerLevel::EV, double max_tilt_changes = 0.1, bool flag_open = true);


            eErrorCode init5024(const ZFSHeader* head );

            /// write matrix into zfs-header starting at index 320
            //bool write( ZFSHeader *head );

            /** \brief calculates the transformationmatrix from leveled points (for testing)
            \param xs, ys, zs,xsh, ysh, zsh (Points and leveled points)
            \return M2 (transformationmatrix)
            */
            int horizPnt(double xs, double ys, double zs, double& xsh, double& ysh, double& zsh) const;


            /// final calculated
            ZFTRMatrix matrix() const { return M2; }
            double roll() const { return Roll_out; } ///< [rad]
            double pitch() const { return Pitch_out; } ///< [rad]
            double yaw() const { return Yaw_out; } ///< [rad]
            zfs::math::Vec3d rpy() const { return { roll(), pitch(), yaw() }; } ///< [rad]
//            zfs::math::Vec3d trans() const { return { Tx_out, Ty_out, Tz_out }; }  ///< [meter]

        private:
            eErrorCode init__(const ZFSHeader* head, double dist, int mode, double max_tilt_changes );

        private:

            /// the result registration matrix
            ZFTRMatrix M2;
            double Roll_out{}, Pitch_out{}, Yaw_out{};
//            double Tx_out{}, Ty_out{}, Tz_out{};

            //
            double dist_mr{};
            double th_level{};

            int useLevel{-1};
            bool level_val[3]{};
            double level_roll_in[3]{}, level_pitch_in[3]{}, level_lenc_in[3]{};
            double N[3]{};


        };

    } // end namespace zfs
}