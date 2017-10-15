/**
 * Contains forwarding declaration for Option.
 * Only for use by Result.
 * Rust Option: https://doc.rust-lang.org/std/option/enum.Option.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "specs.h"
#include "traits.h"

namespace rustfp {

    // forwarding section

    template <class T>
    class Option;

    struct none_t {};
    const none_t None{};

    template <class T>
    RUSTFP_CONSTEXPR auto Some(T &&value)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_constructible<
                Option<special_decay_t<T>>>::value)
        -> Option<special_decay_t<T>>;
}
