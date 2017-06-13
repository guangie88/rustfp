#pragma once

#include "option.h"
#include "traits.h"

#include <functional>
#include <iterator>
#include <type_traits>

namespace rustfp {

    // declaration section

    namespace details {
        template <class StdInputIterable>
        class Iter;

        template <class StdInputIterable>
        class IterMut;

        template <class MovedStdInputIterable>
        class IntoIter;
    }

    /**
     * Wraps a C++ STL type that has iterator, into rustfp iterator that
     * generates immutable reference to the original values in the STL container.
     */
    template <class StdInputIterable>
    auto iter(StdInputIterable &&inputIterable)
        -> details::Iter<std::remove_reference_t<StdInputIterable>>;

    /**
     * Wraps a C++ STL type that has iterator, into rustfp iterator that
     * generates mutable reference to the original values in the STL container.
     */
    template <
        class StdInputIterable,
        class = std::enable_if_t<!std::is_const<StdInputIterable>::value>>
    auto iter_mut(StdInputIterable &inputIterable)
        -> details::IterMut<std::remove_reference_t<StdInputIterable>>;

    /**
     * Wraps a C++ STL type that has iterator, into rustfp iterator that
     * moves each of the original values in the STL container.
     */
    template <
        class MovedStdInputIterable,
        class = std::enable_if_t<!std::is_lvalue_reference<MovedStdInputIterable>::value>>
    auto into_iter(MovedStdInputIterable &&movedInputIterable)
        -> details::IntoIter<MovedStdInputIterable>;

    // implementation section

    namespace details {
        template <class Item, class InputIterable, class InputIterator>
        auto next_impl(InputIterable &inputIterable, InputIterator &curr_it) -> Option<Item> {
            if (curr_it != std::end(inputIterable)) {
                const auto prev_it = curr_it;
                ++curr_it;
                return Some(std::ref(*prev_it));
            } else {
                return None;
            }
        }

        template <class StdInputIterable>
        class Iter {
        public:
            using Item = simplify_ref_t<typename std::iterator_traits<
                typename StdInputIterable::const_iterator>::reference>;

            static_assert(std::is_lvalue_reference<Item>::value,
                "Iter can only take iterable whose iterator dereferences to a reference type");

            Iter(const StdInputIterable &inputIterable) :
                inputIterableRef(inputIterable),
                curr_it(std::cbegin(inputIterable)) {

            }

            auto next() -> Option<Item> {
                return next_impl<Item>(inputIterableRef.get(), curr_it);
            }

        private:
            std::reference_wrapper<const StdInputIterable> inputIterableRef; 
            typename StdInputIterable::const_iterator curr_it;
        };

        template <class StdInputIterable>
        class IterMut {
        public:
            using Item = simplify_ref_t<typename std::iterator_traits<
                typename StdInputIterable::iterator>::reference>;

            static_assert(std::is_lvalue_reference<Item>::value,
                "IterMut can only take iterable whose iterator dereferences to a reference type");

            IterMut(StdInputIterable &inputIterable) :
                inputIterableRef(inputIterable),
                curr_it(std::begin(inputIterable)) {

            }

            auto next() -> Option<Item> {
                return next_impl<Item>(inputIterableRef.get(), curr_it);
            }

        private:
            std::reference_wrapper<StdInputIterable> inputIterableRef; 
            typename StdInputIterable::iterator curr_it;
        };

        template <class MovedStdInputIterable>
        class IntoIter {
        public:
            using Item = typename std::iterator_traits<
                typename MovedStdInputIterable::iterator>::value_type;

            static_assert(!std::is_reference<MovedStdInputIterable>::value,
                "IntoIter can only take moved iterable "
                "whose iterator dereferences to a value type");

            IntoIter(MovedStdInputIterable &&inputIterable) :
                inputIterable(std::move(inputIterable)),
                curr_it(std::begin(this->inputIterable)) {

            }

            auto next() -> Option<Item> {
                if (curr_it != std::end(inputIterable)) {
                    auto prev_it = curr_it;
                    ++curr_it;
                    return Some(std::move(*prev_it));
                } else {
                    return None;
                }
            }

        private:
            MovedStdInputIterable inputIterable;
            typename MovedStdInputIterable::iterator curr_it;
        };
    }

    template <class StdInputIterable>
    auto iter(StdInputIterable &&inputIterable)
        -> details::Iter<std::remove_reference_t<StdInputIterable>> {

        static_assert(std::is_lvalue_reference<StdInputIterable>::value,
            "rustfp::iter() must be invoked on lvalue reference only");

        return details::Iter<std::remove_reference_t<StdInputIterable>>(inputIterable);
    }

    template <class StdInputIterable, class>
    auto iter_mut(StdInputIterable &inputIterable)
        -> details::IterMut<std::remove_reference_t<StdInputIterable>> {

        static_assert(!std::is_const<StdInputIterable>::value,
            "rustfp::iter_mut() must be invoked on non-const lvalue reference only");

        return details::IterMut<std::remove_reference_t<StdInputIterable>>(inputIterable);
    }

    template <class MovedStdInputIterable, class>
    auto into_iter(MovedStdInputIterable &&movedInputIterable)
        -> details::IntoIter<MovedStdInputIterable> {

        static_assert(!std::is_reference<MovedStdInputIterable>::value,
            "rustfp::into_iter() must be invoked on rvalue reference only");

        return details::IntoIter<MovedStdInputIterable>(std::move(movedInputIterable));
    }
}
