/**
 * Contains most of the Rust Result enum implementation.
 *
 * Rust Result: https://doc.rust-lang.org/std/result/enum.Result.html
 *
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
 * Wraps either a valid or error value/reference and provides various monadic
 * operations on the wrapped item.
 * @param T Ok item type to wrap over
 * @param E Err item type to wrap over
 */
template <class T, class E>
class Result {
private:
    using variant_t = mpark::variant<OkImpl<T>, ErrImpl<E>>;

public:
    /**
     * Alias to the Ok/valid item type to be wrapped. ok_t == T.
     */
    using ok_t = T;

    /**
     * Alias to the Err/error item type to be wrapped. err_t == E.
     */
    using err_t = E;

    template <class Tx>
    RUSTFP_CONSTEXPR Result(OkImpl<Tx> &&value) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_move_constructible<variant_t>::value);

    template <class Ex>
    RUSTFP_CONSTEXPR Result(ErrImpl<Ex> &&err) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_move_constructible<variant_t>::value);

    template <class Tx>
    RUSTFP_CONSTEXPR auto operator=(OkImpl<Tx> &&value)
        RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_assignable<variant_t>::value)
            -> Result<T, E> &;

    template <class Ex>
    RUSTFP_CONSTEXPR auto operator=(ErrImpl<Ex> &&err)
        RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_assignable<variant_t>::value)
            -> Result<T, E> &;

    auto unwrap_unchecked() && -> T;

    auto unwrap_err_unchecked() && -> E;

    RUSTFP_CONSTEXPR auto get_unchecked() const RUSTFP_NOEXCEPT -> const T &;

    RUSTFP_CONSTEXPR auto get_err_unchecked() const RUSTFP_NOEXCEPT
        -> const E &;

    template <class OkFn, class ErrFn>
    auto match(OkFn &&ok_fn, ErrFn &&err_fn) && -> std::common_type_t<
        std::result_of_t<OkFn(ok_t)>,
        std::result_of_t<ErrFn(err_t)>>;

    template <class OkFn, class ErrFn>
    auto match(OkFn &&ok_fn, ErrFn &&err_fn) const & -> std::common_type_t<
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

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.is_ok
     *
     * fn is_ok(&self) -> bool
     *
     * Returns true if the result is Ok.
     */
    RUSTFP_CONSTEXPR auto is_ok() const RUSTFP_NOEXCEPT -> bool;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.is_err
     *
     * fn is_err(&self) -> bool
     *
     * Returns true if the result is Err.
     */
    RUSTFP_CONSTEXPR auto is_err() const RUSTFP_NOEXCEPT -> bool;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.ok
     *
     * fn ok(self) -> Option<T>
     *
     * Converts from Result<T, E> to Option<T>.
     *
     * Converts self into an Option<T>, consuming self, and discarding the
     * error, if any.
     */
    auto ok() && -> Option<T>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.err
     *
     * fn err(self) -> Option<E>
     *
     * Converts from Result<T, E> to Option<E>.
     *
     * Converts self into an Option<E>, consuming self, and discarding the
     * success value, if any.
     */
    auto err() && -> Option<E>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.map
     *
     * fn map<U, F>(self, op: F) -> Result<U, E>
     * where
     *     F: FnOnce(T) -> U,
     *
     * Maps a Result<T, E> to Result<U, E> by applying a function to a contained
     * Ok value, leaving an Err value untouched.
     *
     * This function can be used to compose the results of two functions.
     */
    template <class F>
    auto
    map(F &&op) && -> Result<special_decay_t<std::result_of_t<F(T &&)>>, E>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.map_err
     *
     * fn map_err<F, O>(self, op: O) -> Result<T, F>
     * where
     *     O: FnOnce(E) -> F,
     *
     * Maps a Result<T, E> to Result<T, F> by applying a function to a contained
     * Err value, leaving an Ok value untouched.
     *
     * This function can be used to pass through a successful result while
     * handling an error.
     */
    template <class O>
    auto
    map_err(O &&op) && -> Result<T, special_decay_t<std::result_of_t<O(E &&)>>>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.and
     *
     * fn and<U>(self, res: Result<U, E>) -> Result<U, E>
     *
     * Returns res if the result is Ok, otherwise returns the Err value of self.
     */
    template <class U>
    auto and_(Result<U, E> &&res) && -> Result<U, E>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.and_then
     *
     * fn and_then<U, F>(self, op: F) -> Result<U, E>
     * where
     *     F: FnOnce(T) -> Result<U, E>,
     *
     * Calls op if the result is Ok, otherwise returns the Err value of self.
     *
     * This function can be used for control flow based on Result values.
     */
    template <class F>
    auto
    and_then(F &&op) && -> Result<typename std::result_of_t<F(T &&)>::ok_t, E>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.or
     *
     * fn or<F>(self, res: Result<T, F>) -> Result<T, F>
     *
     * Returns res if the result is Err, otherwise returns the Ok value of self.
     */
    template <class F>
    auto or_(Result<T, F> &&res) && -> Result<T, F>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.or_else
     *
     * fn or_else<F, O>(self, op: O) -> Result<T, F>
     * where
     *     O: FnOnce(E) -> Result<T, F>,
     *
     * Calls op if the result is Err, otherwise returns the Ok value of self.
     *
     * This function can be used for control flow based on result values.
     */
    template <class O>
    auto
    or_else(O &&op) && -> Result<T, typename std::result_of_t<O(E &&)>::err_t>;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.unwrap_or
     *
     * fn unwrap_or(self, optb: T) -> T
     *
     * Unwraps a result, yielding the content of an Ok. Else, it returns optb.
     */
    template <class Tx>
    auto unwrap_or(Tx &&optb) && -> T;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.unwrap_or_else
     *
     * fn unwrap_or_else<F>(self, op: F) -> T
     * where
     *     F: FnOnce(E) -> T,
     *
     * Unwraps a result, yielding the content of an Ok. If the value is an Err
     * then it calls op with its value.
     */
    template <class F>
    auto unwrap_or_else(F &&op) && -> T;

    /**
     * https://doc.rust-lang.org/std/result/enum.Result.html#method.unwrap_or_default
     *
     * fn unwrap_or_default(self) -> T
     *
     * Returns the contained value or a default
     *
     * Consumes the self argument then, if Ok, returns the contained value,
     * otherwise if Err, returns the default value for that type.
     */
    auto unwrap_or_default() && -> T;

