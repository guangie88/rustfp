/**
 * Specially added find_map operation, which has the effect of running find,
 * followed by map operations, for convenience and practicality.
 * find function: https://doc.rust-lang.org/std/iter/trait.Self.html#method.find
 * Find struct: https://doc.rust-lang.org/std/iter/struct.Find.html
 * map function: https://doc.rust-lang.org/std/iter/trait.Self.html#method.map
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

    template <class F>
    class FindMapOp {
    public:
        template <class Fx>
        explicit FindMapOp(Fx &&f);

        template <class Self>
        auto operator()(Self &&self) &&
            -> std::result_of_t<F(typename Self::Item)>;

    private:
        F f;
    };

    /**
     * fn find_map<F, B>(&mut self, f: F) -> FilterMap<Self, F> 
     * where
     *     F: FnMut(&Self::Item) -> Option<B>, 
     */
    template <class F>
    auto find_map(F &&f) -> FindMapOp<special_decay_t<F>>;
    
    // implementation section

    template <class F>
    template <class Fx>
    FindMapOp<F>::FindMapOp(Fx &&f) :
        f(std::forward<Fx>(f)) {

    }

    template <class F>
    template <class Self>
    auto FindMapOp<F>::operator()(Self &&self) &&
        -> std::result_of_t<F(typename Self::Item)> {

        static_assert(!std::is_lvalue_reference<Self>::value,
            "find_map can only take rvalue ref object with Iterator traits");

        while (true) {
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
    auto find_map(F &&f) -> FindMapOp<special_decay_t<F>> {
        return FindMapOp<special_decay_t<F>>(std::forward<F>(f));
    }
}
