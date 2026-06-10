#pragma once

#include <zfsfilter/filterapi.h>

#include <map>
#include <string>
#include <vector>
#include <ctime>

#include <zfs/Math/math/average.h>

#include <zftools/zeitmessung.h>

namespace zfsfilter
{
    // ======================================================================================================================
    using namespace std;
    // ======================================================================================================================
    using vec_s = vector<string>;
    using map_avg = map<string, zfs::math::Average>;
    using map_timeavg_result = map<string, double>;
    using time_take_sec = zftools::TimeMeasurement;
    // ======================================================================================================================
    struct algorithm_step_time_measurement_t
    {
        enum class time_measure_t {SUM, AVERAGE};

        vec_s time_measurement_item_desc; 
        map_avg time_averages;
        map_timeavg_result average_times;

        algorithm_step_time_measurement_t(){}

        algorithm_step_time_measurement_t(const vec_s& time_measurement_item_desc_): time_measurement_item_desc(time_measurement_item_desc_) 
        {
            //for (auto cur_item= time_measurement_item_desc.begin(); cur_item != time_measurement_item_desc.end(); ++cur_item)
            //{
            //    average_times[*cur_item] = 0.;
            //}
        }

        void set(const vec_s& time_measurement_item_desc_)
        {
            time_measurement_item_desc = time_measurement_item_desc_;
        }

        void calc(time_measure_t time_measure_option= time_measure_t::AVERAGE)
        {

            // earlier code not totally safe, since not all items may be crossed ... compute only those that are actually in the map:

            for (auto it = time_averages.begin(); it != time_averages.end(); it++)
            {
                if (it->second.getCount() > 1)
                {
                    it->second.calc();

                    switch (time_measure_option)
                    {
                    case time_measure_t::AVERAGE:
                        {
                            average_times[it->first] = it->second.getAvr();
                            break;
                        }
                    case time_measure_t::SUM:
                        {
                            average_times[it->first] = it->second.getAvr()*it->second.getCount();
                            break;
                        }
                    }
                }
                else
                {
                    average_times[it->first] = it->second.getMin();
                }
            }

        }

        string time_measurement_list()
        {
            string output;
            output = "------ detail decomposition of processing step times ------\n";
            output += "list of average times needed for processing steps (per thread):";


            // earlier code not totally safe, since not all items may be crossed ... compute only those that are actually in the map (considering order of measurement):

            for (size_t n = 0; n < time_measurement_item_desc.size(); ++n)
            {
                auto cur_item = average_times.find(time_measurement_item_desc[n]);

                if (cur_item != average_times.end())
                {
                    output += "\nstep: " + cur_item->first + " : " + to_string(cur_item->second) + " seconds";
                }
            }

            output += "\n----------------------------------------------------------\n";
            return output;
        }

        const map_timeavg_result& get_average_times() const
        {
            return average_times;
        }

        const map_avg& get_time_averages() const
        {
            return time_averages;
        }
    };
    // ======================================================================================================================

}