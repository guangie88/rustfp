#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section

    namespace details {
        template <class Movable>
        class Once {
        public:
            using Item = Movable;

            template <class Movablex>
            Once(Movablex &&movable) :
                movable(Some(std::forward<Movablex>(movable))) {

            }

            auto next() -> Option<Item> {
                return std::move(movable);
            }

        private:
            Option<Movable> movable;
        };
    }

    template <class Movable>
    auto once(Movable &&movable) -> details::Once<special_decay_t<Movable>> {
        return details::Once<special_decay_t<Movable>>(std::forward<Movable>(movable));
    }
}
