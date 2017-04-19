#pragma once

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Pred>
        class FilterOp
        {
        public:

        private:
        };

        template <class Iterator, class Pred>
        class Filter
        {
        public:
            using Item = typename Iterator::Item;

        private:
        };
    }

    template <class Pred>
    auto filter(Pred &&pred) -> details::FilterOp<std::remove_reference_t<std::remove_const_t<Pred>>>
    {
        return details::FilterOp<std::remove_reference_t<std::remove_const_t<Pred>>>(std::forward<Pred>(pred));
    }
}
