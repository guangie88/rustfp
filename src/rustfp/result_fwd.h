/**
 * Contains forwarding declaration for Result.
 * Only for use by Result.
 * Rust Result: https://doc.rust-lang.org/std/result/enum.Result.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "traits.h"

namespace rustfp {

    // forwarding section

    namespace details {
        template <class T>
        class OkImpl;

        template <class E>
        class ErrImpl;
    }

    template <class T, class E>
    class Result;

    template <class T>
    constexpr auto Ok(T &&value)
        noexcept(
            std::is_nothrow_move_constructible<
                details::OkImpl<special_decay_t<T>>>::value &&
            std::is_nothrow_move_assignable<T>::value)
        -> details::OkImpl<special_decay_t<T>>;

    template <class E>
    constexpr auto Err(E &&error)
        noexcept(
            std::is_nothrow_move_constructible<
                details::ErrImpl<special_decay_t<E>>>::value &&
            std::is_nothrow_move_assignable<E>::value)
        -> details::ErrImpl<special_decay_t<E>>;
}
