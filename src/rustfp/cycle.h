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
    template <class Copyable>
    class Cycle {
    public:
        using Item = Copyable;

        template <class Copyablex>
        Cycle(Copyablex &&copyable);

        auto next() -> Option<Item>;

    private:
        reverse_decay_t<Copyable> copyable;
    };

    /**
     * fn cycle(self) -> Cycle<Self> 
     * where
     *     Self: Clone, 
     */
    template <class Copyable>
    auto cycle(Copyable &&copyable) -> Cycle<special_decay_t<Copyable>>;

    // implementation section

    template <class Copyable>
    template <class Copyablex>
    Cycle<Copyable>::Cycle(Copyablex &&copyable) :
        copyable(std::forward<Copyablex>(copyable)) {

    }

    template <class Copyable>
    auto Cycle<Copyable>::next() -> Option<Item> {
        return Some(reverse_decay_t<Copyable>(copyable));
    }

    template <class Copyable>
    auto cycle(Copyable &&copyable) -> Cycle<special_decay_t<Copyable>> {
        return Cycle<special_decay_t<Copyable>>(std::forward<Copyable>(copyable));
    }
}
