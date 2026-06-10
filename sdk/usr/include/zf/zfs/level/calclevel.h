#pragma once
#include "../zfsapi.h"

#include "../trmatrix.h"
#include "../flags.h"

#include <string>
#include <vector>


class ZFScanBasic;
class ZFSHeader;
class ZFSLinebyline;
class ZFScan;

#include "level_io.h"
#include "level_errors.h"


//
// old implementation
//


namespace zfs
{
    namespace level
    {
        /** \class levelParams
        * \brief dynamic useage of levelvalues
        * \author Susanne Lipkowski
        *
        *
        * >>> class Compensator uses this calss levelParams <<<
        *
        *
        *		Read level values, which are measured during scanning, from header.
        *		This raedings are sophisticated by centrifugal force and have to be
        *		corrected. The correction parameters are constant for each scanner
        *		setting (resolution and quality).
        *
        * DEUTSCH:
        *		1)	Einlesen der Libellenrohwerte aus dem Scanheader.
        *			x_raw, y_raw, azim
        *
        *		2)	Filterung der Libellenrohwerte.
        *			x_raw, y_raw => x_corr_filtered, y_corr_filtered
        *
        *		3)	Korrektur der gefilterten Libellenrohwerte um dynamische Effekte.
        *			x_corr_filtered, y_corr_filtered => xlevelcorr, ylevelcorr
        *
        *		4)	Berechnung der Korrekturwerte
        *
        *			a.	Ist der horizontale Scanbereich > 50°:
        *				Berechnung der Scannerschrägstellung (n_x, n_y) und der Libellenoffsets (s_x, s_y)
        *				unter Verwendung aller gefilterten Libellenrohwerte in einem Ausgeichungsansatz
        *				=> Residuen (x_corr_wob , y_corr_wob) = Bewegung des Scanners während des Scans,
        *				z.B. Stativeinsinken, Taumelbewegung), im lokalen Scannersystem
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *			b.	Ist der horizontale Scanbereich< 50° & Parent-Scans vorhanden:
        *				Berechnung der Libellenoffsets (s_x, s_y) unter Verwendung des Parent-Scans
        *				(wenn dieser einen Scanbereich > 50° besitzt), mit dem Parent-Scan werden die Schritte
        *				1-4a durchlaufen
        *				Die berechneten Libellenoffsets werden für den Child-Scan übernommen und im folgenden
        *				Ausgleichungsansatz nicht erneut modelliert, im Ansatz wird nur die Schrägstellung des
        *				Scanners ermittelt => Residuen (x_corr_wob , y_corr_wob) = Bewegung des Scanners während
        *				des Scans, z.B. Stativeinsinken, Taumelbewegung), im lokalen Scannersystem
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *			c.	Ist der horizontale Scanbereich< 50° & kein Parent-Scans vorhanden:
        *				Berechnung der Libellenoffsets (s_x, s_y) unter Verwendung der alten Methode, über
        *				3 Levelpositionen, die nach dem Scan statisch bestimmt werden.
        *				Die berechneten Libellenoffsets werden übernommen und im folgenden Ausgleichungsansatz
        *				nicht erneut modelliert, im Ansatz wird nur die Schrägstellung des Scanners ermittelt
        *				=> Residuen (x_corr_wob , y_corr_wob) = Bewegung des Scanners während des Scans,
        *				z.B. Stativeinsinken, Taumelbewegung), im lokalen Scannersystem
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *		5)	Transformation der Residuen vom lokalen Scannersystem ins globale Scannersystem.
        *			x_corr_wob , y_corr_wob  => xlevelcorr_global, ylevelcorr_global
        *
        *
        * ENGLISH:
        *		1)	Read level raw data from scan header.
        *			x_raw, y_raw, azim
        *
        *		2)	Filter level raw data.
        *			x_raw, y_raw => x_corr_filtered, y_corr_filtered
        *
        *		3)	Filtered level raw values are corrected for dynamics effects.
        *			x_corr_filtered, y_corr_filtered => xlevelcorr, ylevelcorr
        *
        *		4)	Calculation of scanner wobbling:
        *
        *			a.	If horizontal scan field > 50deg:
        *				Calculation of scanner tilting (n_x, n_y) and level offsets (s_x, s_y)
        *				based on Least Squares Estimation by using all filtered level raw values
        *				=> Residues (x_corr_wob , y_corr_wob) = show scanner movement during scan,
        *				such as tripod sinking, scanner wobbling), local scanner system
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *			b.	If horizontal scan field < 50deg & Parent-Scan exist:
        *				Calculation of level offsets (s_x, s_y) based on Parent-Scan level values
        *				(if horizontal scan field of parent-scan > 50°), steps 1-4a are made with parent-scan as well
        *				Calculated level offsets are taken over for child-scan, these parameters are not modelled inside
        *				Least Squares Estimation, just scanner tilting is modelled
        *				=> Residuen (x_corr_wob , y_corr_wob) = show scanner movement during scan,
        *				such as tripod sinking, scanner wobbling), local scanner system
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *			c.	If horizontal scan field < 50deg & no Parent-Scan exist:
        *				Calculation of level offsets (s_x, s_y) based on old methode, by using
        *				3 Levelpositions, which are taken statically after scanning .
        *				Calculated level offsets are taken, these parameters are not modelled inside
        *				Least Squares Estimation, just scanner tilting is modelled
        *				=> Residuen (x_corr_wob , y_corr_wob) = show scanner movement during scan,
        *				such as tripod sinking, scanner wobbling), local scanner system
        *				xlevelcorr, ylevelcorr => x_corr_wob , y_corr_wob, n_x, n_y, s_x, s_y
        *
        *		5)	Residues are transformed from local scanner system into global scanner system.
        *			x_corr_wob , y_corr_wob  => xlevelcorr_global, ylevelcorr_global
        */
#pragma warning(push)
#pragma warning(disable: 4251 )// DLL Schnittstelle
        class ZFS_API levelParams
        {
            std::string m_hardware; // The hardware used. Maybe empty for old hardware

