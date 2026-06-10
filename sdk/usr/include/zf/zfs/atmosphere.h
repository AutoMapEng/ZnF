#pragma once
#include "zfsapi.h"

class ZFSHeader;

namespace zfs
{
	/**
	\brief  calculates offset which should be added to the range
	*/
	class ZFS_API AtmosphereCorrection
	{
	public:
        AtmosphereCorrection();
		virtual ~AtmosphereCorrection() {}
		
        /// the temperatur on which the scan was take, the user has to input the temperature while scanning
        /// This will enable the correction
        void setTemperature(double deg);
        void setTemperature(const ZFSHeader&);

        /// current tempeature set
        double temperature() const;

        /// check if enabled
        bool isEnabled() const;

        /// enabled atmosphere correction
        void enable(bool e);
		
        /// calculates offset which should be added to the range [meter]
		double operator()( double rg ) const ; 

        /// the temperature where the correction is zero. Typical 20.5
        static double referenceTemperature();

	protected:
        bool m_enabled{};
        double m_temperature{};   ///< outside temperature [deg]
        double m_fac{};
	};


    //##################################################################################################################################################################################################################


    inline double AtmosphereCorrection::operator()(double rg) const
    {
        if (m_enabled)
        {
            return rg * m_fac - rg;
        }
        return 0;
    }

} // end namespace

