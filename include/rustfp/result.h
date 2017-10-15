/**
 * Contains most of the Rust Result enum implementation.
 * Rust Result: https://doc.rust-lang.org/std/result/enum.Result.html
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "mpark/variant.hpp"

#include "option_fwd.h"
#include "result_fwd.h"
#include "specs.h"
#include "traits.h"
#include "unit.h"

#include <cassert>
#include <type_traits>
#include <utility>

namespace rustfp {

    // declaration section

    /**
     * Simulates the Rust Result enum.
     *
     * Wraps either a valid or error value/reference and
     * provides various monadic operations on the wrapped item.
     * @param T Ok item type to wrap over
     * @param E Err item type to wrap over
     */
    template <class T, class E>
    class Result {
    private:
        using variant_t = mpark::variant<
            details::OkImpl<T>,
            details::ErrImpl<E>>;

    public:
        /** Alias to the Ok/valid item type to be wrapped. ok_t == T. */
        using ok_t = T;

        /** Alias to the Err/error item type to be wrapped. err_t == E. */
        using err_t = E;

        template <class Tx>
        RUSTFP_CONSTEXPR Result(details::OkImpl<Tx> &&value)
            RUSTFP_NOEXCEPT_EXPR(
                std::is_nothrow_move_constructible<variant_t>::value);

        template <class Ex>
        RUSTFP_CONSTEXPR Result(details::ErrImpl<Ex> &&err)
            RUSTFP_NOEXCEPT_EXPR(
                std::is_nothrow_move_constructible<variant_t>::value);

        template <class Tx>
        RUSTFP_CONSTEXPR auto operator=(details::OkImpl<Tx> &&value)
            RUSTFP_NOEXCEPT_EXPR(
                std::is_nothrow_move_assignable<variant_t>::value)
            -> Result<T, E> &;

        template <class Ex>
        RUSTFP_CONSTEXPR auto operator=(details::ErrImpl<Ex> &&err)
            RUSTFP_NOEXCEPT_EXPR(
                std::is_nothrow_move_assignable<variant_t>::value)
            -> Result<T, E> &;

        /**
         * https://doc.rust-lang.org/std/result/enum.Result.html#method.map
         *
         * fn map<U, F>(self, op: F) -> Result<U, E> 
         * where
         *     F: FnOnce(T) -> U, 
         */
        template <class F>
        auto map(F &&op) && ->
            Result<special_decay_t<std::result_of_t<F(T &&)>>, E>;

        /**
         * https://doc.rust-lang.org/std/result/enum.Result.html#method.map_err
         *
         * fn map_err<F, O>(self, op: O) -> Result<T, F> 
         * where
         *     O: FnOnce(E) -> F, 
         */
        template <class O>
        auto map_err(O &&op) && ->
            Result<T, special_decay_t<std::result_of_t<O(E &&)>>>;

        /**
         * https://doc.rust-lang.org/std/result/enum.Result.html#method.and_then
         *
         * fn and_then<U, F>(self, op: F) -> Result<U, E> 
         * where
         *     F: FnOnce(T) -> Result<U, E>, 
         */
        template <class F>
        auto and_then(F &&op) &&
            -> Result<typename std::result_of_t<F(T &&)>::ok_t, E>;

        /**
         * https://doc.rust-lang.org/std/result/enum.Result.html#method.or_else
         *
         * fn or_else<F, O>(self, op: O) -> Result<T, F> 
         * where
         *     O: FnOnce(E) -> Result<T, F>, 
         */
        template <class O>
        auto or_else(O &&op) &&
            -> Result<T, typename std::result_of_t<O(E &&)>::err_t>;

        auto ok() && -> Option<T>;

        auto err() && -> Option<E>;

        RUSTFP_CONSTEXPR auto is_ok() const RUSTFP_NOEXCEPT -> bool;

        RUSTFP_CONSTEXPR auto is_err() const RUSTFP_NOEXCEPT -> bool;

        auto unwrap_unchecked() && -> T;

        auto unwrap_err_unchecked() && -> E;

        RUSTFP_CONSTEXPR auto get_unchecked() const RUSTFP_NOEXCEPT
            -> const T &;

        RUSTFP_CONSTEXPR auto get_err_unchecked() const RUSTFP_NOEXCEPT
            -> const E &;

        template <class OkFn, class ErrFn> 
        auto match(OkFn &&ok_fn, ErrFn &&err_fn) &&
            -> std::common_type_t<
                std::result_of_t<OkFn(ok_t)>,
                std::result_of_t<ErrFn(err_t)>>;

        template <class OkFn, class ErrFn> 
        auto match(OkFn &&ok_fn, ErrFn &&err_fn) const &
            -> std::common_type_t<
                std::result_of_t<OkFn(const ok_t &)>,
                std::result_of_t<ErrFn(const err_t &)>>;

        template <class OkFn>
        auto match_ok(OkFn &&ok_fn) && -> unit_t;

        template <class OkFn>
        auto match_ok(OkFn &&ok_fn) const & -> unit_t;

        template <class ErrFn>
        auto match_err(ErrFn &&err_fn) && -> unit_t;

        template <class ErrFn>
        auto match_err(ErrFn &&err_fn) const & -> unit_t;

    private:
        variant_t value_err;
    };

    template <class T>
    RUSTFP_CONSTEXPR auto Ok(T &&value)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_constructible<
                details::OkImpl<special_decay_t<T>>>::value)
        -> details::OkImpl<special_decay_t<T>>;

    template <class E>
    RUSTFP_CONSTEXPR auto Err(E &&error)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_constructible<
                details::ErrImpl<special_decay_t<E>>>::value)
        -> details::ErrImpl<special_decay_t<E>>;

    template <class OkFn, class ErrFn>
    auto res_if_else(const bool cond, OkFn &&ok_fn, ErrFn &&err_fn)
        -> Result<std::result_of_t<OkFn()>, std::result_of_t<ErrFn()>>;

    // implementation section

    namespace details {
        template <class T>
        class OkImpl {
            template <class Tx, class Ex>
            friend class Result;

        public:
            template <class Tx>
            RUSTFP_CONSTEXPR explicit OkImpl(Tx &&value)
                RUSTFP_NOEXCEPT_EXPR(
                    std::is_nothrow_constructible<reverse_decay_t<T>>::value) :
                value(std::forward<Tx>(value)) {

            }

            inline RUSTFP_CONSTEXPR auto get() const RUSTFP_NOEXCEPT
                -> const T & {
                
                return value;
            }

            inline RUSTFP_CONSTEXPR auto move() &&
                RUSTFP_NOEXCEPT_EXPR(
                    std::is_nothrow_constructible<reverse_decay_t<T>>::value)
                -> reverse_decay_t<T> {

                return std::move(value);
            }

        private:
            reverse_decay_t<T> value;
        };

        template <class E>
        class ErrImpl {
            template <class Tx, class Ex>
            friend class Result;

        public:
            template <class Ex>
            RUSTFP_CONSTEXPR explicit ErrImpl(Ex &&err)
                RUSTFP_NOEXCEPT_EXPR(
                    std::is_nothrow_constructible<reverse_decay_t<E>>::value) :
                err(std::forward<Ex>(err)) {

            }

            inline RUSTFP_CONSTEXPR auto get() const RUSTFP_NOEXCEPT
                -> const E & {

                return err;
            }

            inline RUSTFP_CONSTEXPR auto move() &&
                RUSTFP_NOEXCEPT_EXPR(
                    std::is_nothrow_constructible<reverse_decay_t<E>>::value)
                -> reverse_decay_t<E> {

                return std::move(err);
            }
            
        private:
            reverse_decay_t<E> err;
        };

        template <class T, class E>
        RUSTFP_CONSTEXPR auto get_unchecked(
            const mpark::variant<
                details::OkImpl<T>,
                details::ErrImpl<E>> &value_err) RUSTFP_NOEXCEPT -> const T & {

            return mpark::get_if<details::OkImpl<T>>(&value_err)->get();
        }

        template <class T, class E>
        RUSTFP_CONSTEXPR auto get_err_unchecked(
            const mpark::variant<
                details::OkImpl<T>,
                details::ErrImpl<E>> &value_err) RUSTFP_NOEXCEPT -> const E & {

            return mpark::get_if<details::ErrImpl<E>>(&value_err)->get();
        }

        template <class T, class E>
        auto move_unchecked(
            mpark::variant<
                details::OkImpl<T>,
                details::ErrImpl<E>> &&value_err) -> reverse_decay_t<T> {

            return std::move(
                *mpark::get_if<details::OkImpl<T>>(&value_err)).move();
        }

        template <class T, class E>
        auto move_err_unchecked(
            mpark::variant<
                details::OkImpl<T>,
                details::ErrImpl<E>> &&value_err) -> reverse_decay_t<E> {

            return std::move(
                *mpark::get_if<details::ErrImpl<E>>(&value_err)).move();
        }
    }

    template <class T, class E>
    template <class Tx>
    RUSTFP_CONSTEXPR Result<T, E>::Result(details::OkImpl<Tx> &&value)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_move_constructible<variant_t>::value) :

        value_err(std::move(value)) {

    }

    template <class T, class E>
    template <class Ex>
    RUSTFP_CONSTEXPR Result<T, E>::Result(details::ErrImpl<Ex> &&err)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_move_constructible<variant_t>::value) :

        value_err(std::move(err)) {

    }

    template <class T, class E>
    template <class Tx>
    RUSTFP_CONSTEXPR auto Result<T, E>::operator=(details::OkImpl<Tx> &&value)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_move_assignable<variant_t>::value)
        -> Result<T, E> & {

        value_err = std::move(value);
        return *this;
    }

    template <class T, class E>
    template <class Ex>
    RUSTFP_CONSTEXPR auto Result<T, E>::operator=(details::ErrImpl<Ex> &&err)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_move_assignable<variant_t>::value)
        -> Result<T, E> & {

        value_err = std::move(err);
        return *this;
    }

    template <class T, class E>
    template <class F>
    auto Result<T, E>::map(F &&op) && ->
        Result<special_decay_t<std::result_of_t<F(T &&)>>, E> {

        if (is_ok()) {
            return Ok(op(details::move_unchecked(std::move(value_err))));
        } else {
            return Err(details::move_err_unchecked(std::move(value_err)));
        }
    }

    template <class T, class E>
    template <class O>
    auto Result<T, E>::map_err(O &&op) && ->
        Result<T, special_decay_t<std::result_of_t<O(E &&)>>> {

        if (is_err()) {
            return Err(op(details::move_err_unchecked(std::move(value_err))));
        } else {
            return Ok(details::move_unchecked(std::move(value_err)));
        }
    }

    template <class T, class E>
    template <class F>
    auto Result<T, E>::and_then(F &&op) &&
        -> Result<typename std::result_of_t<F(T &&)>::ok_t, E> {

        if (is_ok()) {
            return op(details::move_unchecked(std::move(value_err)));
        } else {
            return Err(details::move_err_unchecked(std::move(value_err)));
        }
    }

    template <class T, class E>
    template <class O>
    auto Result<T, E>::or_else(O &&op) &&
        -> Result<T, typename std::result_of_t<O(E &&)>::err_t> {

        if (is_err()) {
            return op(details::move_err_unchecked(std::move(value_err)));
        } else {
            return Ok(details::move_unchecked(std::move(value_err)));
        }
    }

    template <class T, class E>
    auto Result<T, E>::ok() && -> Option<T> {
        return is_ok()
            ? Some(details::move_unchecked(std::move(value_err)))
            : None;
    }

    template <class T, class E>
    auto Result<T, E>::err() && -> Option<E> {
        return is_err()
            ? Some(details::move_err_unchecked(std::move(value_err)))
            : None;
    }

    template <class T, class E>
    RUSTFP_CONSTEXPR auto Result<T, E>::is_ok() const RUSTFP_NOEXCEPT -> bool {
        return value_err.index() == 0;
    }

    template <class T, class E>
    RUSTFP_CONSTEXPR auto Result<T, E>::is_err() const RUSTFP_NOEXCEPT -> bool {
        return value_err.index() == 1;
    }

    template <class T, class E>
    RUSTFP_CONSTEXPR auto Result<T, E>::get_unchecked() const RUSTFP_NOEXCEPT
        -> const T & {

        assert(is_ok());
        return details::get_unchecked(value_err);
    }

    template <class T, class E>
    RUSTFP_CONSTEXPR auto Result<T, E>::get_err_unchecked()
        const RUSTFP_NOEXCEPT
        -> const E & {

        assert(is_err());
        return details::get_err_unchecked(value_err);
    }

    template <class T, class E>
    auto Result<T, E>::unwrap_unchecked() && -> T {
        assert(is_ok());

        // reference_wrapper can be implicitly converted to direct reference
        // if T is a reference type
        return details::move_unchecked(std::move(value_err));
    }

    template <class T, class E>
    auto Result<T, E>::unwrap_err_unchecked() && -> E {
        assert(is_err());

        // reference_wrapper can be implicitly converted to direct reference
        // if E is a reference type
        return details::move_err_unchecked(std::move(value_err));
    }

    template <class T, class E>
    template <class OkFn, class ErrFn> 
    auto Result<T, E>::match(OkFn &&ok_fn, ErrFn &&err_fn) &&
        -> std::common_type_t<
            std::result_of_t<OkFn(ok_t)>,
            std::result_of_t<ErrFn(err_t)>> {

        return is_ok()
            ? ok_fn(details::move_unchecked(std::move(value_err)))
            : err_fn(details::move_err_unchecked(std::move(value_err)));
    }

    template <class T, class E>
    template <class OkFn, class ErrFn> 
    auto Result<T, E>::match(OkFn &&ok_fn, ErrFn &&err_fn) const &
        -> std::common_type_t<
            std::result_of_t<OkFn(const ok_t &)>,
            std::result_of_t<ErrFn(const err_t &)>> {

        return is_ok()
            ? ok_fn(details::get_unchecked(value_err))
            : err_fn(details::get_err_unchecked(value_err));
    }

    template <class T, class E>
    template <class OkFn>
    auto Result<T, E>::match_ok(OkFn &&ok_fn) && -> unit_t {
        if (is_ok()) {
            ok_fn(details::move_unchecked(std::move(value_err)));
        }

        return Unit;
    }

    template <class T, class E>
    template <class OkFn>
    auto Result<T, E>::match_ok(OkFn &&ok_fn) const & -> unit_t {
        if (is_ok()) {
            ok_fn(details::get_unchecked(value_err));
        }

        return Unit;
    }

    template <class T, class E>
    template <class ErrFn>
    auto Result<T, E>::match_err(ErrFn &&err_fn) && -> unit_t {
        if (is_err()) {
            err_fn(details::move_err_unchecked(std::move(value_err)));
        }
        
        return Unit;
    }

    template <class T, class E>
    template <class ErrFn>
    auto Result<T, E>::match_err(ErrFn &&err_fn) const & -> unit_t {
        if (is_err()) {
            err_fn(details::get_err_unchecked(value_err));
        }
        
        return Unit;
    }

    template <class T>
    RUSTFP_CONSTEXPR auto Ok(T &&value)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_constructible<
                details::OkImpl<special_decay_t<T>>>::value)
        -> details::OkImpl<special_decay_t<T>> {

        return details::OkImpl<special_decay_t<T>>(std::forward<T>(value));
    }

    template <class E>
    RUSTFP_CONSTEXPR auto Err(E &&error)
        RUSTFP_NOEXCEPT_EXPR(
            std::is_nothrow_constructible<
                details::ErrImpl<special_decay_t<E>>>::value)
        -> details::ErrImpl<special_decay_t<E>> {

        return details::ErrImpl<special_decay_t<E>>(std::forward<E>(error));
    }

    template <class OkFn, class ErrFn>
    auto res_if_else(const bool cond, OkFn &&ok_fn, ErrFn &&err_fn)
        -> Result<std::result_of_t<OkFn()>, std::result_of_t<ErrFn()>> {

        if (cond) {
            return Ok(ok_fn());
        } else {
            return Err(err_fn());
        }
    }
}

#include "option.h"
