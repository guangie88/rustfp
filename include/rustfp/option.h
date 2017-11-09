/**
 * Contains most of the Rust Option enum implementation.
 *
 * Rust Option: https://doc.rust-lang.org/std/option/enum.Option.html
 *
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "option_fwd.h"
#include "result_fwd.h"
#include "traits.h"
#include "unit.h"

#include "nonstd/optional.hpp"

#include <type_traits>
#include <utility>

namespace rustfp {

// declaration section

namespace details {
template <class T>
class SomeImpl;
}

/**
 * Simulates the Rust Option enum. Wraps a value or reference in a nullable
 * wrapper and provides various monadic operations on the wrapped item.
 * @param T item type to wrap over
 */
template <class T>
class Option {
    /**
     * Allows all other Option specializations to have access to all the fields
     * of this Option type.
     */
    template <class Tx>
    friend class Option;

private:
    using optional_t = nonstd::optional_lite::optional<reverse_decay_t<T>>;

public:
    /**
     * Alias to the item type to be wrapped. some_t == T.
     */
    using some_t = T;

    /**
     * Constructor to take in the Some wrapped item and the constructed
     * instance is_some() will return true.
     * @param value Some wrapped item to be passed into the Option wrapper.
     * The item must be convertible to T.
     * @see is_some
     */
    template <class Tx>
    RUSTFP_CONSTEXPR Option(details::SomeImpl<Tx> &&value) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_move_constructible<optional_t>::value);

    /**
     * Copy constructor to target RHS Option.
     * @param RHS option to be copy constructed from.
     */
    RUSTFP_CONSTEXPR Option(const Option<T> &rhs) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_copy_constructible<optional_t>::value);

    /**
     * Copy constructor template to target RHS Option.
     * @param RHS option to be copy constructed from. RHS Tx item type must
     * be convertible to T.
     */
    template <class Tx>
    RUSTFP_CONSTEXPR Option(const Option<Tx> &rhs) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_copy_constructible<optional_t>::value);

    /**
     * Move constructor to target RHS moving Option.
     * @param RHS option to be move constructed from.
     */
    RUSTFP_CONSTEXPR Option(Option<T> &&rhs) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_move_constructible<optional_t>::value);

    /**
     * Move constructor template to target RHS moving Option.
     * @param RHS option to be move constructed from. RHS Tx item type must
     * be convertible to T.
     */
    template <class Tx>
    RUSTFP_CONSTEXPR Option(Option<Tx> &&rhs) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_move_constructible<optional_t>::value);

    /**
     * Constructor to take in None value and the constructed instance
     * is_none() will return true.
     *
     * Since there is already a predefined None value whose type is none_t,
     * None value should only be the one used for this constructor rather
     * than creating another none_t value type.
     * @see is_none
     */
    RUSTFP_CONSTEXPR Option(const none_t &) RUSTFP_NOEXCEPT_EXPR(
        std::is_nothrow_default_constructible<optional_t>::value);

    /**
     * Assignment to take in the Some wrapped item and the assigned instance
     * is_some() will return true.
     * @param value Some wrapped item to be passed into the Option wrapper.
     * The item must be convertible to T.
     * @see is_some
     */
    template <class Tx>
    auto operator=(details::SomeImpl<Tx> &&value) -> Option<T> &;

    /**
     * Copy assignment to target RHS Option.
     * @param RHS option to be copy assigned from.
     */
    auto operator=(const Option<T> &rhs) -> Option<T> &;

    /**
     * Copy assignment template to target RHS Option.
     * @param RHS option to be copy assigned from. RHS Tx item type must be
     * convertible to T.
     */
    template <class Tx>
    auto operator=(const Option<Tx> &rhs) -> Option<T> &;

    /**
     * Move assignment to target RHS Option.
     * @param RHS option to be move assigned from.
     */
    auto operator=(Option<T> &&rhs) -> Option<T> &;

    /**
     * Move assignment template to target RHS Option.
     * @param RHS option to be move assigned from. RHS Tx item type must be
     * convertible to T.
     */
    template <class Tx>
    auto operator=(Option<Tx> &&rhs) -> Option<T> &;

    /**
     * Assignment to take in None value and the assigned instance is_none() will
     * return true.
     *
     * Since there is already a predefined None value whose type is none_t, None
     * value should only be the one used for this assignment rather than
     * creating another none_t value type.
     * @see is_none
     */
    auto operator=(const none_t &) -> Option<T> &;

    /**
     * Returns a lvalue const reference to the contained item.
     *
     * Asserts that is_some() == true. If is_none() == true and the
     * assertion does not take place, this would cause an undefined
     * behaviour.
     * @see is_some
     * @see is_none
     */
    RUSTFP_CONSTEXPR auto get_unchecked() const RUSTFP_NOEXCEPT
        -> std::add_lvalue_reference_t<
            std::add_const_t<std::remove_reference_t<T>>>;

    /**
     * Returns the moved contained item of type T which may possibly be a
     * reference type without any move operation performed if T was originally a
     * reference type.
     *
     * This causes is_none() to become true after invoking because the contained
     * item will be moved. Asserts that is_some() == true. If is_none() == true
     * and the assertion does not take place, this would cause an undefined
     * behaviour.
     * @see some_t
     * @see is_some
     * @see is_none
     */
    auto unwrap_unchecked() && -> T;

    /**
     * Matches the corresponding function to invoke depending on whether
     * is_some() or is_none().
     *
     * If is_some(), some_fn will be invoked. Otherwise none_fn() will be
     * invoked. This causes is_none() to become true after invoking because the
     * contained item will be moved. The return type of both functions must be
     * convertible to each other.
     * @param SomeFn T -> R1, where R1 is the function result type, convertible
     * to R2
     * @param NoneFn () -> R2, where R2 is the function result type, convertible
     * to R1
     * @param some_fn takes in the moved item and generates R1 value
     * @param none_fn takes in no argument and generates R2 value
     * @see is_some
     * @see is_none
     */
    template <class SomeFn, class NoneFn>
    auto match(SomeFn &&some_fn, NoneFn &&none_fn) && -> std::common_type_t<
        std::result_of_t<SomeFn(some_t)>,
        std::result_of_t<NoneFn()>>;

    /**
     * Matches the corresponding function to invoke depending on whether
     * is_some() or is_none().
     *
     * If is_some(), some_fn will be invoked. Otherwise none_fn() will be
     * invoked. This does not move the contained item, and generates only the
     * const lvalue reference if is_some(). The return type of both functions
     * must be convertible to each other.
     * @param SomeFn const T & -> R1, where R1 is the function result type,
     * convertible to R2
     * @param NoneFn () -> R2, where R2 is the function result type, convertible
     * to R1
     * @param some_fn takes in the const lvalue item reference and generates R1
     * value
     * @param none_fn takes in no argument and generates R2 value
     * @see is_some
     * @see is_none
     */
    template <class SomeFn, class NoneFn>
    auto
    match(SomeFn &&some_fn, NoneFn &&none_fn) const & -> std::common_type_t<
        std::result_of_t<SomeFn(const some_t &)>,
        std::result_of_t<NoneFn()>>;

    /**
     * Attempts to invoke function if is_some().
     *
     * If is_some(), some_fn will be invoked. Otherwise nothing happens. This
     * causes is_none() to become true after invoking because the contained item
     * will be moved.
     * @param SomeFn T -> R, where R is the function result type
     * @param some_fn takes in the moved item and generates R value
     * @see is_some
     * @see is_none
     */
    template <class SomeFn>
    auto match_some(SomeFn &&some_fn) && -> unit_t;

    /**
     * Attempts to invoke function if is_some().
     *
     * If is_some(), some_fn will be invoked. Otherwise nothing happens. This
     * does not move the contained item, and generates only the const lvalue
     * reference if is_some().
     * @param SomeFn const T & -> R, where R is the function result type
     * @param some_fn takes in the const lvalue item reference and generates R
     * value
     * @see is_some
     */
    template <class SomeFn>
    auto match_some(SomeFn &&some_fn) const & -> unit_t;

    /**
     * Attempts to invoke function if is_none().
     *
     * Moved version, works similarly to const lvalue reference version. If
     * is_none(), none_fn will be invoked. Otherwise nothing happens.
     * @param NoneFn () -> R, where R is the function result type
     * @param none_fn takes in no argument and generates R value
     * @see is_none
     */
    template <class NoneFn>
    auto match_none(NoneFn &&none_fn) && -> unit_t;

    /**
     * Attempts to invoke function if is_none().
     *
     * Const lvalue referene version, works similarly to moved version. If
     * is_none(), none_fn will be invoked. Otherwise nothing happens.
     * @param NoneFn () -> R, where R is the function result type
     * @param none_fn takes in no argument and generates R value
     * @see is_none
     */
    template <class NoneFn>
    auto match_none(NoneFn &&none_fn) const & -> unit_t;

    /**
     * fn is_some(&self) -> bool
     *
     * Returns true if the option is a Some value.
     */
    RUSTFP_CONSTEXPR auto is_some() const RUSTFP_NOEXCEPT -> bool;

    /**
     * fn is_none(&self) -> bool
     *
     * Returns true if the option is a None value.
     */
    RUSTFP_CONSTEXPR auto is_none() const RUSTFP_NOEXCEPT -> bool;

    /**
     * fn as_ref(&self) -> Option<&T>
     *
     * Converts from Option<T> to Option<&T>.
     */
    auto as_ref() const -> Option<const T &>;

    /**
     * fn as_mut(&mut self) -> Option<&mut T>
     *
     * Converts from Option<T> to Option<&mut T>.
     */
    auto as_mut() -> Option<T &>;

    /**
     * fn unwrap_or(self, def: T) -> T
     *
     * Returns the contained value or a default.
     */
    template <class Tx>
    auto unwrap_or(Tx &&def) && -> T;

    /**
     * fn unwrap_or_else<F>(self, f: F) -> T
     * where
     *     F: FnOnce() -> T,
     *
     * Returns the contained value or computes it from a closure.
     */
    template <class F>
    auto unwrap_or_else(F &&f) && -> special_decay_t<std::result_of_t<F()>>;

    /**
     * fn map<U, F>(self, f: F) -> Option<U>
     * where
     *     F: FnOnce(T) -> U,
     *
     * Maps an Option<T> to Option<U> by applying a function to a contained
     * value.
     */
    template <class F>
    auto map(F &&fn) && -> Option<special_decay_t<std::result_of_t<F(T &&)>>>;

    /**
     * fn map_or<U, F>(self, default: U, f: F) -> U
     * where
     *     F: FnOnce(T) -> U,
     *
     * Applies a function to the contained value (if any), or returns a default
     * (if not).
     */
    template <class U, class F>
    auto map_or(U &&def, F &&f) -> U;

    /**
     * fn map_or_else<U, D, F>(self, default: D, f: F) -> U
     * where
     *     D: FnOnce() -> U,
     *     F: FnOnce(T) -> U,
     *
     * Applies a function to the contained value (if any), or computes a default
     * (if not).
     */
    template <class D, class F>
    auto map_or_else(D &&def, F &&f) -> special_decay_t<std::result_of_t<D()>>;

    /**
     * fn ok_or<E>(self, err: E) -> Result<T, E>
     *
     * Transforms the Option<T> into a Result<T, E>, mapping Some(v) to Ok(v)
     * and None to Err(err).
     */
    template <class E>
    auto ok_or(E &&e) && -> Result<T, E>;

    /**
     * fn ok_or_else<E, F>(self, err: F) -> Result<T, E>
     * where
     *     F: FnOnce() -> E,
     *
     * Transforms the Option<T> into a Result<T, E>, mapping Some(v) to Ok(v)
     * and None to Err(err()).
     */
    template <class F>
    auto
    ok_or_else(F &&err) && -> Result<T, special_decay_t<std::result_of_t<F()>>>;

    /**
     * fn and<U>(self, optb: Option<U>) -> Option<U>
     *
     * Returns None if the option is None, otherwise returns optb.
     */
    template <class U>
    auto and_(Option<U> &&optb) && -> Option<U>;

    /**
     * fn and_then<U, F>(self, f: F) -> Option<U>
     * where
     *     F: FnOnce(T) -> Option<U>,
     *
     * Returns None if the option is None, otherwise calls f with the wrapped
     * value and returns the result.
     */
    template <class F>
    auto
    and_then(F &&f) && -> Option<typename std::result_of_t<F(T &&)>::some_t>;

    /**
     * fn or(self, optb: Option<T>) -> Option<T>
     *
     * Returns the option if it contains a value, otherwise returns optb.
     */
    auto or_(Option<T> &&optb) && -> Option<T>;

    /**
     * fn or_else<F>(self, f: F) -> Option<T>
     * where
     *     F: FnOnce() -> Option<T>,
     *
     * Returns the option if it contains a value, otherwise calls f and returns
     * the result.
     */
    template <class F>
    auto or_else(F &&f) && -> Option<T>;

    /**
     * fn get_or_insert(&mut self, v: T) -> &mut T
     *
     * Inserts v into the option if it is None, then returns a mutable reference
     * to the contained value.
     */
    template <class Tx>
    auto get_or_insert(Tx &&v) -> T &;

    /**
     * fn get_or_insert_with<F>(&mut self, f: F) -> &mut T
     * where
     *     F: FnOnce() -> T,
     *
     * Inserts a value computed from f into the option if it is None, then
     * returns a mutable reference to the contained value.
     */
    template <class F>
    auto get_or_insert_with(F &&f) -> T &;

    /**
     * fn take(&mut self) -> Option<T>
     *
     * Takes the value out of the option, leaving a None in its place.
     */
    auto take() -> Option<T>;

    /**
     * impl<'a, T> Option<&'a T>
     * where
     *     T: Clone,
     * fn cloned(self) -> Option<T>
     *
     * Maps an Option<&T> to an Option<T> by cloning the contents of the option.
     */
    auto cloned() && -> Option<std::remove_const_t<std::remove_reference_t<T>>>;

    /**
     * impl<T> Option<T>
     * where
     *     T: Default,
     * fn unwrap_or_default(self) -> T
     *
     * Returns the contained value or a default
     *
     * Consumes the self argument then, if Some, returns the contained value,
     * otherwise if None, returns the default value for that type.
     */
    auto unwrap_or_default() && -> T;

