/**
 * Contains Rust Iterator any equivalent implementation
 * any function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.any
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    template <class F>
    class AnyOp {
    public:
        template <class Fx>
        explicit AnyOp(Fx &&f);

        template <class Iterator>
        auto operator()(Iterator &&it) && -> bool;

    private:
        F f;
    };

    /**
     * fn any<F>(&mut self, f: F) -> bool 
     * where
     *     F: FnMut(Self::Item) -> bool, 
     */
    template <class F>
    auto any(F &&f) -> AnyOp<special_decay_t<F>>;
    
    // implementation section

    template <class F>
    template <class Fx>
    AnyOp<F>::AnyOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Iterator>
    auto AnyOp<F>::operator()(Iterator &&it) && -> bool {
        static_assert(!std::is_lvalue_reference<Iterator>::value,
            "any can only take rvalue ref object with Iterator traits");

        while (true) {
            auto next_opt = it.next();

            if (next_opt.is_none()) {
                break;
            }

            if (f(std::move(next_opt).unwrap_unchecked())) {
                return true;
            }
        }

        return false;
    }

    template <class F>
    auto any(F &&f) -> AnyOp<special_decay_t<F>> {
        return AnyOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
