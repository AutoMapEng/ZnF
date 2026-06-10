#pragma once

#include <set>

#include <zfs/zfsapi.h>

namespace zfs {
    namespace math {

        template<class option_enum_t>  class multi_options
        {
        public:

            multi_options() {}

            multi_options(const std::set<option_enum_t>& allowable_options_list_,
                const std::set<option_enum_t>& mut_excl_options_) // should be a subset of allowable_options_list_
            {
                set(allowable_options_list_, mut_excl_options_);
            }

            void set(const std::set<option_enum_t>& allowable_options_list_, const std::set<option_enum_t>& mut_excl_options_)
            {
                m_allowable_options_list = allowable_options_list_;
                m_mut_excl_options = mut_excl_options_;
                m_choosen_options.clear();
            }

            bool option_set(const option_enum_t& option) const
            {
                return (m_choosen_options.count(option) != 0);
            }

            void set_options(const std::set<option_enum_t>& choosen_options)
            {
                if (m_choosen_options != choosen_options)
                {
                    m_choosen_options = choosen_options;
                    clear_mut_excl_options_in_choosen_options_list();
                }
            }

            void add_option(const option_enum_t option)
            {
                m_choosen_options.insert(option);
            }

            void clear_mut_excl_options_in_choosen_options_list() // .. the first one remains (prioritize?)
            {
                int cnt = 0; // clear input list from element if in mut_excl_options list
                typename std::set<option_enum_t>::iterator erase_iterator;
                for (auto it = m_mut_excl_options.begin(); it != m_mut_excl_options.end(); ++it)
                {
                    if ((erase_iterator = m_choosen_options.find(*it)) != m_choosen_options.end())
                        cnt++;

                    if (cnt == 2)
                    {
                        m_choosen_options.erase(erase_iterator);
                        cnt--;
                    }
                }
            }

            void replace_set(const std::set<option_enum_t>& old_options, const std::set<option_enum_t>& new_options)
            {
                typename std::set<option_enum_t>::iterator erase_iterator;

                for (auto it = old_options.begin(); it != old_options.end(); ++it)
                {
                    if ((erase_iterator = m_choosen_options.find(*it)) != m_choosen_options.end())
                        m_choosen_options.erase(erase_iterator);
                }

                m_choosen_options.insert(new_options.begin(), new_options.end());

            }

        private:

            std::set<option_enum_t> m_allowable_options_list, m_mut_excl_options;
            std::set<option_enum_t> m_choosen_options;

        };

    } // end namespace math
} // end namespace zfs