private:
    optional_t opt;
};

/**
 * Generates a new Option via a more convenient forwarding function.
 *
 * This allows the caller not to specify the template type T. The function is
 * not perfect forwarding and follows the same strategy as std::make_tuple,
 * where any lvalue reference must be wrapped in std::reference_wrapper to be
 * forwarded as a lvalue reference.
 * @param value item to be passed into the Option for wrapping
 * @see Option
 */
template <class T>
RUSTFP_CONSTEXPR auto Some(T &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<Option<special_decay_t<T>>>::value)
    -> Option<special_decay_t<T>>;

/**
 * Convenient function to generate Option based on bool value.
 *
 * Returns Option<T>, given that some_fn: () -> T. is_some() == true if the cond
 * is true, otherwise is_none() == true.
 * @param cond if true, executes some_fn and generates the Option with is_some()
 * == true. Otherwise generates Option with is_none() == true.
 * @param some_fn function to generate the some value for the new Option. Takes
 * no arguments, i.e. () -> T.
 * @see Option::is_some
 * @see Option::is_none
 */
template <class SomeFn>
auto opt_if(const bool cond, SomeFn &&some_fn)
    -> Option<std::result_of_t<SomeFn()>>;

// implementation section

namespace details {
template <class T>
class SomeImpl {
    template <class Tx>
    friend class Option;

public:
    template <class Tx>
    RUSTFP_CONSTEXPR explicit SomeImpl(Tx &&value)
        : value(std::forward<Tx>(value)) {
    }

