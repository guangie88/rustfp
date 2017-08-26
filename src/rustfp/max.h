/**
 * Contains Rust Iterator max and max_by equivalent implementation
 * max function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.max
 * max_by function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.max_by
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    template <class F>
    class MaxByOp {
    public:
        template <class Fx>
        MaxByOp(Fx &&f);

        template <class Self>
        auto operator()(Self &&self) && -> Option<typename Self::Item>;

    private:
        F f;
    };

    class MaxOp {
    public:
        template <class Self>
        auto operator()(Self &&self) && -> Option<typename Self::Item>;
    };

    /**
     * fn max(self) -> Option<Self::Item> 
     * where
     *     Self::Item: Ord, 
     */
    auto max() -> MaxOp;

    /**
     * fn max_by<F>(self, compare: F) -> Option<Self::Item> 
     * where
     *     F: FnMut(&Self::Item, &Self::Item) -> Ordering, 
     */
    template <class F>
    auto max_by(F &&f) -> MaxByOp<special_decay_t<F>>;

    // implementation section

    template <class F>
    template <class Fx>
    MaxByOp<F>::MaxByOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Self>
    auto MaxByOp<F>::operator()(Self &&self) && -> Option<typename Self::Item> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "max_by can only take rvalue ref object with Iterator traits");

        using Item = typename Self::Item;
        auto max_opt = Option<Item>(None);

        while (true)
        {
            auto next_opt = self.next();

            if (next_opt.is_none()) {
                break;
            }

            if (max_opt.is_some()) {
                auto lhs = reverse_decay(std::move(max_opt).unwrap_unchecked());
                auto rhs = reverse_decay(std::move(next_opt).unwrap_unchecked());

                // Some will automatically perform special decay
                if (f(special_decay(lhs), special_decay(rhs))) {
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

    template <class Self>
    auto MaxOp::operator()(Self &&self) && -> Option<typename Self::Item> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "max can only take rvalue ref object with Iterator traits");

        using F = std::greater_equal<typename Self::Item>;
        return MaxByOp<F>(F())(std::forward<Self>(self));
    }

    inline auto max() -> MaxOp {
        return MaxOp();
    }

    template <class F>
    auto max_by(F &&f) -> MaxByOp<special_decay_t<F>> {
        return MaxByOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
