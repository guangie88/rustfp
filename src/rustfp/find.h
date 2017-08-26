/**
 * Contains Rust Iterator find equivalent implementation
 * find function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.find
 * Find struct: https://doc.rust-lang.org/std/iter/struct.Find.html
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

    template <class P>
    class FindOp {
    public:
        template <class Px>
        explicit FindOp(Px &&p);

        template <class Self>
        auto operator()(Self &&self) && -> Option<typename Self::Item>;

    private:
        P p;
    };

    /**
     * fn find<P>(&mut self, predicate: P) -> Option<Self::Item> 
     * where
     *     P: FnMut(&Self::Item) -> bool, 
     */
    template <class P>
    auto find(P &&p) -> FindOp<special_decay_t<P>>;

    // implementation section

    template <class P>
    template <class Px>
    FindOp<P>::FindOp(Px &&p) :
        p(std::forward<Px>(p)) {

    }

    template <class P>
    template <class Self>
    auto FindOp<P>::operator()(Self &&self) && -> Option<typename Self::Item> {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "find can only take rvalue ref object with Iterator traits");

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
    auto find(P &&p) -> FindOp<special_decay_t<P>> {
        return FindOp<special_decay_t<P>>(std::forward<P>(p));
    }
}
