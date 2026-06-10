#pragma once

#include <zfsfilter/filterapi.h>

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <zfs/Math/zfvec.h>
#include <zfs/Math/statistics/average_3d.h>
#include <zfs/Math/statistics/mean_3d.h>
#include <zfsfilter/tools/noise_parameter.h> // for setting noise data vectors

namespace zfsfilter
{
    namespace planes
    {
        // ======================================================================================================================

        using namespace zfs;
        using namespace zfs::math;

        // ======================================================================================================================

        using vec_b = std::vector<bool>;
        using vec = std::vector<double>;

        using vec_i = std::vector<int>;
        
        using vec_ui8 = std::vector<uint8_t>;
        using vec_ui32 = std::vector<uint32_t>;
        using vec_ui64 = std::vector<uint32_t>;

        using range_d = std::pair<double, double>;

        using mat_ui64 = std::vector<std::vector<uint32_t>>;

        // ======================================================================================================================
        class data_vector_container_base
        {
        public:

            data_vector_container_base()
                : vec_size(0),
                data_cnt(-1)
            {}

            data_vector_container_base(const int32_t& vec_size_)
                : vec_size(vec_size_),
                data_cnt(-1)
            {}

            void preinc() // use this pre - increment function to set data in the derived classes via  access functions
            {
                increment_data_cnt();
            }

            void preinc(const int32_t cnt) // use this pre - increment function to set data in the derived classes via  access functions
            {
                increment_data_cnt(cnt);
            }

            void increment_data_cnt()
            {
                //if (data_cnt < vec_size- 1) data_cnt++;
                data_cnt++;
            }

            void increment_data_cnt(const int32_t& cnt)
            {
                if (data_cnt < 0) 
                    data_cnt= 0;

                else if (data_cnt+ cnt < vec_size) 
                    data_cnt+= cnt;
            }

            void preset_data_cnt(const int32_t& cnt)
            {
                set_data_cnt(cnt);
            }

            void set_data_cnt(const int32_t& cnt)
            {
                data_cnt = cnt;
            }

            void reset_data_cnt() // for reuse of data container
            {
                data_cnt = -1;
            }

            int32_t vec_size;
            int32_t data_cnt;

        };
        // ======================================================================================================================
        class raw_data_vector_container: public data_vector_container_base
        {
            public:

                raw_data_vector_container()
                    : data_vector_container_base(),
                    rg_av_calculated(false),
                    rf_av_calculated(false)
                {}

                raw_data_vector_container(const int32_t vec_size_)
                    : data_vector_container_base(vec_size_),
                      maskv(vec_ui8(vec_size_,0)),
                      rfv(vec_i(vec_size_,0)),
                      rgv(vec(vec_size_,0)),
                      rg_av_calculated(false),
                      rf_av_calculated(false)
                {}

                void add(int32_t rf, const double& rg)
                {
                    increment_data_cnt();

                    rfv[this->data_cnt] = rf;
                    rgv[this->data_cnt] = rg;
                }

                void add(uint8_t mask, int32_t rf, const double& rg)
                {
                    increment_data_cnt();

                    maskv[this->data_cnt] = mask;
                    rfv[this->data_cnt] = rf;
                    rgv[this->data_cnt] = rg;
                }

                void set(const int32_t& idx, int32_t rf, const double& rg)
                {
                    this->data_cnt = idx;
                    rfv[this->data_cnt] = rf;
                    rgv[this->data_cnt] = rg;
                }

                void set(const int32_t& idx, uint8_t mask, int32_t rf, const double& rg)
                {
                    this->data_cnt = idx;
                    maskv[this->data_cnt] = mask;
                    rfv[this->data_cnt] = rf;
                    rgv[this->data_cnt] = rg;
                }

                const double& get_rf_mean() const
                {
                    return rf_av.getAvr();
                }

                const double& get_rg_mean() const
                {
                    return rg_av.getAvr();
                }

                uint8_t& mask()
                {
                    return maskv[this->data_cnt];
                }

                int32_t& rf()
                {
                    return rfv[this->data_cnt];
                }

                double& rg()
                {
                    return rgv[this->data_cnt];
                }

                const uint8_t& mask() const
                {
                    return maskv[this->data_cnt];
                }

                const int32_t& rf() const
                {
                    return rfv[this->data_cnt];
                }

                const double& rg() const
                {
                    return rgv[this->data_cnt];
                }

