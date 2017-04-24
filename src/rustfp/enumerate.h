#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Iterator>
        class Enumerate
        {
        public:
            using Item = std::pair<size_t, typename Iterator::Item>;

            template <class Iteratorx>
            Enumerate(Iteratorx &&it) :
                it(std::forward<Iteratorx>(it)),
                index(0)
            {
            }

            auto next() -> Option<Item>
            {
                return it.next()
                    .map([this](auto &&value)
                    {
                        return std::pair<size_t, typename Iterator::Item>(index++, std::forward<typename Iterator::Item>(value));
                    });
            }

        private:
            Iterator it;
            size_t index;
        };

        class EnumerateOp
        {
        public:
            template <class Iterator>
            auto operator()(Iterator &&it) && -> Enumerate<Iterator>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "enumerate can only take rvalue ref object with Iterator traits");
                return Enumerate<Iterator>(std::move(it));
            }
        };
    }

    inline auto enumerate() -> details::EnumerateOp
    {
        return details::EnumerateOp();
    }
}
