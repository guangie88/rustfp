#pragma once

#include <utility>

#include "result.h"
#include "traits.h"

#define __RUSTFP_RET_IF_ERR(res) \
    if (res.is_err()) { \
        return ::rustfp::Err(::rustfp::reverse_decay(::std::move(res).unwrap_err_unchecked())); \
    }

#define __RUSTFP_LET_IMPL(decl, var, res) \
    __RUSTFP_RET_IF_ERR(res) \
    decl var = ::std::move(res).unwrap_unchecked();

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
