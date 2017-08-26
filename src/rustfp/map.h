/**
 * Contains Rust Iterator map equivalent implementation
 * map function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.map
 * Map struct: https://doc.rust-lang.org/std/iter/struct.Map.html
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
     * impl<B, I, F> Iterator for Map<I, F> 
     * where
     *     F: FnMut(<I as Iterator>::Item) -> B,
     *     I: Iterator, 
     * type Item = B
     */
    template <class Self, class F>
    class Map {
    public:
        /** Type alias to rustfp Iter type */    
        using I = Self;

        /** Type alias to the result of function type F */
        using B = special_decay_t<std::result_of_t<F(typename Self::Item)>>;

        /** Item type to generate */
        using Item = B;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @tparam Fx Forwarded type of F, where F(Self) -> B
         * @param self rustfp Iter instance
         * @param f Function type F instance
         */
        template <class Selfx, class Fx>
        Map(Selfx &&self, Fx &&f);

        /**
         * Generates the next value of map operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
        F f;
    };

    template <class F>
    class MapOp {
    public:
        template <class Fx>
        explicit MapOp(Fx &&f);

        template <class Self>
        auto operator()(Self &&self) && -> Map<Self, F>;

    private:
        F f;
    };

    /**
     * fn map<B, F>(self, f: F) -> Map<Self, F> 
     * where
     *     F: FnMut(Self::Item) -> B, 
     */
    template <class F>
    auto map(F &&f) -> MapOp<special_decay_t<F>>;

    // implementation section

    template <class Self, class F>
    template <class Selfx, class Fx>
    Map<Self, F>::Map(Selfx &&self, Fx &&f) :
        self(std::forward<Selfx>(self)),
        f(std::forward<Fx>(f)) {

    }

    template <class Self, class F>
    auto Map<Self, F>::next() -> Option<Item> {
        return self.next().map([this](auto &&value) {
            return f(std::forward<typename Self::Item>(value));
        });
    }

    template <class F>
    template <class Fx>
    MapOp<F>::MapOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Self>
    auto MapOp<F>::operator()(Self &&self) && -> Map<Self, F> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "map can only take rvalue ref object wselfh Self traselfs");

        return Map<Self, F>(std::move(self), std::move(f));
    }

    template <class F>
    auto map(F &&f) -> MapOp<special_decay_t<F>> {
        return MapOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