private:
    variant_t value_err;
};

template <class T>
class OkImpl {
    template <class Tx, class Ex>
    friend class Result;

public:
    template <class Tx>
    RUSTFP_CONSTEXPR explicit OkImpl(Tx &&value) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_constructible<reverse_decay_t<T>>::value);

    RUSTFP_CONSTEXPR auto get() const RUSTFP_NOEXCEPT -> const T &;

    RUSTFP_CONSTEXPR auto move()
        && RUSTFP_NOEXCEPT_EXPR(
               std::is_nothrow_constructible<reverse_decay_t<T>>::value)
               -> reverse_decay_t<T>;

private:
    reverse_decay_t<T> value;
};

template <class E>
class ErrImpl {
    template <class Tx, class Ex>
    friend class Result;

public:
    template <class Ex>
    RUSTFP_CONSTEXPR explicit ErrImpl(Ex &&err) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_constructible<reverse_decay_t<E>>::value);

    RUSTFP_CONSTEXPR auto get() const RUSTFP_NOEXCEPT -> const E &;

    RUSTFP_CONSTEXPR auto move()
        && RUSTFP_NOEXCEPT_EXPR(
               std::is_nothrow_constructible<reverse_decay_t<E>>::value)
               -> reverse_decay_t<E>;

private:
    reverse_decay_t<E> err;
};

template <class T>
RUSTFP_CONSTEXPR auto Ok(T &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<OkImpl<special_decay_t<T>>>::value)
    -> OkImpl<special_decay_t<T>>;

template <class E>
RUSTFP_CONSTEXPR auto Err(E &&error) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<ErrImpl<special_decay_t<E>>>::value)
    -> ErrImpl<special_decay_t<E>>;

template <class OkFn, class ErrFn>
auto res_if_else(const bool cond, OkFn &&ok_fn, ErrFn &&err_fn)
    -> Result<std::result_of_t<OkFn()>, std::result_of_t<ErrFn()>>;

// implementation section

