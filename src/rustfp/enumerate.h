/**
 * Contains Rust Iterator enumerate equivalent implementation
 * enumerate function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.enumerate
 * Enumerate struct: https://doc.rust-lang.org/std/iter/struct.Enumerate.html
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
     * impl<I> Iterator for Enumerate<I> 
     * where
     *     I: Iterator, 
     * type Item = (usize, <I as Iterator>::Item)
     */
    template <class Self>
    class Enumerate {
    public:
        /** Type alias to rustfp Iter type */
        using I = Self;

        /** Item type to generate */
        using Item = std::pair<size_t, typename Self::Item>;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @param self rustfp Iter instance
         */
        template <class Selfx>
        Enumerate(Selfx &&self);

        /**
         * Generates the next value of enumerate operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
        size_t index;
    };

    class EnumerateOp {
    public:
        template <class Self>
        auto operator()(Self &&self) && -> Enumerate<Self>;
    };

    /**
     * fn enumerate(self) -> Enumerate<Self>
     */
    auto enumerate() -> EnumerateOp;

    // implementation section

    template <class Self>
    template <class Selfx>
    Enumerate<Self>::Enumerate(Selfx &&self) :
        self(std::forward<Selfx>(self)),
        index(0) {

    }

    template <class Iterator>
    auto Enumerate<Iterator>::next() -> Option<Item> {
        return self.next().map([this](auto &&value) {
            return std::pair<size_t, typename Iterator::Item>(
                index++,
                std::forward<typename Iterator::Item>(value));
        });
    }

    template <class Self>
    auto EnumerateOp::operator()(Self &&self) && -> Enumerate<Self> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "enumerate can only take rvalue ref object with Iterator traits");

        return Enumerate<Self>(std::move(self));
    }

    inline auto enumerate() -> EnumerateOp {
        return EnumerateOp();
    }
}
