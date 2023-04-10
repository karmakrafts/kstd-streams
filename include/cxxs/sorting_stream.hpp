/**
 * @author Alexander Hinze
 * @since 10/04/2023
 */

#pragma once

#include <functional>
#include <type_traits>
#include <optional>
#include <vector>
#include "stream_fwd.hpp"
#include "stream_utils.hpp"
#include "owning_iterator_streamable.hpp"

namespace cxxs {
    template<typename S, typename C> requires(std::is_convertible_v<C, std::function<bool(const typename S::value_type&, const typename S::value_type&)>>)
    struct SortingStream final : public Stream<typename S::value_type, S, SortingStream<S, C>> {
        using self_type = SortingStream<S, C>;
        using value_type = typename S::value_type;

        private:

        OwningIteratorStreamable<value_type, std::vector> _sorted_streamable;

        public:

        constexpr SortingStream(S streamable, C&& comparator) noexcept:
                Stream<value_type, S, self_type>(std::move(streamable)) {
            std::vector<value_type> elements;
            collect_streamable(this->_streamable, elements);
            std::sort(elements.begin(), elements.end(), std::forward<C>(comparator));
            _sorted_streamable = OwningIteratorStreamable(std::move(elements));
        }

        [[nodiscard]] constexpr auto next() noexcept -> std::optional<value_type> {
            return _sorted_streamable.next();
        }
    };
}