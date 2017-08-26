/**
 * Contains Rust std::iter::once equivalent implementation
 * once function: https://doc.rust-lang.org/std/iter/fn.once.html
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

    template <class T>
    class Once {
    public:
        using Item = T;

        template <class Tx>
        Once(Tx &&value);

        auto next() -> Option<Item>;

    private:
        Option<T> value;
    };

    /**
     * pub fn once<T>(value: T) -> Once<T>
     */
    template <class T>
    auto once(T &&value) -> Once<special_decay_t<T>>;

    // implementation section

    template <class T>
    template <class Tx>
    Once<T>::Once(Tx &&value) :
        value(Some(std::forward<Tx>(value))) {

    }

    template <class T>
    auto Once<T>::next() -> Option<Item> {
        return std::move(value);
    }

    template <class T>
    auto once(T &&value) -> Once<special_decay_t<T>> {
        return Once<special_decay_t<T>>(std::forward<T>(value));
    }
}
