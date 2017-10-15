/**
 * Contains functional Unit implementation.
 * Equivalent to Rust () type.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

namespace rustfp {
    /** Describes the unit type, which is denoted by an empty struct. */
    struct unit_t {};

    /** Pre-constructed unit value to use for convenience. */
    constexpr unit_t Unit{};
}
