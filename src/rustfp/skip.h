#pragma once

#include "option.h"
#include "util.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class Iterator>
        class Skip
        {
        public:
            using Item = typename Iterator::Item;

            template <class Iteratorx>
            Skip(Iteratorx &&it, const size_t count) :
                it(std::forward<Iteratorx>(it)),
                count(count)
            {
            }

            auto next() -> Option<Item>
            {
                while (count > 0)
                {
                    it.next();
                    --count;
                }

                return it.next();
            }

        private:
            Iterator it;
            size_t count;
        };

        class SkipOp
        {
        public:
            explicit SkipOp(const size_t count) :
                count(count)
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Skip<Iterator>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "skip can only take rvalue ref object with Iterator traits");
                return Skip<Iterator>(std::move(it), count);
            }

        private:
            size_t count;
        };
    }

    inline auto skip(const size_t count) -> details::SkipOp
    {
        return details::SkipOp(count);
    }
}