        public:
            std::vector<int> dynamic_error;			// Lines between tilting changes are too big (>0,007grad), are saved

            LevelRaw m_raw;
            //        std::vector<float> azim;				//[rad]
            //        std::vector<float> x_raw;				//[rad]		raw-xlevel value
            //        std::vector<float> y_raw;				//[rad]		raw-ylevel value

            std::vector<float> x_corr_filtered;	//[rad]		filtered-xlevel values
            std::vector<float> y_corr_filtered;	//[rad]		filtered-ylevel values

    //        std::vector<float> x_corr_offset;		//[rad]
    //        std::vector<float> y_corr_offset;		//[rad]

            std::vector<long> xlevelcorr;			//[µrad]	filtered-xlevel values without dynamic effects (local scanner system)
            std::vector<long> ylevelcorr;			//[µrad]	filtered-ylevel values without dynamic effects (local scanner system)

            std::vector<long> x_corr_wob;			//[µrad]	wobbling error	=> xLevel values without level offset and tilt (local scanner system)
            std::vector<long> y_corr_wob;			//[µrad]					=> yLevel values without level offset and tilt (local scanner system)

    //        std::vector<long> x_corr_tilt;			// scanner	tilting	=> NOT USED YET
    //        std::vector<long> y_corr_tilt;			//					=> NOT USED YET

    //        std::vector<long> x_corr_res;			// residual			=> NOT USED YET
    //        std::vector<long> y_corr_res;			//					=> NOT USED YET

            // results:
            std::vector<long> xlevelcorr_global;	//[µrad]	wobbling error	=> xLevel values without level offset and tilt (global scanner system)
            std::vector<long> ylevelcorr_global;	//[µrad]	wobbling error	=> yLevel values without level offset and tilt (global scanner system)

