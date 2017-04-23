#pragma once

#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class IntStart>
        class Range
        {
        public:
            using Item = IntStart;

            Range(const IntStart current_index, const size_t count_left) :
                current_index(current_index),
                count_left(count_left)
            {
            }

            auto next() -> Option<Item>
            {
                return count_left > 0
                    ? [this]
                    {
                        --count_left;
                        return Some(current_index++);
                    }()

                    : None;
            }

        private:
            IntStart current_index;
            size_t count_left;
        };
    }

    template <class IntStart>
    auto range(const IntStart start_index, const size_t count) -> details::Range<IntStart>
    {
        return details::Range<IntStart>(start_index, count);
    }
}