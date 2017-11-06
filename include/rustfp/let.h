/**
 * Contains C-style macros to perform fast failing with syntactical convenience.
 * Similar to the try-catch / ? operator in Rust.
 *
 * Rust try-catch: https://doc.rust-lang.org/std/macro.try.html
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include <utility>

#include "result.h"
#include "traits.h"

#define __RUSTFP_RET_IF_ERR_IMPL(r)                                            \
    if (r.is_err()) {                                                          \
        return ::rustfp::Err(                                                  \
            ::rustfp::reverse_decay(::std::move(r).unwrap_err_unchecked()));   \
    }

#define __RUSTFP_RET_IF_ERR(res)                                               \
    auto __##ret_if_err_val##__##__LINE__ = ::std::move(res);                  \
    __RUSTFP_RET_IF_ERR_IMPL(__##ret_if_err_val##__##__LINE__)

#define __RUSTFP_LET_IMPL(decl, var, res)                                      \
    auto __##var##__##__LINE__ = ::std::move(res);                             \
    __RUSTFP_RET_IF_ERR_IMPL(__##var##__##__LINE__)                            \
    decl var = ::std::move(__##var##__##__LINE__).unwrap_unchecked();

#define RUSTFP_RET_IF_ERR __RUSTFP_RET_IF_ERR
#define RUSTFP_LET(var, res) __RUSTFP_LET_IMPL(const auto, var, res)
#define RUSTFP_LET_MUT(var, res) __RUSTFP_LET_IMPL(auto, var, res)
#define RUSTFP_LET_REF(var, res) __RUSTFP_LET_IMPL(const auto &, var, res)
#define RUSTFP_LET_MUT_REF(var, res) __RUSTFP_LET_IMPL(auto &, var, res)

#ifdef RUSTFP_SIMPLIFIED_LET
#define ret_if_err RUSTFP_RET_IF_ERR
#define let RUSTFP_LET
#define let_mut RUSTFP_LET_MUT
#define let_ref RUSTFP_LET_REF
#define let_mut_ref RUSTFP_LET_MUT_REF
#endif