            // idea=> corrected level values (x-level and y-level) are splitted up into
            // the following values:	=> based on tilted scanner
            //							=> based on wobble error (Taumelfehler) and tripod movement
            //							=> based on vibrations
            double X0_x[3]{};			// x-level correction parameter to eliminate impacte of centrifugal force
            double X0_y[3]{};			// y-level correction parameter to eliminate impacte of centrifugal force
            double scantime_sec{};		// scanning time => depending on scan settings
            double offset_x{};		// x spirit level offset (zero point)
            double offset_y{};		// y spirit level offset (zero point)
            double Roll_out{};		// tilt angle nx
            double Pitch_out{};		// tilt angle ny
            double Yaw_out{};
            int column{};				// lines
    //        int levelaxis{};			// 0=>x-axis, 1=>y-axis
            bool calc_flag{ true };			// choose way to calculate level correction => "bool calcLevelCorr(std::vector<double> x, std::vector<double> y, std::vector<double> lenc);	"
            bool is_a_selection_flag{};			// is true, if scan is a selection scan (hori_field < 50deg) and if level offsets (sx, sy) are known from parent scan or 3pointMethode => so just tilt values (nx, ny) are calculated
            int th_level_mode{};			// 0=>check working range part (di.th_level_crange); 1=>check whole working range (di.th_level_mrange) ==>reset()
            /// the result registration matrix
            ZFTRMatrix M2;
            double Cxx[4][4]{};		// calculated in "calcLevelM2", Covariance Matrix (ny,nx,sy,sx) ==> n_Neigung,s_Spielpunkt der Libelle (Offset)
            int filter_d{};			// size of filter kernel
            int max_num_fastChanges{ 3 };
            double max_tilt_changes{};	// [deg] tilting changes between tow Lenc-Positions must be smaller than anglemeasurement accuracy
            double frequency_DC{};			// DC Motor Speed Scanheaderparameter 70

            double dist_mr{};			// distance between mirrorcenter and level-rotations center [mm]
            double th_level{};
            double res_level{};		// resolution of level
            double lencZ_field_rad{};		// scanned lenc_field
    //        std::wstring Path;
    //        std::wstring Path_debug;
            ZFSHeader* h{};
            std::wstring scan_parent; ///< abs path to parent scan if scan is a child
            int scan_level_mode{};
            std::string info;						///< some internal information
    //        bool logging{};

            typedef float level_value_t;   ///< libelle [deg]
            typedef std::vector<level_value_t> level_vector_t;

            typedef long level_value_long_t;   ///< libelle [deg]
            typedef std::vector<level_value_long_t> level_vector_long_t;

            typedef float pos_value_t; ///< position [deg]
            typedef std::vector<pos_value_t> pos_vector_t;


            levelParams() {}
            levelParams(const std::string& hardware) : m_hardware(hardware) {}
            levelParams(const ZFSHeader*);

            /**
            *  	levelParams::initLevelCorr(ZFScanBasic *z, bool flag)
            *   \brief	read scan settings to choose right correction parameters (X0_x and X0_y),
            *			read level values from lineheader into vector [rad]
            *			calculates corrected level values (spirit level offset and centrifugal effects are eliminated)
            *	\author Susanne Lipkowski
            *	\param ZFScanBasic *z
            *   \param bool flag ... how to eliminate centrifugal effects, default = false! (do not change)
            *						(flag = true and if scan > 180°, effects are calculated by fitting a sinus function  )
            *						(flag = false ,effects are eliminated by useing correction parameters, which are calculated based on preinvestigation data)
            *	\return bool
            *	\Info:		If horizontal scan field is smaller than 50deg => level offsets are calculated based on parent scan or
            *				based on three point level values (taken after scanning)
            */
            eErrorCode initLevelCorr(ZFScan* z, bool flag = false);	//


            /**
            *  	levelParams::initLevelCorr(ZFSLinebyline *z, bool flag)
            *   \brief	read scan settings to choose right correction parameters (X0_x and X0_y),
            *			read level values from lineheader into vector [rad]
            *			calculates corrected level values (spirit level offset and centrifugal effects are eliminated)
            *	\author Susanne Lipkowski
            *	\param ZFSLinebyline *z
            *   \param bool flag ... how to eliminate centrifugal effects, default = false! (do not change)
            *						(flag = true and if scan > 180°, effects are calculated by fitting a sinus function  )
            *						(flag = false ,effects are eliminated by useing correction parameters, which are calculated based on preinvestigation data)
            *	\return bool
            *	\Info:		If horizontal scan field is smaller than 50deg => level offsets are calculated based on parent scan or
            *				based on three point level values (taken after scanning)
            */
            eErrorCode initLevelCorr(ZFSLinebyline* z, bool flag = false);	//
#if 0
            eErrorCode initLevelCorr_TEST(ZFSLinebyline* z, bool flag = false);
#endif





