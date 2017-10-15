/**
 * Equivalent to Rust range (N..M syntax), but uses start index with length instead.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    /**
     * Iterator structure to hold the indices to be generated.
     * @tparam Index Index type to be used.
     */
    template <class Index>
    class Range {
    public:
        /** Item type to generate */
        using Item = Index;

        /**
         * Initialize the Range Iterator with the start index and count.
         * @param start_index Index value to start from.
         * @param count Number of indices to generate from the start index.
         */
        Range(const Index current_index, const size_t count_left);

        /**
         * Generates the next value of range operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
        auto next() -> Option<Item>;

    private:
        Index current_index;
        size_t count_left;
    };

    /**
     * Creates an Iterator to generate a range of indices.
     * @tparam Index Index type to be used.
     * @param start_index Index value to start from.
     * @param count Number of indices to generate from the start index.
     * @return Iterator to generate the indices.
     */
    template <class Index>
    auto range(const Index start_index, const size_t count) -> Range<Index>;

    // implementation section

    template <class Index>
    Range<Index>::Range(const Index current_index, const size_t count_left) :
        current_index(current_index),
        count_left(count_left) {
    }

    template <class Index>
    auto Range<Index>::next() -> Option<Item> {
        if (count_left > 0) {
            --count_left;
            return Some(current_index++);
        }
        else {
            return None;
        }
    }

    template <class Index>
    auto range(const Index start_index, const size_t count) -> Range<Index> {
        return Range<Index>(start_index, count);
    }
}
