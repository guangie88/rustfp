/**
 * Emulates the for loop in Rust, for rustfp Self. Has similar types used in
 * inspect method of Rust iterator, except that for_each here performs eager
 * evaluation and takes in the moved type, unlike Rust Iterator inspect, which
 * performs lazy evaluation and takes in reference type.
 *
 * Iterators with for-loop: https://doc.rust-lang.org/1.8.0/book/iterators.html
 *
 * inspect function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.inspect
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "unit.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

// declaration section

template <class F>
class ForEachOp {
public:
    template <class Fx>
    explicit ForEachOp(Fx &&f);

    template <class Self>
    auto operator()(Self &&self) && -> unit_t;

private:
    F f;
};

/**
 * fn for_each<F>(self, f: F) -> ()
 * where
 *     F: FnMut(Self::Item) -> (),
 */
template <class F>
auto for_each(F &&f)
    -> ForEachOp<std::remove_reference_t<std::remove_const_t<F>>>;

// implementation section

template <class F>
template <class Fx>
ForEachOp<F>::ForEachOp(Fx &&f) : f(std::forward<Fx>(f)) {
}

template <class F>
template <class Self>
auto ForEachOp<F>::operator()(Self &&self) && -> unit_t {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "for_each can only take rvalue ref object with Iterator traits");

    while (true) {
        auto value_opt = self.next();

        if (value_opt.is_none()) {
            break;
        }

        f(std::move(value_opt).unwrap_unchecked());
    }

    return Unit;
}

template <class F>
auto for_each(F &&f)
    -> ForEachOp<std::remove_reference_t<std::remove_const_t<F>>> {

    return ForEachOp<std::remove_reference_t<std::remove_const_t<F>>>(
        std::forward<F>(f));
}
} // namespace rustfp
