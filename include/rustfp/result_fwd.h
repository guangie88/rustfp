/**
 * Contains forwarding declaration for Result. Only for use by Result.
 *
 * Rust Result: https://doc.rust-lang.org/std/result/enum.Result.html
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "specs.h"
#include "traits.h"

namespace rustfp {

// forwarding section

template <class T>
class OkImpl;

template <class E>
class ErrImpl;

template <class T, class E>
class Result;

template <class T>
RUSTFP_CONSTEXPR auto Ok(T &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<OkImpl<special_decay_t<T>>>::value)
    -> OkImpl<special_decay_t<T>>;

template <class E>
RUSTFP_CONSTEXPR auto Err(E &&error) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<ErrImpl<special_decay_t<E>>>::value)
    -> ErrImpl<special_decay_t<E>>;
} // namespace rustfp
