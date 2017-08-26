/**
 * Contains Rust Iterator cloned equivalent implementation
 * cloned function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.cloned
 * Cloned struct: https://doc.rust-lang.org/std/iter/struct.Cloned.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    /**
     * impl<'a, I, T> Iterator for Cloned<I> 
     * where
     *     I: Iterator<Item = &'a T>,
     *     T: 'a + Clone, 
     * type Item = T
     */
    template <class Self>
    class Cloned {
    public:
        /** Type alias to rustfp Iter type */
        using I = Self;

        /** Type alias to the cloned type after removing const and reference specifiers */
        using T = std::remove_const_t<std::remove_reference_t<typename Self::Item>>;

        /** Item type to generate */
        using Item = T;

        /**
         * Takes in the moved rustfp Iter instance for cloning.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @param self rustfp Iter instance
         */
        template <class Selfx>
        Cloned(Selfx &&self);

        /**
         * Generates the next value of cloned operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
    };

    class ClonedOp
    {
    public:
        template <class Self>
        auto operator()(Self &&self) && -> Cloned<Self>;
    };

    /**
     * fn cloned<'a, T>(self) -> Cloned<Self> 
     * where
     *     Self: Iterator<Item = &'a T>,
     *     T: 'a + Clone, 
     */
    auto cloned() -> ClonedOp;
    
    // implementation section

    template <class Self>
    template <class Selfx>
    Cloned<Self>::Cloned(Selfx &&self) :
        self(std::forward<Selfx>(self)) {

    }

    template <class Self>
    auto Cloned<Self>::next() -> Option<Item> {
        return self.next().map([](auto &&value) -> Item {
            return value;
        });
    }

    template <class Self>
    auto ClonedOp::operator()(Self &&self) && -> Cloned<Self> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "cloned can only take rvalue ref object with Iterator traits");

        return Cloned<Self>(std::move(self));
    }

    inline auto cloned() -> ClonedOp {
        return ClonedOp();
    }
}