            /**
            *  	levelParams::get3PointLevel()
            *   \brief	use 3 point level values (taken after scanning) to calculate
            *			tilting and level offsets
            *	\author Susanne Lipkowski
            *	\return eErrorCode
            */
            eErrorCode get3PointLevel();



            /**
            *  	levelParams::levelParams::initX0(ZFSHeader* h, int lines)
            *	\brief	get parameters (X0_x, X0_y, scantime) to correct dynamic measured level values, scan settings are taken from (h)
            *			QUALITY_SUPERHIGH:  X0_x, X0_y = 0. values for this setting
            *			RES_UH and higher:	X0_x, X0_y = 0. values for this setting ==> no dynamic correktion necessary
            *	\author Susanne Lipkowski
            *	\param ZFSHeader* h	... scan header
            *	\param int lines	... number of lines
            *	\return bool
            */
            bool initX_via_zfsheader(const ZFSHeader* h, int lines);	// choose the right correction parameter X0 depending on Scan resolution and quality
            bool initX_via_resolution(zfs::resolution_t r, zfs::quality_t);	// choose the right correction parameter X0 depending on Scan resolution and quality

            /// set the calibrated range threshold
            void setLevelCalibRange(const ZFSHeader* h);


            //*****************************************************
            /**
            *  	levelParams::initLevelCorr(ZFScan * zfs)
            *   \brief	read scan settings to choose right correction parameters (X0_x and X0_y),
            *			read level values from lineheader into vector [rad]
            *			calculate corrected level values (spirit level offset and centrifugal effects are eliminated)
            *	\author Susanne Lipkowski
            *	\param ZFScan * zfs
            *	\return bool
            */
            //        bool initLevelCorr(ZFScan* z, bool flag);


                    /**
                    *  	levelParams::initLevelCorr(ZFScan * zfs)
                    *   \brief	read scan settings to choose right correction parameters (X0_x and X0_y),
                    *			read level values from lineheader into vector [rad]
                    *			calculate corrected level values (spirit level offset and centrifugal effects are eliminated)
                    *	\author Susanne Lipkowski
                    *	\param ZFScan * zfs
                    *	\return bool
                    */
                    //        bool initLevelCorr(const ZFSHeader* h, int);

#if 0
        /**
        *  	levelParams::readLevel(ZFScan* z )
        *   \brief	read raw level values and lenc
        *			==> values are saved into x_raw, y_raw, azim [rad]
        *	\author Susanne Lipkowski
        *	\param	ZFScan* z ... scan
        *	\return bool
        */
            bool readLevel(ZFScan* z);


            /**
            *  	levelParams::readLevel(ZFSLinebyline* z)
            *   \brief	read raw level values and lenc
            *			==> values are saved into x_raw, y_raw, azim [rad]
            *	\author Susanne Lipkowski
            *	\param	ZFSLinebyline* z ... scan as Linebyline
            *	\return bool
            */
            bool readLevel(ZFSLinebyline* z);
#endif

            /**
            *  	levelParams::getXlevel_corr(int line)
            *   \brief	gives xlevel[rad] value at position (line), this value is free of dynamic effects and spirit level offset (zero point)
            *	\author Susanne Lipkowski
            *	\param	int line	... number of line (1:end)
            *	\return xlevel ... correction value for lenc with xlevel==> (lenc_corr = lenc - xlevel*cot(henc_corr) ) [mrad],
            *						false == 1000.
            */
            //        double getXlevel_corr(int line) const;


            /**
            *  	levelParams::getYlevel_corr(int line)
            *   \brief	gives ylevel[rad] value at position (line), this value is free of dynamic effects and spirit level offset (zero point)
            *	\author Susanne Lipkowski
            *	\param	int line	... number of line (1:end)
            *	\return ylevel ... correction value for henc with ylevel==> (henc_corr = henc+ylevel) [mrad],
            *					   false == 1000.
            */
            //        double getYlevel_corr(int line) const;


