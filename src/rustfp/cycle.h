#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Copyable>
        class Cycle
        {
        public:
            using Item = Copyable;

            template <class Copyablex>
            Cycle(Copyablex &&copyable) :
                copyable(std::forward<Copyablex>(copyable))
            {
            }

            auto next() -> Option<Item>
            {
                return Some(reverse_decay_t<Copyable>(copyable));
            }

        private:
            reverse_decay_t<Copyable> copyable;
        };
    }

    template <class Copyable>
    auto cycle(Copyable &&copyable) -> details::Cycle<special_decay_t<Copyable>>
    {
        return details::Cycle<special_decay_t<Copyable>>(std::forward<Copyable>(copyable));
    }
}