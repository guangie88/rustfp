/**
 * Contains Rust Iterator take equivalent implementation.
 *
 * take function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.take
 *
 * Take struct: https://doc.rust-lang.org/std/iter/struct.Take.html
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
 * impl<I> Iterator for Take<I>
 * where
 *     I: Iterator,
 * type Item = <I as Iterator>::Item
 */
template <class Self>
class Take {
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
     * @param count Number of items to take
     */
    template <class Selfx>
    Take(Selfx &&self, const size_t count);

    /**
     * Generates the next value of take operation.
     * @return Some(Item) if there is a next value to generate,
     * otherwise None.
     */
    auto next() -> Option<Item>;

private:
    Self self;
    size_t count;
};

class TakeOp {
public:
    explicit TakeOp(const size_t count);

    template <class Self>
    auto operator()(Self &&self) && -> Take<Self>;

private:
    size_t count;
};

/**
 * fn take(self, n: usize) -> Take<Self>
 */
auto take(const size_t count) -> TakeOp;

// implementation section

template <class Self>
template <class Selfx>
Take<Self>::Take(Selfx &&self, const size_t count)
    : self(std::forward<Selfx>(self)), count(count) {
}

template <class Self>
auto Take<Self>::next() -> Option<Item> {
    if (count > 0) {
        --count;
        return self.next();
    }

    return None;
}

TakeOp::TakeOp(const size_t count) : count(count) {
}

template <class Self>
auto TakeOp::operator()(Self &&self) && -> Take<Self> {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "take can only take rvalue ref object with Self traits");

    return Take<Self>(std::move(self), count);
}

inline auto take(const size_t count) -> TakeOp {
    return TakeOp(count);
}
} // namespace rustfp
