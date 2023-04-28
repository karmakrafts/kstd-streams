/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include <functional>
#include <type_traits>
#include <optional>
#include <vector>
#include <algorithm>
#include "stream_fwd.hpp"
#include "stream_utils.hpp"
#include "owning_iterator_streamable.hpp"

namespace kstd::streams {
    template<typename S, kstd::concepts::Function<bool(const typename S::value_type&, const typename S::value_type&)> C> //
    struct SortingStream final : public Stream<typename S::value_type, S, SortingStream<S, C>> {
        using self_type = SortingStream<S, C>;
        using value_type = typename S::value_type;

        private:

        OwningIteratorStreamable<std::vector<value_type>> _sorted_streamable;

        public:

        constexpr SortingStream(S streamable, C&& comparator) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)) {
            std::vector<value_type> elements;
            collect_into<S, std::vector<value_type>>(this->_streamable, elements);
            std::sort(elements.begin(), elements.end(), std::forward<C>(comparator));
            _sorted_streamable = OwningIteratorStreamable(std::move(elements));
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            return _sorted_streamable.next();
        }
    };
}