                const uint8_t& mask(int32_t cnt) const
                {
                    return maskv[cnt];
                }

                const int32_t& rf(int32_t cnt) const
                {
                    return rfv[cnt];
                }

                const double& rg(int32_t cnt) const
                {
                    return rgv[cnt];
                }

                bool calculate_rf_mean()
                {
                    rf_av.clear();

                    for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                    {
                        rf_av.add(rfv[elem]);
                    }

                    rf_av_calculated = rf_av.calc();

                    return rf_av_calculated;
                }

                bool calculate_rg_mean()
                {
                    rg_av.clear();

                    for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                    {
                        rg_av.add(rgv[elem]);
                    }

                    rg_av_calculated = rg_av.calc();

                    return rg_av_calculated;
                }

                vec_ui8 maskv;                     // < mask info
                vec_i rfv;                        // < intensity containing vectors
                vec rgv;                          // < range vectors

            private:

                bool rg_av_calculated;
                bool rf_av_calculated;

                zfs::math::mean rg_av; // rg average
                zfs::math::mean rf_av; // intensity average

        };
        // ======================================================================================================================
        class angle_data_vector_container : public data_vector_container_base
        {
        public:

            angle_data_vector_container() : data_vector_container_base(), aac(0), eac(0)
            {}

            angle_data_vector_container(const int32_t vec_size_)
                : data_vector_container_base(vec_size_),
                aac(0), eac(0), aav(vec(vec_size_,0)), eav(vec(vec_size_,0))
            {}

            void add(const double& aa, const double& ea)
            {
                increment_data_cnt();

                aav[this->data_cnt] = aa;
                eav[this->data_cnt] = ea;
            }

            void set(const int32_t& idx, const double& aa, const double& ea)
            {
                this->data_cnt = idx;
                aav[this->data_cnt] = aa;
                eav[this->data_cnt] = ea;
            }

            double& aa()
            {
                return aav[this->data_cnt];
            }

            double& ea()
            {
                return eav[this->data_cnt];
            }

            const double& aa() const
            {
                return aav[this->data_cnt];
            }

            const double& ea() const
            {
                return eav[this->data_cnt];
            }

            const double& aa(int32_t cnt) const
            {
                return aav[cnt];
            }

            const double& ea(int32_t cnt) const
            {
                return eav[cnt];
            }
#if 0
            void center_angles_to_mean()
            {
                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    aa_av.add(aav[elem]);
                    ea_av.add(eav[elem]);
                }
                if (aa_av.calc() && ea_av.calc())
                {
                    aac = aa_av.getAvr();
                    eac = ea_av.getAvr();
                    for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                    {
                        aav[elem] -= aac;
                        eav[elem] -= eac;
                    }
                }
            }
#endif
            void center_angles_to(const int32_t& center_count)
            {
                aac = aav[center_count];
                eac = eav[center_count];
                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    aav[elem] -= aac;
                    eav[elem] -= eac;
                }
            }

            void center_angles_to(const int32_t& center_count, range_d& range_aa, range_d& range_ea)
            {
                aac = aav[center_count];
                eac = eav[center_count];

                range_aa = range_d(0, 0);
                range_ea = range_d(0, 0);

                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    aav[elem] -= aac;
                    eav[elem] -= eac;

                    if (range_aa.first > aav[elem]) range_aa.first = aav[elem];
                    if (range_aa.second < aav[elem]) range_aa.second = aav[elem];

                    if (range_ea.first > eav[elem]) range_ea.first = eav[elem];
                    if (range_ea.second < eav[elem]) range_ea.second = eav[elem];
                }
            }
#if 0
            void center_angles_to_mean(range_d& range_aa, range_d& range_ea)
            {
                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    aa_av.add(aav[elem]);
                    ea_av.add(eav[elem]);
                }

                range_aa = range_d(0, 0);
                range_ea = range_d(0, 0);

                if (aa_av.calc() && ea_av.calc())
                {
                    aac = aa_av.getAvr();
                    eac = ea_av.getAvr();

                    for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                    {
                        aav[elem] -= aac;
                        eav[elem] -= eac;

                        if (range_aa.first > aav[elem]) range_aa.first = aav[elem];
                        if (range_aa.second < aav[elem]) range_aa.second = aav[elem];

                        if (range_ea.first > eav[elem]) range_ea.first = eav[elem];
                        if (range_ea.second < eav[elem]) range_ea.second = eav[elem];
                    }
                }
            }

            void reset_data_cnt()
            {
                data_vector_container_base::reset_data_cnt();
                aa_av.clear();
                ea_av.clear();
            }
