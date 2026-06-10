#pragma once
#include "zfsapi.h"

class ZFSHeader;

namespace zfs
{
	/**
	\brief  class TOF for time of light correction depending on range and motorspeed
			angle will be connected
	*/
	class ZFS_API TOF
	{
	public:
		TOF();
		virtual ~TOF() {}

		enum mod_t {
			M_OFF,			///< off
			M_ACCURATE,		///< default mode
			M_2STEP,			///< calculates just 2 steps for correction

            M_DEFAULT = M_ACCURATE
		};
		bool init(const ZFSHeader& );

		bool isEnabled() const { return m_enabled; }
		void enable( bool e ) { if( m_speedMotor>0 ) m_enabled=e; }

		void setMode( mod_t m ) { m_mode=m; }
		mod_t getMode() const { return m_mode; }

		double operator()( double rg ) const; ///< calculates the angle offset [rad]

	protected:
        double m_speedMotor{};	///< motorspeed [rps]
		bool m_enabled{};			///< enable/disable (default:enabled)
		mod_t m_mode{};			///< mode of calculation if enabled
		int m_encVert{};			///< vertical encoder inc/360deg
        double m_vertFac{};
        double m_vertFac1{};
        double m_speedFac{};

	};

    //##################################################################################################################################################################################################################


    inline double TOF::operator()(double rg) const
    {
        if (m_enabled)
        {
            const double o = m_speedFac * rg ;

            if (m_mode == M_2STEP)
            {
                const int n = (int)(o * m_vertFac1);
                return n * m_vertFac;
            }

            return o;
        }
        return 0.;
    }

} // end namespace
