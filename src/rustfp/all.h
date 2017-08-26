/**
 * Contains Rust Iterator all equivalent implementation
 * all function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.all
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
    class AllOp {
    public:
        template <class Fx>
        AllOp(Fx &&f);

        template <class Iterator>
        auto operator()(Iterator &&it) && -> bool;

    private:
        F f;
    };

    /**
     * fn all<F>(&mut self, f: F) -> bool 
     * where
     *     F: FnMut(Self::Item) -> bool, 
     */
    template <class F>
    auto all(F &&f) -> AllOp<special_decay_t<F>>;

    // implementation section

    template <class F>
    template <class Fx>
    AllOp<F>::AllOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Iterator>
    auto AllOp<F>::operator()(Iterator &&it) && -> bool {
        static_assert(!std::is_lvalue_reference<Iterator>::value,
            "all can only take rvalue ref object with Iterator traits");

        while (true) {
            auto next_opt = it.next();

            if (next_opt.is_none()) {
                break;
            }

            if (!f(std::move(next_opt).unwrap_unchecked())) {
                return false;
            }
        }

        return true;
    }

    template <class F>
    auto all(F &&f) -> AllOp<special_decay_t<F>> {
        return AllOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
