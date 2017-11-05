/**
 * Contains Rust Iterator min and min_by equivalent implementation.
 *
 * min function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.min
 *
 * min_by function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.min_by
 *
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
class MinByOp {
public:
    template <class Fx>
    explicit MinByOp(Fx &&f);

    template <class Self>
    auto operator()(Self &&self) && -> Option<typename Self::Item>;

private:
    F f;
};

class MinOp {
public:
    template <class Self>
    auto operator()(Self &&self) && -> Option<typename Self::Item>;
};

/**
 * fn min(self) -> Option<Self::Item>
 * where
 *     Self::Item: Ord,
 */
auto min() -> MinOp;

/**
 * fn min_by<F>(self, compare: F) -> Option<Self::Item>
 * where
 *     F: FnMut(&Self::Item, &Self::Item) -> Ordering,
 */
template <class F>
auto min_by(F &&f) -> MinByOp<special_decay_t<F>>;

// implementation section

template <class F>
template <class Fx>
MinByOp<F>::MinByOp(Fx &&f) : f(std::forward<Fx>(f)) {
}

template <class F>
template <class Self>
auto MinByOp<F>::operator()(Self &&self) && -> Option<typename Self::Item> {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "min_by can only take rvalue ref object with Iterator traits");

    using Item = typename Self::Item;
    auto min_opt = Option<Item>(None);

    while (true) {
        auto next_opt = self.next();

        if (next_opt.is_none()) {
            break;
        }

        if (min_opt.is_some()) {
            auto lhs = reverse_decay(std::move(min_opt).unwrap_unchecked());
            auto rhs = reverse_decay(std::move(next_opt).unwrap_unchecked());

            // Some will automatically perform special decay
            if (f(special_decay(lhs), special_decay(rhs))) {
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

template <class Self>
auto MinOp::operator()(Self &&self) && -> Option<typename Self::Item> {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "min can only take rvalue ref object with Iterator traits");

    using F = std::less_equal<typename Self::Item>;
    return MinByOp<F>(F())(std::forward<Self>(self));
}

inline auto min() -> MinOp {
    return MinOp();
}

template <class F>
auto min_by(F &&f) -> MinByOp<special_decay_t<F>> {
    return MinByOp<special_decay_t<F>>(std::forward<F>(f));
}
} // namespace rustfp
