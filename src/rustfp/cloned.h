#pragma once

#include "option.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section
    
    namespace details {
        template <class Iterator>
        class Cloned {
        public:
            using Item = std::remove_const_t<std::remove_reference_t<typename Iterator::Item>>;

            template <class Iteratorx>
            Cloned(Iteratorx &&it) :
                it(std::forward<Iteratorx>(it)) {

            }

            auto next() -> Option<Item> {
                return it.next().map([](auto &&value) -> Item {
                    return value;
                });
            }

        private:
            Iterator it;
        };

        class ClonedOp
        {
        public:
            template <class Iterator>
            auto operator()(Iterator &&it) && -> Cloned<Iterator> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "cloned can only take rvalue ref object with Iterator traits");

                return Cloned<Iterator>(std::move(it));
            }
        };
    }

    inline auto cloned() -> details::ClonedOp {
        return details::ClonedOp();
    }
}
