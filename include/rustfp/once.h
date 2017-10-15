/**
 * Contains Rust std::iter::once equivalent implementation
 * once function: https://doc.rust-lang.org/std/iter/fn.once.html
 * Once struct: https://doc.rust-lang.org/std/iter/struct.Once.html
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
     * impl<T> Iterator for Once<T>
     * type Item = T
     */
    template <class T>
    class Once {
    public:
        /** Type alias to rustfp Iter type */
        using Item = T;
        
        /**
         * Takes in any other value other than Once type.
         * This forwarding constructor contains a second special template argument
         * to prevent conflict with the default copy/move constructor.
         * @tparam Tx Other value type other than Once type.
         * @param value other value to assign
         */
        template <class Tx, class =
            std::enable_if_t<!std::is_same<Once<T>, std::remove_const_t<std::remove_reference_t<Tx>>>::value>>
        explicit Once(Tx &&value);

        /**
         * Generates the next value of once operation.
         * @return Some(Item) if there is a next value to generate,
         * otherwise None.
         */
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
    template <class Tx, class>
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
