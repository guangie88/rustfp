#pragma once

#include "option.h"
#include "util.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace rustfp {
    
    // implementation section

    namespace details {
        template <class Iterator>
        class Take {
        public:
            using Item = typename Iterator::Item;

            template <class Iteratorx>
            Take(Iteratorx &&it, const size_t count) :
                it(std::forward<Iteratorx>(it)),
                count(count) {

            }

            auto next() -> Option<Item> {
                if (count > 0) {
                    --count;
                    return it.next();
                }

                return None;
            }

        private:
            Iterator it;
            size_t count;
        };

        class TakeOp {
        public:
            explicit TakeOp(const size_t count) :
                count(count) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Take<Iterator> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "take can only take rvalue ref object with Iterator traits");

                return Take<Iterator>(std::move(it), count);
            }

        private:
            size_t count;
        };
    }

    inline auto take(const size_t count) -> details::TakeOp {
        return details::TakeOp(count);
    }
}
