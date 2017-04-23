#pragma once

#include "option.h"

#include <functional>
#include <iterator>
#include <type_traits>

namespace rustfp
{
    namespace details
    {
        template <class StdInputIterable>
        class Iter
        {
        public:
            using Item = typename std::iterator_traits<typename StdInputIterable::const_iterator>::reference;
            static_assert(std::is_lvalue_reference<Item>::value, "Iter can only take iterable whose iterator dereferences to a reference type");

            Iter(const StdInputIterable &inputIterable) :
                inputIterableRef(inputIterable),
                currIt(std::cbegin(inputIterable))
            {
            }

            auto next() -> Option<Item>
            {
                return currIt != std::cend(inputIterableRef.get())
                    ? [this]
                    {
                        const auto prevIt = currIt;
                        ++currIt;
                        return Some(std::cref(*prevIt));
                    }()

                    : None;
            }

        private:
            std::reference_wrapper<const StdInputIterable> inputIterableRef; 
            typename StdInputIterable::const_iterator currIt;
        };

        template <class StdInputIterable>
        class IterMut;

        template <class MovedStdInputIterable>
        class IntoIter;
    }

    template <class StdInputIterable>
    auto iter(StdInputIterable &&inputIterable) -> details::Iter<std::remove_reference_t<StdInputIterable>>
    {
        static_assert(std::is_lvalue_reference<StdInputIterable>::value, "rustfp::iter() must be invoked on lvalue reference only");
        return details::Iter<std::remove_reference_t<StdInputIterable>>(inputIterable);
    }

    template <class StdInputIterable, class = std::enable_if_t<!std::is_const<StdInputIterable>::value>>
    auto iter_mut(StdInputIterable &inputIterable)
    {
        static_assert(!std::is_const<StdInputIterable>::value, "rustfp::iter_mut() must be invoked on non-const lvalue reference only");
    }

    template <class MovedStdInputIterable, class = std::enable_if_t<!std::is_lvalue_reference<MovedStdInputIterable>::value>>
    auto into_iter(MovedStdInputIterable &&movedInputIterable);
}
