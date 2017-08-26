/**
 * Contains Rust Iterator filter_map equivalent implementation
 * filter_map function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.filter_map
 * FilterMap struct: https://doc.rust-lang.org/std/iter/struct.FilterMap.html
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
     * impl<B, I, F> Iterator for FilterMap<I, F> 
     * where
     *     F: FnMut(<I as Iterator>::Item) -> Option<B>,
     *     I: Iterator, 
     * type Item = B
     */
    template <class Self, class F>
    class FilterMap {
    public:
        /** Type alias to rustfp Iter type */
        using I = Self;

        /** Type to be returned in Some */
        using B = typename std::result_of_t<F(typename Self::Item)>::some_t;

        /** Item type to generate */
        using Item = B;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @tparam Fx Forwarded type of F, where F(Self) -> U
         * @param self rustfp Iter instance
         * @param f Function type F instance
         */
        template <class Selfx, class Fx>
        FilterMap(Selfx &&self, Fx &&f);

        /**
         * Generates the next value of filter_map operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
        F f;
    };

    template <class F>
    class FilterMapOp {
    public:
        template <class Fx>
        explicit FilterMapOp(Fx &&f);

        template <class Self>
        auto operator()(Self &&self) && -> FilterMap<Self, F>;

    private:
        F f;
    };

    /**
     * fn filter_map<B, F>(self, f: F) -> FilterMap<Self, F> 
     * where
     *     F: FnMut(Self::Item) -> Option<B>, 
     */
    template <class F>
    auto filter_map(F &&f) -> FilterMapOp<special_decay_t<F>>;
    
    // implementation section

    template <class Self, class F>
    template <class Selfx, class Fx>
    FilterMap<Self, F>::FilterMap(Selfx &&self, Fx &&f) :
        self(std::forward<Selfx>(self)),
        f(std::forward<Fx>(f)) {

    }

    template <class Self, class F>
    auto FilterMap<Self, F>::next() -> Option<Item> {
        while (true)
        {
            auto next_opt = self.next();

            if (next_opt.is_none()) {
                break;
            }

            auto mapped_opt = f(std::move(next_opt).unwrap_unchecked());

            if (mapped_opt.is_some()) {
                return std::move(mapped_opt);
            }
        }

        return None;
    }

    template <class F>
    template <class Fx>
    FilterMapOp<F>::FilterMapOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Self>
    auto FilterMapOp<F>::operator()(Self &&self) && -> FilterMap<Self, F> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "filter_map can only take rvalue ref object with Iterator traits");

        return FilterMap<Self, F>(std::move(self), std::move(f));
    }

    template <class F>
    auto filter_map(F &&f) -> FilterMapOp<special_decay_t<F>> {
        return FilterMapOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
