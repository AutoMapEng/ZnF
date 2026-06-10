#pragma once

#include <zfs/zfsapi.h>

#include <list>
#include <algorithm>

namespace zfs {
    namespace math {
        // ==============================================================================================
        // class for managing states of an object based on an enum type
        // ==============================================================================================
        template<class state_enum_t> class base_state
        {
        public:

            base_state()
            {}

            base_state(const state_enum_t& state) : state_list({ state })
            {}

            void set_current_state(const state_enum_t& state)
            {
                state_list.push_back(state);
            }

            bool empty() const
            {
                return state_list.empty();
            }

            bool find_state(const state_enum_t& query_state) const
            {
                return (std::count(state_list.begin(), state_list.end(), query_state) != 0);
            }

            bool find_states(const std::list<state_enum_t> state_list_)
            {
                bool bFound = true;

                for (auto state_it = state_list_.begin(); bFound && state_it != state_list_.end(); ++state_it)
                {
                    bFound = bFound && (std::count(state_list.begin(), state_list.end(), *state_it) != 0);
                }

                return bFound;
            }

            bool check_current_state(const state_enum_t& query_state) const
            {
                return state_list.back() == query_state;
            }

            bool operator==(const state_enum_t& query_state) const
            {
                return check_current_state(query_state);
            }

            bool operator!=(const state_enum_t& query_state) const
            {
                return !check_current_state(query_state);
            }

            base_state& operator=(const base_state& state) = delete;

            void reset()
            {
                state_list.clear();
            }

            void reset(const state_enum_t& initial_state)
            {
                if (state_list.back() != initial_state)
                {
                    reset();
                    state_list.push_back(initial_state);
                }
            }

        protected:

            std::list<state_enum_t> state_list;


        };
        // ==============================================================================================
        template<class error_state_enum_t> class error_state : public base_state<error_state_enum_t>
        {
        public:

            error_state() : base_state<error_state_enum_t>()
            {}

            bool error_occured() const
            {
                return !this->state_list.empty();
            }

            bool no_error_occured() const
            {
                return this->state_list.empty();
            }
        };
        // ==============================================================================================
        template<class state_enum_t, class error_state_enum_t> class object_state : public base_state<state_enum_t>
        {
        public:

            object_state() = delete;

            object_state(const state_enum_t& initial_state) :base_state<state_enum_t>(initial_state) {}

            void set_error(const error_state_enum_t& error_state)
            {
                object_error_state.set_current_state(error_state);
            }

            bool error_occured() const
            {
                return object_error_state.error_occured();
            }

            bool no_error_occured() const
            {
                return object_error_state.no_error_occured();
            }

            bool check_current_state_no_err(const state_enum_t& query_state) const
            {
                return base_state<state_enum_t>::check_current_state(query_state) && !object_error_state.error_occured();
            }

            bool find_state_no_err(const state_enum_t& query_state) const
            {
                return base_state<state_enum_t>::find_state(query_state) && !object_error_state.error_occured();
            }

            bool check_err_state(const error_state_enum_t& error_query_state) const
            {
                return object_error_state == error_query_state;
            }

            void reset()
            {
                if (this->state_list.size() > 1)
                {
                    state_enum_t initial_state = this->state_list.front();
                    this->state_list.clear();
                    this->state_list.push_back(initial_state);
                }
                object_error_state.reset();
            }

        private:
            error_state<error_state_enum_t> object_error_state;
        };
        // ==============================================================================================
    } // end namespace math
} // end namespace zfs