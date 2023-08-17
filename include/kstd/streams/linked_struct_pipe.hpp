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
 * @since 17/08/2023
 */

#pragma once

#include <kstd/defaults.hpp>
#include <kstd/option.hpp>
#include <type_traits>

namespace kstd::streams {
    template<typename A, typename F>
    struct LinkedStructPipe final {
        using AddressType = A;
        using ValueType = std::remove_pointer_t<AddressType>;
        using Reference = ValueType&;
        using FunctorType = F;
        using Self = LinkedStructPipe<AddressType, FunctorType>;

        static_assert(std::is_invocable_r_v<AddressType, FunctorType, AddressType>,
                      "Invalid functor signature, should be (A) -> A");

        private:
        AddressType _current;
        FunctorType _functor;

        public:
        KSTD_DEFAULT_MOVE_COPY(LinkedStructPipe, Self, constexpr)

        constexpr LinkedStructPipe(AddressType address, FunctorType&& functor) noexcept :
                _current {address},
                _functor {std::forward<FunctorType>(functor)} {
        }

        ~LinkedStructPipe() noexcept = default;

        [[nodiscard]] constexpr auto get_next() noexcept -> Option<Reference> {
            if(_current == nullptr) {
                return {};
            }
            auto* result = _current;
            _current = _functor(_current);
            return *result;
        }
    };
}// namespace kstd::streams