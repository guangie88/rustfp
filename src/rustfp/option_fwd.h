/**
 * Contains forwarding declaration for Option.
 * Only for use by Result.
 * Rust Option: https://doc.rust-lang.org/std/option/enum.Option.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "traits.h"

namespace rustfp {

    // forwarding section

    template <class T>
    class Option;

    struct none_t {};
    const none_t None{};

    template <class T>
    auto Some(T &&value) -> Option<special_decay_t<T>>;
}
