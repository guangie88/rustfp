#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section

    namespace details {
        template <class Pred>
        class FindOp {
        public:
            template <class Predx>
            explicit FindOp(Predx &&pred) :
                pred(std::forward<Predx>(pred)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Option<typename Iterator::Item> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "find can only take rvalue ref object with Iterator traits");

                while (true) {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    if (pred(next_opt.get_unchecked())) {
                        return std::move(next_opt);
                    }
                }

                return None;
            }

        private:
            Pred pred;
        };
    }

    template <class Pred>
    auto find(Pred &&pred) -> details::FindOp<special_decay_t<Pred>> {
        return details::FindOp<special_decay_t<Pred>>(std::forward<Pred>(pred));
    }
}