#endif
            void center_angles_to(const int32_t& center_count, const vec_b& sample_indicator)
            {
                aac = aav[center_count];
                eac = eav[center_count];
                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    if (sample_indicator[elem])
                    {
                        aav[elem] -= aac;
                        eav[elem] -= eac;
                    }
                }
            }

            void reset()
            {
                aac = 0;
                eac = 0;
            }

            double aac, eac;
#if 0
            Average aa_av, ea_av;
#endif
            vec aav, eav;                     // < azimuth/ elevation angle vectors

        };
        // ======================================================================================================================
        class xyz_data_vector_container : public data_vector_container_base
        {
        public:

            xyz_data_vector_container() : data_vector_container_base(), xvc(0), yvc(0), zvc(0)
            {}

            xyz_data_vector_container(const int32_t vec_size_)
                : data_vector_container_base(vec_size_),
                  xv(vec(vec_size_,0)),
                  yv(vec(vec_size_,0)),
                  zv(vec(vec_size_,0)), 
                  xvc(0), yvc(0), zvc(0)
            {}

            void add(const double& x, const double& y, const double& z)
            {
                increment_data_cnt();

                xv[this->data_cnt] = x;
                yv[this->data_cnt] = y;
                zv[this->data_cnt] = z;
            }

            void add(const Vec3d& xyz)
            {
                increment_data_cnt();

                xv[this->data_cnt] = xyz.x();
                yv[this->data_cnt] = xyz.y();
                zv[this->data_cnt] = xyz.z();
            }

            void set(const int32_t& idx, const double& x, const double& y, const double& z)
            {
                this->data_cnt = idx;
                xv[this->data_cnt] = x;
                yv[this->data_cnt] = y;
                zv[this->data_cnt] = z;
            }

            void set(const int32_t& idx, const Vec3d& xyz)
            {
                this->data_cnt = idx;
                xv[this->data_cnt] = xyz.x();
                yv[this->data_cnt] = xyz.y();
                zv[this->data_cnt] = xyz.z();
            }

            double& x()
            {
                return xv[this->data_cnt];
            }

            double& y()
            {
                return yv[this->data_cnt];
            }

            double& z()
            {
                return zv[this->data_cnt];
            }

            const double& x() const
            {
                return xv[this->data_cnt];
            }

            const double& y() const
            {
                return yv[this->data_cnt];
            }

            const double& z() const
            {
                return zv[this->data_cnt];
            }

            const double& x(int32_t cnt) const
            {
                return xv[cnt];
            }

            const double& y(int32_t cnt) const
            {
                return yv[cnt];
            }

            const double& z(int32_t cnt) const
            {
                return zv[cnt];
            }

            void set_center_sample_vector(int32_t nc)
            {
                xvc = xv[nc]; yvc = yv[nc]; zvc = zv[nc];
            }

            bool calculate_coordinate_mean()
            {
                xyz_av.clear();

                for (int32_t elem = 0; elem <= this->data_cnt; elem++)
                {
                    xyz_av.add(xv[elem], yv[elem], zv[elem]);
                }

                return xyz_av.calc();
            }

            const vec& get_coordinate_mean() const
            {
                return xyz_av.getAvr();
            }

            void reset_data_cnt()
            {
                data_vector_container_base::reset_data_cnt();
            }

            mean_3D xyz_av; // averages for positions

            vec xv, yv, zv;     // < vectors for x,y and z

            double xvc, yvc, zvc; // < center sample vector components

        };
        // ======================================================================================================================
        class noise_data_vector_container : public data_vector_container_base
        {
        public:

            noise_data_vector_container() : data_vector_container_base()
            {}

            noise_data_vector_container(const int32_t vec_size_)
                : data_vector_container_base(vec_size_),
                sigma_xv(vec(vec_size_, 1.)),
                sigma_yv(vec(vec_size_, 1.)),
                sigma_zv(vec(vec_size_, 1.))
            {}

            void add(const double& sigma_x, const double& sigma_y, const double& sigma_z)
            {
                increment_data_cnt();

                sigma_xv[this->data_cnt] = sigma_x;
                sigma_yv[this->data_cnt] = sigma_y;
                sigma_zv[this->data_cnt] = sigma_z;
            }

            void set(int32_t idx, const double& sigma_x, const double& sigma_y, const double& sigma_z)
            {
                this->data_cnt = idx;
                sigma_xv[this->data_cnt] = sigma_x;
                sigma_yv[this->data_cnt] = sigma_y;
                sigma_zv[this->data_cnt] = sigma_z;
            }

            double& sigma_x()
            {
                return sigma_xv[this->data_cnt];
            }

            double& sigma_y()
            {
                return sigma_yv[this->data_cnt];
            }

            double& sigma_z()
            {
                return sigma_zv[this->data_cnt];
            }

            const double& sigma_x() const
            {
                return sigma_xv[this->data_cnt];
            }

            const double& sigma_y() const
            {
                return sigma_yv[this->data_cnt];
            }

            const double& sigma_z() const
            {
                return sigma_zv[this->data_cnt];
            }

            void set_vectors(const noise_parameter_t& noise_parameter, const raw_data_vector_container& raw_data_vectors, const xyz_data_vector_container& xyz_data_vectors, const vec_b& sample_indicator)
            {
                const uint32_t L = raw_data_vectors.data_cnt + 1;

                for (uint32_t n = 0; n < L; ++n)
                {
                    if (sample_indicator[n])
                    {
                        const double sigma_rg_2_rg_ratio = (noise_parameter.noise.calcM(raw_data_vectors.rf(n))+ noise_parameter.noise_offset()) / raw_data_vectors.rg(n);
#if defined(_NO_WEIGHTING)
                    // calculate values but don't use
                        check_noise_data_container.add(sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.x(n)),
                            sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.y(n)),
                            sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.z(n)));
                        // project sigma onto the directions of the local scanner look angle
                        // rg is larger than a minimum range set in calling function
                        cur_noise_data_vectors.add(1., 1., 1.);
#else
                        add(sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.x(n)),
                            sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.y(n)),
                            sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.z(n)));