namespace details {
template <class T, class E>
RUSTFP_CONSTEXPR auto get_unchecked(
    const mpark::variant<OkImpl<T>, ErrImpl<E>> &value_err) RUSTFP_NOEXCEPT
    -> const T & {

    return mpark::get_if<OkImpl<T>>(&value_err)->get();
}

template <class T, class E>
RUSTFP_CONSTEXPR auto get_err_unchecked(
    const mpark::variant<OkImpl<T>, ErrImpl<E>> &value_err) RUSTFP_NOEXCEPT
    -> const E & {

    return mpark::get_if<ErrImpl<E>>(&value_err)->get();
}

template <class T, class E>
auto move_unchecked(mpark::variant<OkImpl<T>, ErrImpl<E>> &&value_err)
    -> reverse_decay_t<T> {

    return std::move(*mpark::get_if<OkImpl<T>>(&value_err)).move();
}

template <class T, class E>
auto move_err_unchecked(mpark::variant<OkImpl<T>, ErrImpl<E>> &&value_err)
    -> reverse_decay_t<E> {

    return std::move(*mpark::get_if<ErrImpl<E>>(&value_err)).move();
}
} // namespace details

template <class T, class E>
template <class Tx>
RUSTFP_CONSTEXPR Result<T, E>::Result(OkImpl<Tx> &&value)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_constructible<variant_t>::value)
    : value_err(OkImpl<T>(std::move(value).move())) {
}

template <class T, class E>
template <class Ex>
RUSTFP_CONSTEXPR Result<T, E>::Result(ErrImpl<Ex> &&err)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_constructible<variant_t>::value)
    : value_err(ErrImpl<E>(std::move(err).move())) {
}

template <class T, class E>
template <class Tx>
RUSTFP_CONSTEXPR auto Result<T, E>::operator=(OkImpl<Tx> &&value)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_assignable<variant_t>::value)
        -> Result<T, E> & {

    value_err = OkImpl<T>(std::move(value).move());
    return *this;
}

template <class T, class E>
template <class Ex>
RUSTFP_CONSTEXPR auto Result<T, E>::operator=(ErrImpl<Ex> &&err)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_assignable<variant_t>::value)
        -> Result<T, E> & {

    value_err = ErrImpl<E>(std::move(err).move());
    return *this;
}

template <class T, class E>
RUSTFP_CONSTEXPR auto Result<T, E>::get_unchecked() const RUSTFP_NOEXCEPT
    -> const T & {

    assert(is_ok());
    return details::get_unchecked(value_err);
}

template <class T, class E>
RUSTFP_CONSTEXPR auto Result<T, E>::get_err_unchecked() const RUSTFP_NOEXCEPT
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
auto Result<T, E>::match(OkFn &&ok_fn, ErrFn &&err_fn) && -> std::common_type_t<
    std::result_of_t<OkFn(ok_t)>,
    std::result_of_t<ErrFn(err_t)>> {

    return is_ok() ? ok_fn(details::move_unchecked(std::move(value_err)))
                   : err_fn(details::move_err_unchecked(std::move(value_err)));
}

