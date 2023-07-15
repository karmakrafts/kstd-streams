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

#include "basic_pipe.hpp"
#include "identity_pipe.hpp"
#include "pipe.hpp"
#include "reference_supplier.hpp"
#include "value_supplier.hpp"

namespace kstd::streams {
    template<typename C>
    [[nodiscard]] constexpr auto stream(C& container) noexcept
            -> IdentityPipe<ReferenceSupplier<typename C::iterator>,
                            BasicPipe<ReferenceSupplier<typename C::iterator>>> {
        using supplier = ReferenceSupplier<typename C::iterator>;
        return IdentityPipe<supplier, BasicPipe<supplier>>(supplier(container.begin(), container.end()));
    }

    template<typename C>
    [[nodiscard]] constexpr auto stream(const C& container) noexcept
            -> IdentityPipe<ReferenceSupplier<typename C::const_iterator, true>,
                            BasicPipe<ReferenceSupplier<typename C::const_iterator, true>>> {
        using supplier = ReferenceSupplier<typename C::const_iterator, true>;
        return IdentityPipe<supplier, BasicPipe<supplier>>(supplier(container.cbegin(), container.cend()));
    }
}// namespace kstd::streams