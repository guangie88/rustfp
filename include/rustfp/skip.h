/**
 * Contains Rust Iterator skip equivalent implementation.
 *
 * skip function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.skip
 *
 * Skip struct: https://doc.rust-lang.org/std/iter/struct.Skip.html
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "util.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace rustfp {

// declaration section

/**
 * impl<I> Iterator for Skip<I>
 * where
 *     I: Iterator,
 * type Item = <I as Iterator>::Item
 */
template <class Self>
class Skip {
public:
    /**
     * Type alias to rustfp Iter type.
     */
    using I = Self;

    /**
     * Item type to generate.
     */
    using Item = typename Self::Item;

    /**
     * Takes in both the moved rustfp Iter instance and function type F
     * instance.
     * @tparam Selfx Forwarded type of Self, rustfp Iter type
     * @param self rustfp Iter instance
     * @param count Number of items to skip
     */
    template <class Selfx>
    Skip(Selfx &&self, const size_t count);

    /**
     * Generates the next value of skip operation.
     * @return Some(Item) if there is a next value to generate, otherwise None.
     */
    auto next() -> Option<Item>;

private:
    Self self;
    size_t count;
};

class SkipOp {
public:
    explicit SkipOp(const size_t count);

    template <class Self>
    auto operator()(Self &&self) && -> Skip<Self>;

private:
    size_t count;
};

/**
 * fn skip(self, n: usize) -> Skip<Self>
 */
auto skip(const size_t count) -> SkipOp;

// implementation section

template <class Self>
template <class Selfx>
Skip<Self>::Skip(Selfx &&self, const size_t count)
    : self(std::forward<Selfx>(self)), count(count) {
}

template <class Self>
auto Skip<Self>::next() -> Option<Item> {
    while (count > 0) {
        self.next();
        --count;
    }

    return self.next();
}

inline SkipOp::SkipOp(const size_t count) : count(count) {
}

template <class Self>
auto SkipOp::operator()(Self &&self) && -> Skip<Self> {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "skip can only take rvalue ref object with Self traits");

    return Skip<Self>(std::move(self), count);
}

inline auto skip(const size_t count) -> SkipOp {
    return SkipOp(count);
}
} // namespace rustfp
