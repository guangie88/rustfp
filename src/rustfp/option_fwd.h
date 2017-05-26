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
