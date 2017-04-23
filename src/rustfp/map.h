#pragma once

#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Iterator, class FnToType>
        class Map
        {
        public:
            using Item = special_decay_t<std::result_of_t<FnToType(typename Iterator::Item)>>;

            template <class Iteratorx, class FnToTypex>
            Map(Iteratorx &&it, FnToTypex &&fn) :
                it(std::forward<Iteratorx>(it)),
                fn(std::forward<FnToTypex>(fn))
            {
            }

            auto next() -> Option<Item>
            {
                return it.next().map(
                    [this](auto &&value)
                    {
                        return fn(std::forward<decltype(value)>(value));
                    });
            }

        private:
            Iterator it;
            FnToType fn;
        };

        template <class FnToType>
        class MapOp
        {
        public:
            template <class FnToTypex>
            explicit MapOp(FnToTypex &&fn) :
                fn(std::forward<FnToTypex>(fn))
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Map<Iterator, FnToType>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "map can only take rvalue ref object with Iterator traits");
                return Map<Iterator, FnToType>(std::move(it), std::move(fn));
            }

        private:
            FnToType fn;
        };
    }

    template <class FnToType>
    auto map(FnToType &&fn) -> details::MapOp<special_decay_t<FnToType>>
    {
        return details::MapOp<special_decay_t<FnToType>>(std::forward<FnToType>(fn));
    }
}