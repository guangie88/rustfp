/**
 * Contains conversion from std containers/iterator to
 * Rust style iterators so that other operations can be used.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "traits.h"

#include <functional>
#include <iterator>
#include <type_traits>

namespace rustfp {

    // declaration section

    /**
     * Represents rustfp iterator type for generating immutable
     * references to the original values in the container.
     * @tparam StdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     */
    template <class StdInputIterable>
    class Iter {
    public:
        /** Type alias referencing to the const reference type of given container. */
        using Item = simplify_ref_t<typename std::iterator_traits<
            typename StdInputIterable::const_iterator>::reference>;

        static_assert(std::is_lvalue_reference<Item>::value,
            "Iter can only take iterable whose iterator dereferences to a reference type");

        /**
         * Constructor to take in the container value.
         * @param input_iterable Immutable reference of container
         * to accept for generate the items.
         */
        explicit Iter(const StdInputIterable &input_iterable);

        /**
         * Generates the next item from the container.
         * @return Next iteration of item to be generated.
         * Some(Item) if the end of iteration has not been reached,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        std::reference_wrapper<const StdInputIterable> inputIterableRef;
        typename StdInputIterable::const_iterator curr_it;
    };

    /**
     * Represents rustfp iterator type for generating mutable
     * references to the original values in the container.
     * @tparam StdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     */
    template <class StdInputIterable>
    class IterMut {
    public:
        /** Type alias referencing to the reference type of given container. */
        using Item = simplify_ref_t<typename std::iterator_traits<
            typename StdInputIterable::iterator>::reference>;

        static_assert(std::is_lvalue_reference<Item>::value,
            "IterMut can only take iterable whose iterator dereferences to a reference type");

        /**
         * Constructor to take in the container value.
         * @param input_iterable Reference of container
         * to accept for generate the items.
         */
        explicit IterMut(StdInputIterable &input_iterable);

        /**
         * Generates the next item from the container.
         * @return Next iteration of item to be generated.
         * Some(Item) if the end of iteration has not been reached,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        std::reference_wrapper<StdInputIterable> inputIterableRef; 
        typename StdInputIterable::iterator curr_it;
    };

    /**
     * Represents rustfp iterator type for generating moved values
     * from the container.
     * @tparam MovedStdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     */
    template <class MovedStdInputIterable>
    class IntoIter {
    public:
        /** Type alias referencing to the value type of given container. */
        using Item = typename std::iterator_traits<
            typename MovedStdInputIterable::iterator>::value_type;

        static_assert(!std::is_reference<MovedStdInputIterable>::value,
            "IntoIter can only take moved iterable "
            "whose iterator dereferences to a value type");

        /**
         * Constructor to take in the container value.
         * @param input_iterable Moved container
         * to accept for generate the moved items.
         */
        explicit IntoIter(MovedStdInputIterable &&input_iterable);

        /**
         * Generates the next item from the container.
         * @return Next iteration of item to be generated.
         * Some(Item) if the end of iteration has not been reached,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        MovedStdInputIterable input_iterable;
        typename MovedStdInputIterable::iterator curr_it;
    };

    /**
     * Represents rustfp iterator type for generating immutable
     * references to the original values from the std-styled iterators.
     * @tparam StdBeginInputIterator Begin iterator type.
     * @tparam StdEndInputIterator End iterator type.
     */
    template <class StdBeginInputIterator, class StdEndInputIterator>
    class IterBeginEnd {
    public:
        /**
         * Type alias referencing to the const reference type of common type of
         * the iterator types.
         */
        using Item = std::add_const_t<typename std::iterator_traits<
            typename std::common_type_t<
                StdBeginInputIterator,
                StdEndInputIterator>>::value_type> &;

        /**
         * Constructor to take in the pair of std-styled iterators.
         * @param begin_it Begin iterator of the container/array.
         * @param begin_it End iterator of the container/array.
         */
        IterBeginEnd(
            StdBeginInputIterator &&begin_it,
            StdEndInputIterator &&end_it);

        /**
         * Generates the next item from the container.
         * @return Next iteration of item to be generated.
         * Some(Item) if the end of iteration has not been reached,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        StdBeginInputIterator curr_it;
        StdEndInputIterator end_it;
    };

    /**
     * Wraps a container type that has std-styled iterator, into rustfp iterator that
     * generates immutable reference to the original values in the container.
     * @tparam StdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     * @param input_iterable Immutable reference of container
     * to accept for generate the items.
     * @return rustfp iterator to hold the state of both given iterators.
     */
    template <class StdInputIterable>
    auto iter(StdInputIterable &&input_iterable)
        -> Iter<std::remove_reference_t<StdInputIterable>>;

    /**
     * Wraps a container type that has std-styled iterator, into rustfp iterator that
     * generates mutable reference to the original values in the container.
     * @tparam StdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     * @param input_iterable Reference of container
     * to accept for generate the items.
     * @return rustfp iterator to hold the state of both given iterators.
     */
    template <
        class StdInputIterable,
        class = std::enable_if_t<!std::is_const<StdInputIterable>::value>>
    auto iter_mut(StdInputIterable &input_iterable)
        -> IterMut<std::remove_reference_t<StdInputIterable>>;

    /**
     * Wraps a container type that has std-styled iterator, into rustfp iterator that
     * moves each of the original values in the container.
     * @tparam MovedStdInputIterable container type that is able to produce
     * std input iterator for begin and end.
     * @param input_iterable Moved container
     * to accept for generate the moved items.
     * @return rustfp iterator to hold the state of both given iterators.
     */
    template <
        class MovedStdInputIterable,
        class = std::enable_if_t<!std::is_lvalue_reference<MovedStdInputIterable>::value>>
    auto into_iter(MovedStdInputIterable &&moved_input_iterable)
        -> IntoIter<MovedStdInputIterable>;

    /**
     * Wraps a pair of begin and end std-styled iterators, into rustfp iterator
     * that generates immutable reference to the original values in the container.
     * @tparam StdBeginInputIterator Begin iterator type.
     * @tparam StdEndInputIterator End iterator type.
     * @param begin_it Begin iterator of the container/array.
     * @param end_it End iterator of container/array.
     * @return rustfp iterator to hold the state of both given iterators.
     */
    template <class StdBeginInputIterator, class StdEndInputIterator>
    auto iter_begin_end(
        StdBeginInputIterator &&begin_it,
        StdEndInputIterator &&end_it)
        -> IterBeginEnd<
            std::remove_reference_t<StdBeginInputIterator>,
            std::remove_reference_t<StdEndInputIterator>>;


    // implementation section

    namespace details {
        template <class Item, class InputIterable, class InputIterator>
        auto next_impl(InputIterable &input_iterable, InputIterator &curr_it) -> Option<Item> {
            if (curr_it != std::end(input_iterable)) {
                const auto prev_it = curr_it;
                ++curr_it;
                return Some(std::ref(*prev_it));
            } else {
                return None;
            }
        }
    }

    template <class StdInputIterable>
    Iter<StdInputIterable>::Iter(const StdInputIterable &input_iterable) :
        inputIterableRef(input_iterable),
        curr_it(std::cbegin(input_iterable)) {

    }

    template <class StdInputIterable>
    auto Iter<StdInputIterable>::next() -> Option<Item> {
        return details::next_impl<Item>(inputIterableRef.get(), curr_it);
    }

    template <class StdInputIterable>
    IterMut<StdInputIterable>::IterMut(StdInputIterable &input_iterable) :
        inputIterableRef(input_iterable),
        curr_it(std::begin(input_iterable)) {

    }

    template <class StdInputIterable>
    auto IterMut<StdInputIterable>::next() -> Option<Item> {
        return details::next_impl<Item>(inputIterableRef.get(), curr_it);
    }

    template <class MovedStdInputIterable>
    IntoIter<MovedStdInputIterable>::IntoIter(MovedStdInputIterable &&input_iterable) :
        input_iterable(std::move(input_iterable)),
        curr_it(std::begin(this->input_iterable)) {

    }

    template <class MovedStdInputIterable>
    auto IntoIter<MovedStdInputIterable>::next() -> Option<Item> {
        if (curr_it != std::end(input_iterable)) {
            auto prev_it = curr_it;
            ++curr_it;
            return Some(std::move(*prev_it));
        } else {
            return None;
        }
    }

    template <class StdBeginInputIterator, class StdEndInputIterator>
    IterBeginEnd<StdBeginInputIterator, StdEndInputIterator>::IterBeginEnd(
        StdBeginInputIterator &&begin_it,
        StdEndInputIterator &&end_it) :

        curr_it(std::forward<StdBeginInputIterator>(begin_it)),
        end_it(std::forward<StdEndInputIterator>(end_it)) {

    }

    template <class StdBeginInputIterator, class StdEndInputIterator>
    auto IterBeginEnd<StdBeginInputIterator, StdEndInputIterator>::next() -> Option<Item> {
        if (curr_it != end_it) {
            const auto prev_it = curr_it;
            ++curr_it;
            return Some(std::cref(*prev_it));
        } else {
            return None;
        }
    }

    template <class StdInputIterable>
    auto iter(StdInputIterable &&input_iterable)
        -> Iter<std::remove_reference_t<StdInputIterable>> {

        static_assert(std::is_lvalue_reference<StdInputIterable>::value,
            "rustfp::iter() must be invoked on lvalue reference only");

        return Iter<std::remove_reference_t<StdInputIterable>>(input_iterable);
    }

    template <class StdInputIterable, class>
    auto iter_mut(StdInputIterable &input_iterable)
        -> IterMut<std::remove_reference_t<StdInputIterable>> {

        static_assert(!std::is_const<StdInputIterable>::value,
            "rustfp::iter_mut() must be invoked on non-const lvalue reference only");

        return IterMut<std::remove_reference_t<StdInputIterable>>(input_iterable);
    }

    template <class MovedStdInputIterable, class>
    auto into_iter(MovedStdInputIterable &&moved_input_iterable)
        -> IntoIter<MovedStdInputIterable> {

        static_assert(!std::is_reference<MovedStdInputIterable>::value,
            "rustfp::into_iter() must be invoked on rvalue reference only");

        return IntoIter<MovedStdInputIterable>(std::move(moved_input_iterable));
    }

    template <class StdBeginInputIterator, class StdEndInputIterator>
    auto iter_begin_end(
        StdBeginInputIterator &&begin_it,
        StdEndInputIterator &&end_it)
        -> IterBeginEnd<
            std::remove_reference_t<StdBeginInputIterator>,
            std::remove_reference_t<StdEndInputIterator>> {

        return IterBeginEnd<
            std::remove_reference_t<StdBeginInputIterator>,
            std::remove_reference_t<StdEndInputIterator>>(
                std::forward<StdBeginInputIterator>(begin_it),
                std::forward<StdEndInputIterator>(end_it));
    }
}
