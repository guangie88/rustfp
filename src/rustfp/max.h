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
        class MaxByOp {
        public:
            template <class Comparerx>
            MaxByOp(Comparerx &&cmp) :
                cmp(std::forward<Comparerx>(cmp)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Option<typename Iterator::Item> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "max_by can only take rvalue ref object with Iterator traits");

                using Item = typename Iterator::Item;
                auto max_opt = Option<Item>(None);

                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    if (max_opt.is_some()) {
                        auto lhs = reverse_decay(std::move(max_opt).unwrap_unchecked());
                        auto rhs = reverse_decay(std::move(next_opt).unwrap_unchecked());

                        // Some will automatically perform special decay
                        if (cmp(special_decay(lhs), special_decay(rhs))) {
                            max_opt = Some(lhs);
                        } else {
                            max_opt = Some(rhs);
                        }
                    } else {
                        max_opt = std::move(next_opt);
                    }
                }

                return std::move(max_opt);
            }

        private:
            Comparer cmp;
        };

        class MaxOp {
        public:
            template <class Iterator>
            auto operator()(Iterator &&it) && -> Option<typename Iterator::Item> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "max can only take rvalue ref object with Iterator traits");

                using Comparer = std::greater_equal<typename Iterator::Item>;
                return MaxByOp<Comparer>(Comparer())(std::forward<Iterator>(it));
            }
        };
    }

    inline auto max()
        -> details::MaxOp {

        return details::MaxOp();
    }

    template <class Comparer>
    auto max_by(Comparer &&cmp)
        -> details::MaxByOp<special_decay_t<Comparer>> {

        return details::MaxByOp<special_decay_t<Comparer>>(std::forward<Comparer>(cmp));
    }
}
