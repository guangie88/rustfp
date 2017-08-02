#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section

    namespace details {
        template <class Comparer>
        class MinByOp {
        public:
            template <class Comparerx>
            MinByOp(Comparerx &&cmp) :
                cmp(std::forward<Comparerx>(cmp)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Option<typename Iterator::Item> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "min_by can only take rvalue ref object with Iterator traits");

                using Item = typename Iterator::Item;
                auto min_opt = Option<Item>(None);

                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    if (min_opt.is_some()) {
                        auto lhs = reverse_decay(std::move(min_opt).unwrap_unchecked());
                        auto rhs = reverse_decay(std::move(next_opt).unwrap_unchecked());

                        // Some will automatically perform special decay
                        if (cmp(lhs, rhs)) {
                            min_opt = Some(lhs);
                        } else {
                            min_opt = Some(rhs);
                        }
                    } else {
                        min_opt = std::move(next_opt);
                    }
                }

                return std::move(min_opt);
            }

        private:
            Comparer cmp;
        };

        class MinOp {
        public:
            template <class Iterator>
            auto operator()(Iterator &&it) && -> Option<typename Iterator::Item> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "min can only take rvalue ref object with Iterator traits");

                using Comparer = std::less_equal<typename Iterator::Item>;
                return MinByOp<Comparer>(Comparer())(std::forward<Iterator>(it));
            }
        };
    }

    inline auto min()
        -> details::MinOp {

        return details::MinOp();
    }

    template <class Comparer>
    auto min_by(Comparer &&cmp)
        -> details::MinByOp<special_decay_t<Comparer>> {

        return details::MinByOp<special_decay_t<Comparer>>(std::forward<Comparer>(cmp));
    }
}
