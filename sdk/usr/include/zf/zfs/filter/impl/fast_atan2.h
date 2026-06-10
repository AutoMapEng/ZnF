#pragma once
#include <zfcore/zfmath.h>
#include <cmath>


namespace zfs
{
#if 0
    inline double FastArcTan(double x) {
        return (M_PI/4) * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
    }


    inline double FastArcTan2(double y, double x) {
        if (x >= 0) { // -pi/2 .. pi/2
            if (y >= 0) { // 0 .. pi/2
                if (y < x) { // 0 .. pi/4
                    return FastArcTan(y / x);
                }
                else { // pi/4 .. pi/2
                    return (M_PI/2) - FastArcTan(x / y);
                }
            }
            else {
                if (-y < x) { // -pi/4 .. 0
                    return FastArcTan(y / x);
                }
                else { // -pi/2 .. -pi/4
                    return -(M_PI / 2) - FastArcTan(x / y);
                }
            }
        }
        else { // -pi..-pi/2, pi/2..pi
            if (y >= 0) { // pi/2 .. pi
                if (y < -x) { // pi*3/4 .. pi
                    return FastArcTan(y / x) + M_PI;
                }
                else { // pi/2 .. pi*3/4
                    return (M_PI / 2) - FastArcTan(x / y);
                }
                }
            else { // -pi .. -pi/2
                if (-y < -x) { // -pi .. -pi*3/4
                    return FastArcTan(y / x) - M_PI;
                }
                else { // -pi*3/4 .. -pi/2
                    return -(M_PI / 2) - FastArcTan(x / y);
                }
            }
            }
        }

#endif



	/**
	 * Fast atan2 calculation using a precalculated LUT.
	 */
	class FastAtan2
	{
		const float pi = static_cast<float>(M_PI);
		std::vector<float> LUT;
		const int N{ 1024 };
		const float r45{ 0.25f * pi };
		const float r90{ 0.5f * pi };
		const float mr90{ -0.5f * pi };
		const float r180{ pi };

		inline bool nsi(float si) const { return si < 0; }
		inline bool nco(float co) const { return co < 0; }

	public:

		FastAtan2() {};

		void initLUT()
		{
			if (LUT.empty())
			{
				LUT.resize(N);
				for (int i = 0; i < N; i++)
				{
					LUT[i] = atanf(static_cast<float>(i) / N);
				}
			}
		}

		float max_err_deg() const
		{
			return atanf(0.5f / N) / pi * 180.f;
		}

		inline float operator()(float si, float co) const
		{
			if (si == 0.f && co == 0.f) return 0.f;

			const float abs_si = fabs(si);
			const float abs_co = fabs(co);

			if (abs_si > abs_co)
			{
				const float q = abs_co / abs_si;
				const int idx = static_cast<int>(q * N + 0.5);
				const float ph = idx >= N ? r45 : LUT[idx];

				if (!nsi(si) && !nco(co))
					return r90 - ph;
				else if (!nsi(si) && nco(co))
					return r90 + ph;
				else if (nsi(si) && nco(co))
					return mr90 - ph;
				else if (nsi(si) && !nco(co))
					return mr90 + ph;
				return ph;
			}
			else // si < co
			{
				const float q = abs_si / abs_co;
				const int idx = static_cast<int>(q * N + 0.5);
				const float ph = idx >= N ? r45 : LUT[idx];

				if (!nsi(si) && nco(co))
					return r180 - ph;
				else if (nsi(si) && nco(co))
					return ph - r180;
				else if (nsi(si) && !nco(co))
					return -ph;
				return ph;
			}
		}

	};

	// global in filter_line.cpp
	extern FastAtan2 fastatan2;


} // end namespace
