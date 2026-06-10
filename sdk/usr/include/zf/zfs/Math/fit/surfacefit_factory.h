#pragma once 

#include <typeindex>
#include <typeinfo>
#include <memory>
#include <map>
#include <functional>

#include "surfacefit.h"

namespace zfs { 
    namespace math {

        struct ZFS_API surfacefit_factory_base
        {
            virtual std::shared_ptr<surface_fit> Create(const int& order, const surface_fit::operation_t& operation, const surface_fit::type_t& type, const std::set<surface_fit::option_t>& options) = 0;
            virtual ~surfacefit_factory_base() {}
        };

        struct ZFS_API surfacefit_factory :public surfacefit_factory_base
        {
            virtual std::shared_ptr<surface_fit> Create(const int& order, const surface_fit::operation_t& operation, const surface_fit::type_t& type, const std::set<surface_fit::option_t>& options) override;

        private:

            static std::map<surface_fit::type_t, std::function<std::shared_ptr<surface_fit>()>> mapping;

        };
    } // end namespace math
} // end namespace zfs