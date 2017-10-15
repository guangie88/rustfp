/**
 * Contains Rust Iterator flat_map equivalent implementation
 * flat_map function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.flat_map
 * FlatMap struct: https://doc.rust-lang.org/std/iter/struct.FlatMap.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "iter.h"
#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    /**
     * impl<I, U, F> Iterator for FlatMap<I, U, F> 
     * where
     *     F: FnMut(<I as Iterator>::Item) -> U,
     *     I: Iterator,
     *     U: IntoIterator, 
     * type Item = <U as IntoIterator>::Item
     */
    template <class Self, class F>
    class FlatMap {
    public:
        /** Type alias to rustfp Iter type */
        using I = Self;

        /** Type alias to the result of function type F */
        using U = std::result_of_t<F(typename Self::Item)>;

        /** Item type to generate */
        using Item = typename IntoIter<U>::Item;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @tparam Fx Forwarded type of F, where F(Self) -> U
         * @param self rustfp Iter instance
         * @param f Function type F instance
         */
        template <class Selfx, class Fx>
        FlatMap(Selfx &&self, Fx &&f);

        /**
         * Generates the next value of flat map operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
        Option<IntoIter<U>> sub_self_opt;
        F f;
    };

    template <class F>
    class FlatMapOp {
    public:
        template <class Fx>
        explicit FlatMapOp(Fx &&f);

        template <class Self>
        auto operator()(Self &&self) && -> FlatMap<Self, F>;

    private:
        F f;
    };

    /**
     * Implements the following:
     * fn flat_map<U, F>(self, f: F) -> FlatMap<Self, U, F> 
     * where
     *     F: FnMut(Self::Item) -> U,
     *     U: IntoIterator,
     */
    template <class F>
    auto flat_map(F &&f) -> FlatMapOp<special_decay_t<F>>;

    // implementation section

    template <class Self, class F>
    template <class Selfx, class Fx>
    FlatMap<Self, F>::FlatMap(Selfx &&self, Fx &&f) :
        self(std::forward<Selfx>(self)),
        sub_self_opt(None),
        f(std::forward<Fx>(f)) {

    }

    template <class Self, class F>
    auto FlatMap<Self, F>::next() -> Option<Item> {
        while (true) {
            if (sub_self_opt.is_none()) {
                auto item_opt = self.next();

                if (item_opt.is_none()) {
                    // base case
                    return None;
                }

                // load up the sub_self and retry again
                sub_self_opt = Some(into_iter(f(std::move(item_opt).unwrap_unchecked())));
                continue;
            }

            auto sub_self = std::move(sub_self_opt).unwrap_unchecked();
            auto next_opt = sub_self.next();

            if (next_opt.is_none()) {
                sub_self_opt = None;
                continue;
            }

            sub_self_opt = Some(move(sub_self));

            // success case
            return std::move(next_opt);
        }
    }

    template <class F>
    template <class Fx>
    FlatMapOp<F>::FlatMapOp(Fx &&f) :
        f(std::forward<Fx>(f)) {
    
    }

    template <class F>
    template <class Self>
    auto FlatMapOp<F>::operator()(Self &&self) && -> FlatMap<Self, F> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "flat_map can only take rvalue ref object with Iterator traits");

        return FlatMap<Self, F>(std::move(self), std::move(f));
    }

    template <class F>
    auto flat_map(F &&f) -> FlatMapOp<special_decay_t<F>> {
        return FlatMapOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