template <class T, class E>
template <class OkFn, class ErrFn>
auto Result<T, E>::match(OkFn &&ok_fn, ErrFn &&err_fn)
    const & -> std::common_type_t<
        std::result_of_t<OkFn(const ok_t &)>,
        std::result_of_t<ErrFn(const err_t &)>> {

    return is_ok() ? ok_fn(details::get_unchecked(value_err))
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

template <class T, class E>
RUSTFP_CONSTEXPR auto Result<T, E>::is_ok() const RUSTFP_NOEXCEPT -> bool {
    return value_err.index() == 0;
}

template <class T, class E>
RUSTFP_CONSTEXPR auto Result<T, E>::is_err() const RUSTFP_NOEXCEPT -> bool {
    return value_err.index() == 1;
}

template <class T, class E>
auto Result<T, E>::ok() && -> Option<T> {
    return is_ok() ? Some(details::move_unchecked(std::move(value_err))) : None;
}

template <class T, class E>
auto Result<T, E>::err() && -> Option<E> {
    return is_err() ? Some(details::move_err_unchecked(std::move(value_err)))
                    : None;
}

template <class T, class E>
template <class F>
auto Result<T, E>::map(
    F &&op) && -> Result<special_decay_t<std::result_of_t<F(T &&)>>, E> {

    if (is_ok()) {
        return Ok(op(details::move_unchecked(std::move(value_err))));
    } else {
        return Err(details::move_err_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class O>
auto Result<T, E>::map_err(
    O &&op) && -> Result<T, special_decay_t<std::result_of_t<O(E &&)>>> {

    if (is_err()) {
        return Err(op(details::move_err_unchecked(std::move(value_err))));
    } else {
        return Ok(details::move_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class U>
auto Result<T, E>::and_(Result<U, E> &&res) && -> Result<U, E> {
    if (is_ok()) {
        return std::move(res);
    } else {
        return Err(details::move_err_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class F>
auto Result<T, E>::and_then(
    F &&op) && -> Result<typename std::result_of_t<F(T &&)>::ok_t, E> {

    if (is_ok()) {
        return op(details::move_unchecked(std::move(value_err)));
    } else {
        return Err(details::move_err_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class F>
auto Result<T, E>::or_(Result<T, F> &&res) && -> Result<T, F> {
    if (is_err()) {
        return std::move(res);
    } else {
        return Ok(details::move_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class O>
auto Result<T, E>::or_else(
    O &&op) && -> Result<T, typename std::result_of_t<O(E &&)>::err_t> {

    if (is_err()) {
        return op(details::move_err_unchecked(std::move(value_err)));
    } else {
        return Ok(details::move_unchecked(std::move(value_err)));
    }
}

template <class T, class E>
template <class Tx>
auto Result<T, E>::unwrap_or(Tx &&optb) && -> T {
    if (is_ok()) {
        return details::move_unchecked(std::move(value_err));
    } else {
        return std::forward<Tx>(optb);
    }
}

template <class T, class E>
template <class F>
auto Result<T, E>::unwrap_or_else(F &&op) && -> T {
    if (is_ok()) {
        return details::move_unchecked(std::move(value_err));
    } else {
        return op();
    }
}

template <class T, class E>
auto Result<T, E>::unwrap_or_default() && -> T {
    if (is_ok()) {
        return details::move_unchecked(std::move(value_err));
    } else {
        return T();
    }
}

template <class T>
template <class Tx>
RUSTFP_CONSTEXPR OkImpl<T>::OkImpl(Tx &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<reverse_decay_t<T>>::value)
    : value(std::forward<Tx>(value)) {
}

template <class T>
RUSTFP_CONSTEXPR auto OkImpl<T>::get() const RUSTFP_NOEXCEPT -> const T & {
    return value;
}

template <class T>
    RUSTFP_CONSTEXPR auto OkImpl<T>::move()
    && RUSTFP_NOEXCEPT_EXPR(
           std::is_nothrow_constructible<reverse_decay_t<T>>::value)
           -> reverse_decay_t<T> {

    return std::move(value);
}

template <class E>
template <class Ex>
RUSTFP_CONSTEXPR ErrImpl<E>::ErrImpl(Ex &&err) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<reverse_decay_t<E>>::value)
    : err(std::forward<Ex>(err)) {
}

template <class E>
RUSTFP_CONSTEXPR auto ErrImpl<E>::get() const RUSTFP_NOEXCEPT -> const E & {
    return err;
}

template <class E>
    RUSTFP_CONSTEXPR auto ErrImpl<E>::move()
    && RUSTFP_NOEXCEPT_EXPR(
           std::is_nothrow_constructible<reverse_decay_t<E>>::value)
           -> reverse_decay_t<E> {

    return std::move(err);
}

template <class T>
RUSTFP_CONSTEXPR auto Ok(T &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<OkImpl<special_decay_t<T>>>::value)
    -> OkImpl<special_decay_t<T>> {

    return OkImpl<special_decay_t<T>>(std::forward<T>(value));
}

template <class E>
RUSTFP_CONSTEXPR auto Err(E &&error) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<ErrImpl<special_decay_t<E>>>::value)
    -> ErrImpl<special_decay_t<E>> {

    return ErrImpl<special_decay_t<E>>(std::forward<E>(error));
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
} // namespace rustfp

#include "option.h"