    inline RUSTFP_CONSTEXPR auto get() const -> const T & {
        return value;
    }

    inline RUSTFP_CONSTEXPR auto move() && -> reverse_decay_t<T> {
        return std::move(value);
    }

private:
    reverse_decay_t<T> value;
};
} // namespace details

template <class T>
template <class Tx>
RUSTFP_CONSTEXPR Option<T>::Option(details::SomeImpl<Tx> &&value)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_constructible<optional_t>::value)
    : opt(std::move(value).move()) {

    static_assert(
        std::is_constructible<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T cannot be constructed from Tx");
}

template <class T>
RUSTFP_CONSTEXPR Option<T>::Option(const Option<T> &rhs)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_copy_constructible<optional_t>::value)
    : opt(rhs.opt) {

    static_assert(
        std::is_copy_constructible<T>::value, "T is not copy constructible");
}

template <class T>
template <class Tx>
RUSTFP_CONSTEXPR Option<T>::Option(const Option<Tx> &rhs)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_copy_constructible<optional_t>::value)
    : opt(rhs.opt ? optional_t(*rhs.opt) : nonstd::nullopt) {

    static_assert(
        std::is_constructible<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T cannot be constructed from Tx");
}

template <class T>
RUSTFP_CONSTEXPR Option<T>::Option(Option<T> &&rhs)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_constructible<optional_t>::value)
    : opt(std::move(rhs.opt)) {

    static_assert(
        std::is_move_constructible<T>::value, "T is not move constructible");

    rhs.opt.reset();
}

