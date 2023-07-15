// Copyright 2023 Karma Krafts & associates
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @author Alexander Hinze
 * @since 14/07/2023
 */

#pragma once

#include <functional>
#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>
#include <utility>

#include "basic_pipe.hpp"
#include "pipe.hpp"

namespace kstd::streams {
    template<typename S, typename IMPL>
    struct IdentityPipe : public BasicPipe<S> {
        // clang-format off
        using supplier_type = S;
        using impl_type     = IMPL;
        using in_type       = typename supplier_type::in_type;
        using out_type      = typename supplier_type::out_type;
        using value_type    = out_type;
        using self          = IdentityPipe<supplier_type, impl_type>;
        // clang-format on

        protected:
        [[nodiscard]] constexpr auto get_self() noexcept -> impl_type& {
            return static_cast<impl_type&>(*this);
        }

        private:
        template<typename P>
        [[nodiscard]] constexpr auto make_filter_sleeve(P&& predicate) noexcept -> decltype(auto) {
            return [&predicate](auto& supplier) -> Option<out_type> {
                auto element = supplier.get_next();
                if(!element) {
                    return {};
                }
                while(!predicate(element)) {
                    element = supplier.get_next();
                }
                return element;
            };
        }

        template<typename M, typename R>
        [[nodiscard]] constexpr auto make_map_sleeve(M&& mapper) noexcept -> decltype(auto) {
            return [&mapper](auto& supplier) -> Option<R> {
                auto element = supplier.get_next();
                if(!element) {
                    return {};
                }
                return mapper(*element);
            };
        }

        public:
        KSTD_DEFAULT_MOVE_COPY(IdentityPipe, self, constexpr);

        explicit constexpr IdentityPipe(supplier_type supplier) noexcept :
                BasicPipe<supplier_type>(std::move(supplier)) {
        }

        ~IdentityPipe() noexcept = default;

        template<typename P>
        [[nodiscard]] constexpr auto filter(P&& predicate) noexcept
                -> Pipe<self, decltype(make_filter_sleeve<P>(std::forward<P>(predicate)))> {
            static_assert(std::is_convertible_v<P, std::function<bool(out_type&)>>, "Closure signature doesn't match");
            return Pipe(std::move(*this), make_filter_sleeve<P>(std::forward<P>(predicate)));
        }

        template<typename M>
        [[nodiscard]] constexpr auto map(M&& mapper) noexcept
                -> Pipe<self,
                        decltype(make_map_sleeve<M, std::invoke_result_t<M, out_type>>(std::forward<M>(mapper)))> {
            using return_type = std::invoke_result_t<M, out_type>;
            static_assert(std::is_convertible_v<M, std::function<return_type(out_type&)>>,
                          "Closure signature doesn't match");
            return Pipe(std::move(*this), make_map_sleeve<M, return_type>(std::forward<M>(mapper)));
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> Option<out_type> {
            return get_self().get_next();
        }
    };
}// namespace kstd::streams