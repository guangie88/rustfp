#pragma once

#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>
#include <vector>

namespace rustfp
{
    namespace details
    {
        template <class Into>
        class CollectOp;

        template <class T, class Allocator>
        class CollectOp<std::vector<T, Allocator>>
        {
        public:
            template <class Iterator>
            auto operator()(Iterator &&it) && -> std::vector<T, Allocator>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "collect<std::vector<T, Allocator>> can only take rvalue ref object with Iterator traits");

                std::vector<T, Allocator> vec;

                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none())
                    {
                        break;
                    }

                    vec.push_back(next_opt.unwrap_unchecked());
                }

                return vec;
            }
        };
    }

    template <class Into>
    auto collect() -> details::CollectOp<Into>
    {
        return details::CollectOp<Into>();
    }
}