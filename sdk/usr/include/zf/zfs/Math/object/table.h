#pragma once

#include <zfs/zfsapi.h>

#include <cstdint>
#include <map>
#include <string>
#include <list>
#include <set>
#include <vector>
#include <utility>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>

#include <zfcore/unicode.h>
#include <zfcore/filename.h>

namespace zfs {
    namespace math {
        // ======================================================================================================================
        using list_s = std::list<std::string>;
        using vec_s = std::vector<std::string>;
        using set_s = std::set<std::string>;
        // ======================================================================================================================
        // class data_table
        // . contains rows which are accessed via key_t (in the easiest case integer numbers)
        // . the columns are accessed or referenced by strings
        // . the table can be filtered according to a maximum or minium value in a specified column
        // . a sorted table extract can be generated which has then type sorted_table_data_extract_t
        // . individual rows or columns can be extracted from the table via public members
        // . sizes of the table can be enquired
        // . the used key-set and column-reference-std::string set can be enquired with functions
        //   get_key_set() and get_column_set(), respectively.
        // . the table or the sorted table can be written to disk in a text file
        // ======================================================================================================================
        template<typename key_t> class data_table
        {
        public:
            typedef std::set< key_t >                                    key_t_set;
            typedef std::vector< key_t >                                 key_t_vector;
            typedef std::map< std::string, double >                      data_row_t;
            typedef std::map< key_t, std::map< std::string, double > >  table_data_t;
            typedef std::map< key_t, double >                            data_column_t;
            typedef std::map< std::string, double >                     data_extract_row_t;
            typedef std::vector< data_extract_row_t >                    sorted_table_data_extract_t;

            enum class filter_t { min, max };

            data_table() {}

            void add(const key_t& key, const std::string& column, const double& value)
            {
                key_set.insert(key);
                column_set.insert(column);
                table_data[key][column] = value;
            }

            double get(const key_t& key, const std::string& column) const
            {
                if (table_data.find(key) != table_data.end())
                    if (table_data[key].find(key) != table_data[key].end())
                        return table_data[key][column];
            }

            data_table<key_t> filter_max(const std::string& column, const double& value_max)
            {
                key_t_vector erase_vector;

                data_table<key_t> filtered_table(*this);

                erase_vector.reserve(table_data.size());

                for (auto it = table_data.begin(); it != table_data.end(); ++it)
                {
                    if ((it->second)[column] > value_max)
                        erase_vector.push_back((*it).first);
                }

                for (auto erase_it = erase_vector.begin(); erase_it != erase_vector.end(); ++erase_it)
                {
                    filtered_table.table_data.erase(*erase_it);
                    filtered_table.key_set.erase(*erase_it);
                }

                return filtered_table;
            }

            data_table<key_t> filter_min(const std::string& column, const double& value_min)
            {
                key_t_vector erase_vector;

                data_table<key_t> filtered_table(*this);

                erase_vector.reserve(table_data.size());

                for (auto it = table_data.begin(); it != table_data.end(); ++it)
                {
                    if ((it->second)[column] < value_min)
                        erase_vector.push_back((*it).first);
                }

                for (typename key_t_vector::const_iterator erase_it = erase_vector.begin(); erase_it != erase_vector.end(); ++erase_it)
                {
                    filtered_table.table_data.erase(*erase_it);
                    filtered_table.key_set.erase(*erase_it);
                }

                return filtered_table;
            }

            const data_row_t& get_row(const key_t& key)
            {
                return table_data[key];
            }

            data_column_t get_column(const std::string& column)
            {
                data_column_t table_column;

                for (auto it = table_data.begin(); it != table_data.end(); ++it)
                {
                    table_column[it->first] = (it->second)[column];
                }

                return table_column;
            }

            const typename data_table<key_t>::key_t_set& get_key_set() const
            {
                return key_set;
            }

            const set_s& get_column_set() const
            {
                return column_set;
            }

            size_t rows() const
            {
                return key_set.size();
            }

            size_t columns() const
            {
                return column_set.size();
            }

            void clear()
            {
                table_data.clear();
                key_set.clear();
                column_set.clear();
            }

            bool write_table(const std::wstring& filename)
            {
                zf::FileName Path(filename);

                Path.MkDirWithoutName();

                std::ofstream outfile;

#ifdef _WINDOWS
                outfile.open(filename);
#else
                outfile.open(zf::utf16_to_utf8(filename));
#endif

                if (outfile.is_open())
                {

                    outfile << "idx;";

                    for (auto it = column_set.begin(); it != column_set.end(); ++it)
                    {
                        outfile << *it << ";";
                    }

                    outfile << "\n";

                    for (auto row_it = table_data.begin(); row_it != table_data.end(); ++row_it)
                    {
                        outfile << std::setprecision(std::numeric_limits<int>::digits10 + 1);

                        outfile << row_it->first << ";";

                        outfile << std::setprecision(std::numeric_limits<double>::digits10 + 1);

                        for (auto col_it = row_it->second.begin(); col_it != row_it->second.end(); ++col_it)
                        {
                            outfile << col_it->second << ";";
                        }

                        outfile << "\n";
                    }

                    outfile.close();

                    return true;
                }
                return false;

            }

            bool write_table_sorted(const std::wstring& filename, const std::string& column, const std::string& sort_direction = "descend")
            {
                zf::FileName Path(filename);

                Path.MkDirWithoutName();

                std::ofstream outfile;

#ifdef _WINDOWS
                outfile.open(filename);
#else
                outfile.open(zf::utf16_to_utf8(filename));
#endif

                if (outfile.is_open())
                {

                    outfile << "tbl_idx;";

                    for (auto it = column_set.begin(); it != column_set.end(); ++it)
                    {
                        outfile << *it << ";";
                    }

                    outfile << "\n";

                    key_t_vector sort_vector = sort(column, sort_direction);

                    for (auto key_it = sort_vector.begin(); key_it != sort_vector.end(); ++key_it)
                    {
                        outfile << std::setprecision(std::numeric_limits<int>::digits10 + 1);

                        outfile << *key_it << ";";

                        outfile << std::setprecision(std::numeric_limits<double>::digits10 + 1);

                        data_row_t current_row = get_row(*key_it);

                        for (auto col_it = current_row.begin(); col_it != current_row.end(); ++col_it)
                        {
                            outfile << col_it->second << ";";
                        }

                        outfile << "\n";
                    }

                    outfile.close();

                    return true;
                }
                return false;

            }

            key_t_vector sort(const std::string& column, const std::string& sort_direction = "descend")
            {
                data_column_t sort_column = get_column(column);
                key_t_vector sort_idx(key_set.begin(), key_set.end());

                if (sort_direction == "descend")
                    // sort indexes based on comparing values in v
                    std::sort(sort_idx.begin(), sort_idx.end(), [&sort_column](key_t k1, key_t k2) {return sort_column[k1] > sort_column[k2]; });
                else if (sort_direction == "ascend")
                    std::sort(sort_idx.begin(), sort_idx.end(), [&sort_column](key_t k1, key_t k2) {return sort_column[k1] < sort_column[k2]; });

                return sort_idx;
            }

            sorted_table_data_extract_t get_sorted_table_extract(const std::string& column, const std::string& sort_direction = "descend")
            {
                sorted_table_data_extract_t sorted_table_data_extract(key_set.size());

                key_t_vector sort_vector = sort(column, sort_direction);

                uint32_t sorted_table_data_extract_idx = 0;

                for (auto key_it = sort_vector.begin(); key_it != sort_vector.end(); ++key_it, ++sorted_table_data_extract_idx)
                    sorted_table_data_extract[sorted_table_data_extract_idx] = get_row(*key_it);

                return sorted_table_data_extract;
            }

        private:

            bool         check{ false };

            key_t_set     key_set;
            set_s         column_set;

            table_data_t table_data;
        };
        // ======================================================================================================================
    } // end namespace math
} // end namespace zfs