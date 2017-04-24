#pragma once

#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Pred>
        class AllOp
        {
        public:
            template <class Predx>
            AllOp(Predx &&pred) :
                pred(std::forward<Predx>(pred))
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> bool
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "all can only take rvalue ref object with Iterator traits");

                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none())
                    {
                        break;
                    }

                    if (!pred(next_opt.unwrap_unchecked()))
                    {
                        return false;
                    }
                }

                return true;
            }

        private:
            Pred pred;
        };
    }

    template <class Pred>
    auto all(Pred &&pred) -> details::AllOp<special_decay_t<Pred>>
    {
        return details::AllOp<special_decay_t<Pred>>(std::forward<Pred>(pred));
    }
}