template <class T>
template <class Tx>
RUSTFP_CONSTEXPR Option<T>::Option(Option<Tx> &&rhs)
    RUSTFP_NOEXCEPT_EXPR(std::is_nothrow_move_constructible<optional_t>::value)
    : opt(rhs.opt ? optional_t(std::move(*rhs.opt)) : nonstd::nullopt) {

    static_assert(
        std::is_constructible<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T cannot be constructed from Tx");

    rhs.opt.reset();
}

template <class T>
RUSTFP_CONSTEXPR Option<T>::Option(const none_t &) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_default_constructible<optional_t>::value)
    : opt() {
}

template <class T>
template <class Tx>
auto Option<T>::operator=(details::SomeImpl<Tx> &&value) -> Option<T> & {
    static_assert(
        std::is_assignable<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T is not assignable from Tx");

    opt = std::move(value).move();
    return *this;
}

template <class T>
auto Option<T>::operator=(const Option<T> &rhs) -> Option<T> & {
    static_assert(
        std::is_copy_assignable<T>::value, "T is not copy assignable");

    opt = rhs.opt;
    return *this;
}

template <class T>
template <class Tx>
auto Option<T>::operator=(const Option<Tx> &rhs) -> Option<T> & {
    static_assert(
        std::is_assignable<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T is not assignable from Tx");

    opt = rhs.opt ? optional_t(*rhs.opt) : nonstd::nullopt;
    return *this;
}

template <class T>
auto Option<T>::operator=(Option<T> &&rhs) -> Option<T> & {
    static_assert(
        std::is_move_assignable<reverse_decay_t<T>>::value,
        "T is not move assignable");

    opt = std::move(rhs.opt);
    rhs.opt.reset();

    return *this;
}

template <class T>
template <class Tx>
auto Option<T>::operator=(Option<Tx> &&rhs) -> Option<T> & {
    static_assert(
        std::is_assignable<reverse_decay_t<T>, reverse_decay_t<Tx>>::value,
        "T is not assignable from Tx");

    opt = rhs.opt ? optional_t(std::move(*rhs.opt)) : nonstd::nullopt;
    rhs.opt.reset();

    return *this;
}

template <class T>
auto Option<T>::operator=(const none_t &) -> Option<T> & {
    opt.reset();
    return *this;
}

template <class T>
RUSTFP_CONSTEXPR auto Option<T>::get_unchecked() const RUSTFP_NOEXCEPT
    -> std::add_lvalue_reference_t<
        std::add_const_t<std::remove_reference_t<T>>> {

    // reference_wrapper can implicitly convert into reference
    return *opt;
}

template <class T>
auto Option<T>::unwrap_unchecked() && -> T {
    // forward moves rref value only, lref remains as lref
    reverse_decay_t<T> value = std::forward<T>(*opt);
    opt.reset();

    // no effect on reference_wrapper, only affects value type
    return std::move(value);
}

template <class T>
template <class SomeFn, class NoneFn>
auto Option<T>::match(SomeFn &&some_fn, NoneFn &&none_fn)
    && -> std::common_type_t<
        std::result_of_t<SomeFn(some_t)>,
        std::result_of_t<NoneFn()>> {

    return is_some() ? some_fn(std::move(*this).unwrap_unchecked()) : none_fn();
}

template <class T>
template <class SomeFn, class NoneFn>
auto Option<T>::match(SomeFn &&some_fn, NoneFn &&none_fn)
    const & -> std::common_type_t<
        std::result_of_t<SomeFn(const some_t &)>,
        std::result_of_t<NoneFn()>> {

    return is_some() ? some_fn(get_unchecked()) : none_fn();
}

template <class T>
template <class SomeFn>
auto Option<T>::match_some(SomeFn &&some_fn) && -> unit_t {
    if (is_some()) {
        some_fn(std::move(*this).unwrap_unchecked());
    }

    return Unit;
}

template <class T>
template <class SomeFn>
auto Option<T>::match_some(SomeFn &&some_fn) const & -> unit_t {
    if (is_some()) {
        some_fn(get_unchecked());
    }

    return Unit;
}

template <class T>
template <class NoneFn>
auto Option<T>::match_none(NoneFn &&none_fn) && -> unit_t {
    if (is_none()) {
        none_fn();
    }

    return Unit;
}

template <class T>
template <class NoneFn>
auto Option<T>::match_none(NoneFn &&none_fn) const & -> unit_t {
    if (is_none()) {
        none_fn();
    }

    return Unit;
}

template <class T>
RUSTFP_CONSTEXPR auto Option<T>::is_some() const RUSTFP_NOEXCEPT -> bool {
    return opt.has_value();
}

template <class T>
RUSTFP_CONSTEXPR auto Option<T>::is_none() const RUSTFP_NOEXCEPT -> bool {
    return !opt.has_value();
}

template <class T>
auto Option<T>::as_ref() const -> Option<const T &> {
    return is_some() ? Some(std::cref(*opt)) : None;
}

template <class T>
auto Option<T>::as_mut() -> Option<T &> {
    static_assert(
        !std::is_const<T>::value,
        "T must not have const specifier for Option::as_mut()");

    return is_some() ? Some(std::ref(*opt)) : None;
}

template <class T>
template <class Tx>
auto Option<T>::unwrap_or(Tx &&def) && -> T {
    if (is_some()) {
        return std::move(*this).unwrap_unchecked();
    } else {
        return std::forward<Tx>(def);
    }
}

template <class T>
template <class F>
auto Option<T>::unwrap_or_else(
    F &&f) && -> special_decay_t<std::result_of_t<F()>> {

    if (is_some()) {
        return std::move(*this).unwrap_unchecked();
    } else {
        return special_decay(f());
    }
}

template <class T>
template <class F>
auto Option<T>::map(
    F &&f) && -> Option<special_decay_t<std::result_of_t<F(T &&)>>> {

    if (is_some()) {
        return Some(f(std::move(*this).unwrap_unchecked()));
    } else {
        return None;
    }
}

template <class T>
template <class U, class F>
auto Option<T>::map_or(U &&def, F &&f) -> U {
    if (is_some()) {
        return special_decay(f(std::move(*this).unwrap_unchecked()));
    } else {
        return std::forward<U>(def);
    }
}

template <class T>
template <class D, class F>
auto Option<T>::map_or_else(D &&def, F &&f)
    -> special_decay_t<std::result_of_t<D()>> {

    if (is_some()) {
        return special_decay(f(std::move(*this).unwrap_unchecked()));
    } else {
        return special_decay(def());
    }
}

template <class T>
template <class E>
auto Option<T>::ok_or(E &&e) && -> Result<T, E> {
    if (is_some()) {
        return Ok(reverse_decay(std::move(*this).unwrap_unchecked()));
    } else {
        return Err(std::forward<E>(e));
    }
}

template <class T>
template <class F>
auto Option<T>::ok_or_else(
    F &&err) && -> Result<T, special_decay_t<std::result_of_t<F()>>> {

    if (is_some()) {
        return Ok(reverse_decay(std::move(*this).unwrap_unchecked()));
    } else {
        return Err(err());
    }
}

template <class T>
template <class U>
auto Option<T>::and_(Option<U> &&optb) && -> Option<U> {
    if (is_some()) {
        opt.reset();
        return std::move(optb);
    } else {
        optb.opt.reset();
        return None;
    }
}

template <class T>
template <class F>
auto Option<T>::and_then(
    F &&f) && -> Option<typename std::result_of_t<F(T &&)>::some_t> {

    if (is_some()) {
        return f(std::move(*this).unwrap_unchecked());
    } else {
        return None;
    }
}

template <class T>
auto Option<T>::or_(Option<T> &&optb) && -> Option<T> {
    if (is_some()) {
        optb.opt.reset();
        return std::move(*this);
    } else {
        return std::move(optb);
    }
}

template <class T>
template <class F>
auto Option<T>::or_else(F &&f) && -> Option<T> {
    return is_some() ? std::move(*this) : f();
}

template <class T>
template <class Tx>
auto Option<T>::get_or_insert(Tx &&v) -> T & {
    if (is_none()) {
        opt = std::forward<Tx>(v);
    }

    return *opt;
}

template <class T>
template <class F>
auto Option<T>::get_or_insert_with(F &&f) -> T & {
    if (is_none()) {
        opt = f();
    }

    return *opt;
}

template <class T>
auto Option<T>::take() -> Option<T> {
    if (is_some()) {
        return Some(reverse_decay(std::move(*this).unwrap_unchecked()));
    } else {
        return None;
    }
}

template <class T>
auto Option<T>::cloned()
    && -> Option<std::remove_const_t<std::remove_reference_t<T>>> {

    if (is_some()) {
        // force invoke copy ctor by not converting & to reference_wrapper
        return Some(std::move(*this).unwrap_unchecked());
    } else {
        return None;
    }
}

template <class T>
auto Option<T>::unwrap_or_default() && -> T {
    if (is_some()) {
        // no need to convert & to reference_wrapper
        return std::move(*this).unwrap_unchecked();
    } else {
        return T();
    }
}

template <class T>
RUSTFP_CONSTEXPR auto Some(T &&value) RUSTFP_NOEXCEPT_EXPR(
    std::is_nothrow_constructible<Option<special_decay_t<T>>>::value)
    -> Option<special_decay_t<T>> {

    return Option<special_decay_t<T>>(
        details::SomeImpl<special_decay_t<T>>(std::forward<T>(value)));
}

template <class SomeFn>
auto opt_if(const bool cond, SomeFn &&some_fn)
    -> Option<std::result_of_t<SomeFn()>> {

    if (cond) {
        return Some(some_fn());
    } else {
        return None;
    }
}
} // namespace rustfp

#include "result.h"