            /**
            *  	levelParams::writeLevel(...)
            *   \brief	save std:vector<...> to file
            *	\author Susanne Lipkowski
            *	\return bool
            */
            /*
            bool writeLevel(level_vector_long_t& level_x, level_vector_long_t& level_y, std::string filename) const;
            bool writeLevel(const level_vector_t& level_x, const level_vector_t& level_y, std::string filename) const;
            bool writeLevel(const pos_vector_t& level_x, std::string filename) const;
            */
            //*****************************************************


            /**
            *  	levelParams::filterLevel(std::vector<double> level, std::vector<double> &level_filtered)
            *   \brief calculates filtered level values by using a lowpass filter (weighted mean value)
            *	\author Susanne Lipkowski
            *	\param std::vector<double> level ... raw level values [rad]
            *	\param std::vector<double> &level_filtered ... filtered raw level values [rad]
            *	\return bool
            */
            static bool filterLevel(const level_vector_t&, level_vector_t&, int filter_size = 11, double frequency_DC = 25., const std::string& hardware = {});

        protected:



            /**
            *  	levelParams::getParentLevel()
            *   \brief	read level values of parent scan and calculate tilting, level offsets
            *			from parent scan level values
            *	\author Susanne Lipkowski
            *	\return eErrorCode
            */
            eErrorCode getParentLevel();


            /**
            *  	levelParams::nitLevel(ZFSHeader* h)
            *   \brief	read scan settings to choose right correction parameters (X0_x and X0_y),
            *			read level values from lineheader into vector [rad]
            *			calculates corrected level values (spirit level offset and centrifugal effects are eliminated)
            *	\author Susanne Lipkowski
            *	\param	ZFSHeader* h
            *	\return eErrorCode
            */
            eErrorCode basic_init_via_zfsheader(const ZFSHeader* h);//


            static std::vector<double> build_filter_params(int& filter_size, double frequency_DC);

            /**
            *  	levelParams::calcLevelCorr()
            *	\author Susanne Lipkowski
            *	\return eErrorCode
            */
            eErrorCode calcLevelCorr();			//


            /**
            *  	levelParams::calcLevelCorr(std::vector<double> x,  std::vector<double> y, std::vector<double> lenc)
            *   \brief calculates dynamic corrected level values, these values are transformed into global system => xlevelcorr_global, ylevelcorr_global
            *	\author Susanne Lipkowski
            *	\param std::vector<double> x = xlevel
            *	\param std::vector<double> y = ylevel
            *	\param std::vector<double> angs = horizontal angles
            *	\return eErrorCode
            */
            eErrorCode calcLevelCorr__(const level_vector_t& x, const level_vector_t& y, const pos_vector_t& lenc);			// correcte levelreadings relating to centrifugal force effects

            //-------------------------------------------------------------------------
            /**
            *  	levelParams::calcLevelM2(std::vector<double> levelx, std::vector<double> levely, std::vector<double> angs, std::vector<double> &level_corr, double dist)
            *   \brief calculate:	-spirit level offset (for xlevel and ylevel=> sx,sy)
            *						-tilt angle for x axis and y axis (nx,ny)
            *						-M2 matrix (to transform coordinates into horizontal system)
            *						-Cxx matrix (covariance matrix)
            *         used model:	ylevel =  ny*cos(angs) + nx*sin(angs) - sy;
            *						xlevel = -ny*sin(angs) + nx*cos(angs) - sx;
            *	\author Susanne Lipkowski
            *	\param const level_vector_t& levelx ... xlevel values [µrad]
            *	\param const level_vector_t& levely ... ylevel values [µrad]
            *	\param const pos_vector_t& angs  ... lenc [rad]
            *	\return eErrorCode
            */
            //        eErrorCode calcLevelM2(const level_vector_long_t&, const level_vector_long_t&, const pos_vector_t&);


