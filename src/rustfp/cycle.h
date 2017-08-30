/**
 * Contains Rust Iterator cycle equivalent implementation
 * cycle function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.cycle
 * Cycle struct: https://doc.rust-lang.org/std/iter/struct.Cycle.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp {
    
    // declaration section

    /**
     * impl<I> Iterator for Cycle<I> 
     * where
     *     I: Clone + Iterator, 
     * type Item = <I as Iterator>::Item
     */
    template <class Self>
    class Cycle {
    public:
        // to ensure that the given Self iterator is clonable
        // which means generally means copy assignable in c++
        static_assert(std::is_copy_assignable<Self>::value, "Cycle can only take in copy assignable types. "
            "Operations like map tend to generate rvalues and makes the iterator not copy assignable.");

        /** Type alias to rustfp Iter type */
        using I = Self;
        
        /** Item type to generate */
        using Item = typename Self::Item;

        /**
         * Takes in both the moved rustfp Iter instance and
         * and function type F instance.
         * @tparam Selfx Forwarded type of Self, rustfp Iter type
         * @param self rustfp Iter instance
         */
        template <class Selfx>
        Cycle(Selfx &&self);

        /**
         * Generates the next value of cycle operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        reverse_decay_t<Self> self;
        reverse_decay_t<Self> ref_self;
    };

    class CycleOp {
    public:
        template <class Self>
        auto operator()(Self &&self) && -> Cycle<Self>;
    };

    /**
     * fn cycle(self) -> Cycle<Self> 
     * where
     *     Self: Clone, 
     */
    auto cycle() -> CycleOp;

    // implementation section

    template <class Self>
    template <class Selfx>
    Cycle<Self>::Cycle(Selfx &&self) :
        self(self),
        ref_self(std::forward<Selfx>(self)) {

    }

    template <class Self>
    auto Cycle<Self>::next() -> Option<Item> {
        auto item = self.next();

        if (item.is_some()) {
            return std::move(item);
        } else {
            // handles the case of empty iterator
            self = ref_self;
            return self.next();
        }
    }

    template <class Self>
    auto CycleOp::operator()(Self &&self) && -> Cycle<Self> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "cycle can only take rvalue ref object with Iterator traits");

        return Cycle<Self>(std::move(self));
    }

    inline auto cycle() -> CycleOp {
        return CycleOp();
    }
}
