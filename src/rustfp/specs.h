/**
 * Contains noexcept macro to relax the noexcept specifications on MSVC
 * so that it no longer triggers the internal compilation error.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#ifndef RUSTFP_NOEXCEPT
#   ifdef _MSC_VER
        // disable complex noexcept specifications on MSVC to prevent ICE
#       if _MSC_VER >= 1910
#           define RUSTFP_NOEXCEPT_EXPR(expr)
#           define RUSTFP_NOEXCEPT noexcept
#           define RUSTFP_CONSTEXPR constexpr
#       else
#           define RUSTFP_NOEXCEPT_EXPR(expr)
#           define RUSTFP_NOEXCEPT noexcept
#           define RUSTFP_CONSTEXPR
#       endif
#   else
        // g++ and clang have no issues
#       define RUSTFP_NOEXCEPT_EXPR(expr) noexcept(expr)
#       define RUSTFP_NOEXCEPT noexcept
#       define RUSTFP_CONSTEXPR constexpr
#   endif
#endif