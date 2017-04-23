#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Iterator, class Pred>
        class Filter
        {
        public:
            using Item = typename Iterator::Item;

            template <class Iteratorx, class Predx>
            Filter(Iteratorx &&it, Predx &&pred) :
                it(std::forward<Iteratorx>(it)),
                pred(std::forward<Predx>(pred))
            {
            }

            auto next() -> Option<Item>
            {
                auto next_opt = it.next();

                if (next_opt.is_none())
                {
                    return None;
                }

                return pred(next_opt.get_unchecked())
                    ? std::move(next_opt)
                    : next();
            }

        private:
            Iterator it;
            Pred pred;
        };

        template <class Pred>
        class FilterOp
        {
        public:
            template <class Predx>
            explicit FilterOp(Predx &&pred) :
                pred(std::forward<Predx>(pred))
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Filter<Iterator, Pred>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "filter can only take rvalue ref object with Iterator traits");
                return Filter<Iterator, Pred>(std::move(it), std::move(pred));
            }

        private:
            Pred pred;
        };
    }

    template <class Pred>
    auto filter(Pred &&pred) -> details::FilterOp<special_decay_t<Pred>>
    {
        return details::FilterOp<special_decay_t<Pred>>(std::forward<Pred>(pred));
    }
}
