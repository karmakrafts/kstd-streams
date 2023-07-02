/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include "kstd/option.hpp"
#include "kstd/utils.hpp"
#include "owning_iterator_streamable.hpp"
#include "stream_fwd.hpp"
#include "stream_utils.hpp"
#include <algorithm>
#include <vector>

namespace kstd::streams {
    template<typename S, typename C>//
    struct SortingStream final : public Stream<typename S::ValueType, S, SortingStream<S, C>> {
        using Self = SortingStream<S, C>;
        using ValueType = typename S::ValueType;

        private:
        OwningIteratorStreamable<std::vector<ValueType>> _sorted_streamable;

        public:
        constexpr SortingStream(S streamable, C&& comparator) noexcept :
                Stream<ValueType, S, Self>(utils::move(streamable)) {
            std::vector<ValueType> elements;
            collect_into<S, std::vector<ValueType>>(this->_streamable, elements);
            std::sort(elements.begin(), elements.end(), utils::forward<C>(comparator));
            _sorted_streamable = OwningIteratorStreamable(utils::move(elements));
        }

        [[nodiscard]] constexpr auto next() noexcept -> Option<ValueType> {
            return _sorted_streamable.next();
        }
    };
}// namespace kstd::streams