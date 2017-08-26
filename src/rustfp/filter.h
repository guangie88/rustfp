/**
 * Contains Rust Iterator filter equivalent implementation
 * filter function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.filter
 * Filter struct: https://doc.rust-lang.org/std/iter/struct.Filter.html
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
     * impl<I, P> Iterator for Filter<I, P> 
     * where
     *     I: Iterator,
     *     P: FnMut(&<I as Iterator>::Item) -> bool, 
     * type Item = <I as Iterator>::Item
     */
    template <class Self, class P>
    class Filter {
    public:
        /** Type alias to rustfp Iter type */
        using I = Self;

        /** Item type to generate */
        using Item = typename Self::Item;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @tparam Fx Forwarded type of F, where F(Self) -> U
         * @param self rustfp Iter instance
         * @param f Function type F instance
         */
        template <class Selfx, class Px>
        Filter(Selfx &&self, Px &&p);

        /**
         * Generates the next value of filter operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Self self;
        P p;
    };

    template <class P>
    class FilterOp {
    public:
        template <class Px>
        explicit FilterOp(Px &&p);

        template <class Self>
        auto operator()(Self &&self) && -> Filter<Self, P>;

    private:
        P p;
    };

    /**
     * fn filter<P>(self, predicate: P) -> Filter<Self, P> 
     * where
     *     P: FnMut(&Self::Item) -> bool, 
     */
    template <class P>
    auto filter(P &&p) -> FilterOp<special_decay_t<P>>;
    
    // implementation section

    template <class Self, class P>
    template <class Selfx, class Px>
    Filter<Self, P>::Filter(Selfx &&self, Px &&p) :
        self(std::forward<Selfx>(self)),
        p(std::forward<Px>(p)) {

    }

    template <class Self, class P>
    auto Filter<Self, P>::next() -> Option<Item> {
        while (true) {
            auto next_opt = self.next();

            if (next_opt.is_none()) {
                break;
            }

            if (p(next_opt.get_unchecked())) {
                return std::move(next_opt);
            }
        }

        return None;
    }

    template <class P>
    template <class Px>
    FilterOp<P>::FilterOp(Px &&p) :
        p(std::forward<Px>(p)) {

    }

    template <class P>
    template <class Self>
    auto FilterOp<P>::operator()(Self &&self) && -> Filter<Self, P> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "filter can only take rvalue ref object with Iterator traits");

        return Filter<Self, P>(std::move(self), std::move(p));
    }

    template <class P>
    auto filter(P &&p) -> FilterOp<special_decay_t<P>> {
        return FilterOp<special_decay_t<P>>(std::forward<P>(p));
    }
}