#endif
                    }
                    else
                    {
                        increment_data_cnt();
                    }
                }
            }

            void set_vectors(const noise_parameter_t& noise_parameter, const raw_data_vector_container& raw_data_vectors, const xyz_data_vector_container& xyz_data_vectors)
            {
                const uint32_t L = raw_data_vectors.data_cnt + 1;

                for (uint32_t n = 0; n < L; ++n)
                {
                    const double sigma_rg_2_rg_ratio = (noise_parameter.noise.calcM(raw_data_vectors.rf(n)) + noise_parameter.noise_offset()) / raw_data_vectors.rg(n);
#if defined(_NO_WEIGHTING)
                    // calculate values but don't use
                    check_noise_data_container.add(sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.x(n)),
                        sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.y(n)),
                        sigma_rg_2_rg_ratio* abs(cur_xyz_data_vectors.z(n)));
                    // project sigma onto the directions of the local scanner look angle
                    // rg is larger than a minimum range set in calling function
                    cur_noise_data_vectors.add(1., 1., 1.);
#else
                    add(sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.x(n)),
                        sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.y(n)),
                        sigma_rg_2_rg_ratio* std::abs(xyz_data_vectors.z(n)));
#endif
                }
            }

            vec sigma_xv, sigma_yv, sigma_zv; // < vectors keeping the sigma values

        };

        // ======================================================================================================================

        class data_vector_container
        {

        public:

            data_vector_container() = delete;

            data_vector_container(int32_t vec_size)
                : total_cnt(0),
                  raw_data_vectors(raw_data_vector_container(vec_size)),
                  angle_data_vectors(angle_data_vector_container(vec_size)),
                  xyz_data_vectors(xyz_data_vector_container(vec_size)),
                  noise_data_vectors(noise_data_vector_container(vec_size))
                {}

            void reset_data_cnts()
            {
                total_cnt = 0;
                raw_data_vectors.reset_data_cnt();
                angle_data_vectors.reset_data_cnt();
                xyz_data_vectors.reset_data_cnt();
                noise_data_vectors.reset_data_cnt();
            }

            void reset()
            {
                reset_data_cnts();
                angle_data_vectors.reset();
            }

            void increment_data_cnts()
            {
                raw_data_vectors.increment_data_cnt();
                angle_data_vectors.increment_data_cnt();
                xyz_data_vectors.increment_data_cnt();
                noise_data_vectors.increment_data_cnt();

                increment_total_data_cnt();
            }

            void increment_total_data_cnt()
            {
                total_cnt++;
            }

            void increment_total_data_cnt(const int32_t& cnt)
            {
                total_cnt += cnt;
            }

            uint32_t                    total_cnt;

            raw_data_vector_container   raw_data_vectors;
            angle_data_vector_container angle_data_vectors;
            xyz_data_vector_container   xyz_data_vectors;
            noise_data_vector_container noise_data_vectors;

        };

        // ======================================================================================================================

        using plane_data_vector_line_t = std::vector<data_vector_container>;
        using plane_data_vector_matrix_t = std::vector<plane_data_vector_line_t>;

        // ======================================================================================================================
        // plane_data_vector_line - class
        // . for line based processing
        // ======================================================================================================================

        class plane_data_vector_line
        {

        public:

            plane_data_vector_line() = delete;

            plane_data_vector_line(int32_t pixels, int32_t vector_size)
                : m_pixels(pixels),
                m_common_vector_idxcnt(vec_ui64(pixels,0))
            {
                m_data.resize(m_pixels, data_vector_container(vector_size));
            }

            const data_vector_container& operator()(int32_t pixel) const
            {
                return m_data[pixel];
            }

            data_vector_container& operator()(int32_t pixel)
            {
                return m_data[pixel];
            }

            uint32_t pixels() const
            {
                return m_pixels;
            }

            void reset_data_cnts()
            {
                for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                {
                    m_data[pixel].reset_data_cnts();
                }
            }

            void reset()
            {
                for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                {
                    m_data[pixel].reset();
                }
            }

            uint32_t& get_counter(const int32_t& pixel)
            {
                return m_common_vector_idxcnt[pixel];
            }

            void increment_vector_idxcnt(const int32_t& pixel)
            {
                ++m_common_vector_idxcnt[pixel];
            }

            void increment_vector_idxcnt(const int32_t& pixel, const int32_t& cnt)
            {
                m_common_vector_idxcnt[pixel] += cnt;
            }

            void reset_idxcnts()
            {
                for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                    m_common_vector_idxcnt[pixel] = 0;
            }

        private:

            int32_t m_pixels;

            vec_ui64 m_common_vector_idxcnt;

            plane_data_vector_line_t m_data;
        };

        // ======================================================================================================================
        // plane_data_vector_matrix - class
        // . for matrix based processing
        // ======================================================================================================================

        class plane_data_vector_matrix
        {

        public:

            plane_data_vector_matrix() = delete;

            plane_data_vector_matrix(int32_t lines, int32_t pixels, int32_t vector_size)
                : m_lines(lines),
                m_pixels(pixels),
                m_common_vector_idxcnt(std::vector<vec_ui64>(lines,vec_ui64(pixels,0)))
            {
                m_data.resize( m_lines );
                for (int32_t k = 0; k < m_lines; ++k)
                {
                    m_data[k].resize(m_pixels, data_vector_container(vector_size));
                }
            }

            const data_vector_container& operator()(const int32_t& line, const int32_t& pixel) const
            {
                return m_data[line][pixel];
            }

            data_vector_container& operator()(const int32_t& line, const int32_t& pixel)
            {
                return m_data[line][pixel];
            }

            int32_t lines() const
            {
                return m_lines;
            }

            int32_t pixels() const
            {
                return m_pixels;
            }

            void reset_data_cnts()
            {
                for (int32_t line = 0; line < m_lines; ++line)
                    for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                        m_data[line][pixel].reset_data_cnts();
            }

            void reset()
            {
                for (int32_t line = 0; line < m_lines; ++line)
                    for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                        m_data[line][pixel].reset();
            }

            uint32_t& get_counter(const int32_t& line, const int32_t& pixel)
            {
                return m_common_vector_idxcnt[line][pixel];
            }

            void increment_vector_idxcnt(const int32_t& line, const int32_t& pixel)
            {
                ++m_common_vector_idxcnt[line][pixel];
            }

            void increment_vector_idxcnt(const int32_t& line, const int32_t& pixel, const uint32_t& cnt)
            {
                m_common_vector_idxcnt[line][pixel] += cnt;
            }

            void reset_idxcnts()
            {
                for (int32_t line = 0; line < m_lines; ++line)
                    for (int32_t pixel = 0; pixel < m_pixels; ++pixel)
                        m_common_vector_idxcnt[line][pixel] = 0;
            }

        private:

            int32_t m_lines;
            int32_t m_pixels;

            mat_ui64 m_common_vector_idxcnt;

            plane_data_vector_matrix_t m_data;
        };

        // ======================================================================================================================

    }
}
