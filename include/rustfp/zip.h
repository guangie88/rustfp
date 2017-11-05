/**
 * Contains Rust Iterator zip equivalent implementation.
 *
 * zip function:
 * https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.zip
 *
 * Zip struct:
 * https://doc.rust-lang.org/std/iter/struct.Zip.html
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

// declaration section

/**
 * impl<Self, Other> Iterator for Zip<Self, Other>
 * where
 *     Self: Iterator,
 *     Other: Iterator,
 * type Item = (<Self as Iterator>::Item, <Other as Iterator>::Item)
 */
template <class Self, class Other>
class Zip {
public:
    /**
     * Type alias to rustfp Iter type for Self.
     */
    using A = Self;

    /**
     * Type alias to rustfp Iter type for Other (RHS).
     */
    using B = Other;

    /**
     * Item type to generate.
     */
    using Item = std::pair<typename Self::Item, typename Other::Item>;

    /**
     * Takes in both the moved rustfp Iter instance and
     * and function type F instance.
     * @tparam Selfx Forwarded type of Self, rustfp Iter type
     * @tparam Otherx Forwarded type of Other, another rustfp Iter type
     * @param self rustfp Iter instance
     * @param other RHS rustfp Iter instance
     */
    template <class Selfx, class Otherx>
    Zip(Selfx &&self, Otherx &&other);

    /**
     * Generates the next value of zip operation.
     * @return Some(Item) if there is a next value to generate,
     * otherwise None.
     */
    auto next() -> Option<Item>;

private:
    Self self;
    Other other;
};

template <class Other>
class ZipOp {
public:
    template <class Otherx>
    explicit ZipOp(Otherx &&other);

    template <class Self>
    auto operator()(Self &&self) && -> Zip<Self, Other>;

private:
    Other other;
};

/**
 * fn zip<U>(self, other: U) -> Zip<Self, <U as IntoIterator>::IntoIter>
 * where
 *     U: IntoIterator,
 */
template <class U>
auto zip(U &&other) -> ZipOp<U>;

// implementation section

template <class Self, class Other>
template <class Selfx, class Otherx>
Zip<Self, Other>::Zip(Selfx &&self, Otherx &&other)
    : self(std::forward<Selfx>(self)), other(std::forward<Otherx>(other)) {
}

template <class Self, class Other>
auto Zip<Self, Other>::next() -> Option<Item> {
    auto opt_next = self.next();
    auto opt_rhs_next = other.next();

    if (opt_next.is_some() && opt_rhs_next.is_some()) {
        return Some(Item(
            std::move(opt_next).unwrap_unchecked(),
            std::move(opt_rhs_next).unwrap_unchecked()));
    }

    return None;
}

template <class Other>
template <class Otherx>
ZipOp<Other>::ZipOp(Otherx &&other) : other(std::move(other)) {
}

template <class Other>
template <class Self>
auto ZipOp<Other>::operator()(Self &&self) && -> Zip<Self, Other> {
    static_assert(
        !std::is_lvalue_reference<Self>::value,
        "zip can only take rvalue ref object with Iterator traits");

    return Zip<Self, Other>(std::move(self), std::move(other));
}

template <class U>
auto zip(U &&other) -> ZipOp<U> {
    static_assert(
        !std::is_lvalue_reference<U>::value,
        "zip can only take second rvalue ref object with Iterator traits");

    return ZipOp<U>(std::move(other));
}
} // namespace rustfp