                    /**
                    *  	levelParams::calcLevelM2(std::vector<double> levelx, std::vector<double> levely, std::vector<double> angs, std::vector<double> &level_corr, double dist)
                    *   \brief calculate:	-spirit level offset (for xlevel and ylevel=> sx,sy)
                    *						-tilt angle for x axis and y axis (nx,ny)
                    *						-M2 matrix (to transform coordinates into horizontal system)
                    *						-Cxx matrix (covariance matrix)
                    *         used model:	ylevel =  ny*cos(angs) + nx*sin(angs) - sy;
                    *						xlevel = -ny*sin(angs) + nx*cos(angs) - sx;
                    *	\author Susanne Lipkowski
                    *	\param const level_vector_t& levelx ... xlevel values [µrad]
                    *	\param const level_vector_t& levely ... ylevel values [µrad]
                    *	\param const level_vector_t& levelx_raw ... xlevel_raw values [µrad]
                    *	\param const level_vector_t& levely_raw ... ylevel_raw values [µrad]
                    *	\param const pos_vector_t& angs  ... lenc [rad]
                    *	\return eErrorCode
                    */
            eErrorCode calcLevelM2(level_vector_long_t&, level_vector_long_t&, level_vector_long_t&, level_vector_long_t&, const pos_vector_t&);

            /**
            *  	levelParams::calcLevelM2_(std::vector<double> levelx, std::vector<double> levely, std::vector<double> angs, std::vector<double> &level_corr, double dist)
            *   \brief calculates:	-tilt angle for x axis and y axis (nx,ny)
            *						-M2 matrix (to transform coordinates into horizontal system)
            *						-Cxx matrix (covariance matrix)
            *                        ==> level offsets (sy, sx) are known
            *         used model:	ylevel =  ny*cos(angs) + nx*sin(angs);
            *						xlevel = -ny*sin(angs) + nx*cos(angs);
            *	\author Susanne Lipkowski
            *	\param const level_vector_t& levelx ... xlevel values [µrad]
            *	\param const level_vector_t& levely ... ylevel values [µrad]
            *	\param const pos_vector_t& angs  ... lenc [rad]
            *	\return eErrorCode
            */
            eErrorCode calcLevelM2_(const level_vector_long_t&, const level_vector_long_t&, const pos_vector_t&); // if level offset values are known


#if 0
            /**
            *  	levelParams::calcLevel_3Positions(std::vector<double> levelx, std::vector<double> levely, std::vector<double> angs, std::vector<double> &XX)
            *   \brief calculate:	-spirit level offset (for xlevel and ylevel=> sx,sy)
            *						-tilt angle for x axis and y axis (nx,ny)
            *						-M2 matrix (to transform coordinates into horizontal system)
            *						-Cxx matrix (covariance matrix)
            *         used model:	ylevel =  ny*cos(angs) + nx*sin(angs) - sy;
            *						xlevel = -ny*sin(angs) + nx*cos(angs) - sx;
            *	\author Susanne Lipkowski
            *	\param std::vector<double> levelx ... xlevel values [rad]
            *	\param std::vector<double> levely ... ylevel values [rad]
            *	\param std::vector<double> angs  ... lenc [rad]
            *   \param std::vector<double> XX ... [ny nx sy sx]  [rad]
            *	\return eErrorCode
            */
            eErrorCode calcLevel_3Positions(const level_vector_t&, const level_vector_t&, const level_vector_t&, level_vector_t&);
#endif


#if 0
            /**
            *  	levelParams::calcLevelOffset(std::vector<double> level, std::vector<double> angs, std::vector<double> &level_corr, double offset)
            *   \brief calculate spirit level offset, by fitting (a*sin(ang+b)+c)
            *				==> tilted scanner is modeled by:	   (a*sin(ang+b)
            *				==> spirit level offset is modeled by: c
            *	\author Susanne Lipkowski
            *	\param std::vector<double> level ... level values [rad]
            *	\param std::vector<double> angs  ... lenc [rad]
            *	\param std::vector<double> &level_corr ... level values corrected by spirit level offset [rad]
            *	\param double offset ... calculated spirit level offset [rad]
            *	\return double ... offset
            */
            double calcLevelOffset(const level_vector_t&, const pos_vector_t&, level_vector_t&);
#endif

            /**
            * Checl Levelvalues
            * Limits
            */
            eErrorCode checkLevelMinMax();

        };
#pragma warning(pop)
    }
} // end namespace zfs
