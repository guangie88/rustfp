/**
 * Contains Rust Iterator fold equivalent implementation.
 *
 * fold function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.fold
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

// declaration section

template <class B, class F>
class FoldOp {
public:
    template <class Bx, class Fx>
    FoldOp(Bx &&init, Fx &&f);

    template <class Self>
    auto operator()(Self &&self) && -> B;

private:
    B init;
    F f;
};

/**
 * F fold<B, F>(self, init: B, f: F) -> B
 * where
 *     F: FnMut(B, Self::Item) -> B,
 */
template <class B, class F>
auto fold(B &&init, F &&f) -> FoldOp<special_decay_t<B>, special_decay_t<F>>;

// implementation section

template <class B, class F>
template <class Bx, class Fx>
FoldOp<B, F>::FoldOp(Bx &&init, Fx &&f)
    : init(std::forward<Bx>(init)), f(std::forward<Fx>(f)) {
}

template <class B, class F>
template <class Self>
auto FoldOp<B, F>::operator()(Self &&self) && -> B {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "fold can only take rvalue ref object with Iterator traits");

    auto op_acc = std::move(init);

    while (true) {
        auto next_opt = self.next();

        if (next_opt.is_none()) {
            break;
        }

        op_acc = f(std::move(op_acc), std::move(next_opt).unwrap_unchecked());
    }

    return op_acc;
}

template <class B, class F>
auto fold(B &&init, F &&f) -> FoldOp<special_decay_t<B>, special_decay_t<F>> {

    return FoldOp<special_decay_t<B>, special_decay_t<F>>(
        std::forward<B>(init), std::forward<F>(f));
}
} // namespace rustfp
