#pragma once

//! Contains Rust Option enum implementation
/*! @author Chen Weiguang
 *  @version 0.1.0
 */

#include "option_fwd.h"
#include "traits.h"

#include "akrzemi1/optional.hpp"

#include <type_traits>
#include <utility>

namespace rustfp
{
    // declaration section

    namespace details
    {
        template <class T>
        class SomeImpl;
    }

    //! Simulates the Rust Option enum.
    /*! Wraps a value or reference in a nullable wrapper and provides various monadic operations on the wrapped item.
     *  @param T item type to wrap over
     */
    template <class T>
    class Option
    {
    public:
        //! Alias to the item type to be wrapped. SomeType == T.
        using SomeType = T;
        
        //! Constructor to take in the Some wrapped item and the constructed instance is_some() will return true.
        /*! @param value Some wrapped item to be passed into the Option wrapper. The item must be convertible to T.
         *  @see is_some
         */
        template <class Tx>
        Option(details::SomeImpl<Tx> &&value);

        //! Copy constructor to target RHS Option.
        /*! @param RHS option to be copy constructed from.
         */
        Option(const Option<T> &rhs);

        //! Copy constructor template to target RHS Option.
        /*! @param RHS option to be copy constructed from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        Option(const Option<Tx> &rhs);

        //! Move constructor to target RHS moving Option.
        /*! @param RHS option to be move constructed from.
         */
        Option(Option<T> &&rhs);

        //! Move constructor template to target RHS moving Option.
        /*! @param RHS option to be move constructed from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        Option(Option<Tx> &&rhs);

        //! Constructor to take in None value and the constructed instance is_none() will return true.
        /*! Since there is already a predefined None value whose type is none_t, None value should only be the one used
         *  for this constructor rather than creating another none_t value type.
         *  @see is_none
         */
        Option(const none_t &);

        //! Assignment to take in the Some wrapped item and the assigned instance is_some() will return true.
        /*! @param value Some wrapped item to be passed into the Option wrapper. The item must be convertible to T.
         *  @see is_some
         */
        template <class Tx>
        auto operator=(details::SomeImpl<Tx> &&value) -> Option<T> &;

        //! Copy assignment to target RHS Option.
        /*! @param RHS option to be copy assigned from.
         */
        auto operator=(const Option<T> &rhs) -> Option<T> &;

        //! Copy assignment template to target RHS Option.
        /*! @param RHS option to be copy assigned from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        auto operator=(const Option<Tx> &rhs) -> Option<T> &;

        //! Move assignment to target RHS Option.
        /*! @param RHS option to be move assigned from.
         */
        auto operator=(Option<T> &&rhs) -> Option<T> &;

        //! Move assignment template to target RHS Option.
        /*! @param RHS option to be move assigned from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        auto operator=(Option<Tx> &&rhs) -> Option<T> &;

        //! Assignment to take in None value and the assigned instance is_none() will return true.
        /*! Since there is already a predefined None value whose type is none_t, None value should only be the one used
         *  for this assignment rather than creating another none_t value type.
         *  @see is_none
         */
        auto operator=(const none_t &) -> Option<T> &;

        //! Performs a map of current item type to another item type.
        /*! Only performs the given fn if is_some() == true. Changes the output type to Option<Tx>, given that fn: T -> Tx.
         *  @param FnTToTx function type which maps from T to Tx.
         *  @param fn function that takes in a rvalue reference of T to generate a new item type in the new Option.
         *  @see is_some
         */
        template <class FnTToTx>
        auto map(FnTToTx &&fn) && -> Option<std::result_of_t<FnTToTx(T &&)>>;

        //! Performs a map of current item type to a new Option with another item type or no item.
        /*! Only performs the given fn if is_some() == true. Changes the output type to Option<Tx>, given that fn: T -> Option<Tx>.
         *  @param FnTToOptTx function type which maps from T to Option<Tx>.
         *  @param fn function that takes in a rvalue reference of T to generate an Option with new item type.
         *  @see is_some
         */
        template <class FnTToOptTx>
        auto and_then(FnTToOptTx &&fn) && -> Option<typename std::result_of_t<FnTToOptTx(T &&)>::SomeType>;

        //! Performs a map of no item to a new Option with another item type or no item.
        /*! Only performs the given fn if is_none() == true. Changes the output type to Option<Tx>, given that fn: () -> Option<Tx>.
         *  @param FnToOptTx function type which maps from () to Option<Tx>.
         *  @param fn function that takes in no argument to generate an Option with new item type.
         *  @see is_none
         */
        template <class FnToOptTx>
        auto or_else(FnToOptTx &&fn) && -> Option<T>;

        //! Returns true if the Option instance contains an item. Otherwise returns false.
        /*! is_some() == ! is_none().
         *  @see is_none
         */
        auto is_some() const -> bool;

        //! Returns true if the Option instance does not contain an item. Otherwise returns false.
        /*! is_none() == ! is_some().
         *  @see is_some
         */
        auto is_none() const -> bool;

        //! Returns a lvalue const reference to the contained item.
        /*! Asserts that is_some() == true. If is_none() == true and the assertion does not take place,
         *  this would cause an undefined behaviour.
         *  @see is_some
         *  @see is_none
         */
        auto get_unchecked() const -> const T &;

        //! Returns the moved contained item of type T if T is not a lvalue reference, otherwise returns std::reference_wrapper<T> to the contained item.
        /*! This causes is_none() to become true because the contained item will be moved.
         *  Asserts that is_some() == true. If is_none() == true and the assertion does not take place,
         *  this would cause an undefined behaviour.
         *  @see SomeType
         *  @see is_some
         *  @see is_none
         */
        auto unwrap_unchecked() -> reverse_decay_t<T>;

    private:
        std::experimental::optional<reverse_decay_t<T>> opt;
    };

    //! Generates a new Option via a more convenient forwarding function.
    /*! This allows the caller not to specify the template type T. The function is not perfect forwarding
     *  and follows the same strategy as std::make_tuple, where any lvalue reference must be wrapped
     *  in std::reference_wrapper to be forwarded as a lvalue reference.
     *  @param value item to be passed into the Option for wrapping
     *  @see Option
     */
    template <class T>
    auto Some(T &&value) -> Option<special_decay_t<T>>;

    //! Convenient function to generate Option based on bool value.
    /*! Returns Option<T>, given that some_fn: () -> T.
     *  is_some() == true if the cond is true, otherwise is_none() == true.
     *  @param cond if true, executes some_fn and generates the Option with is_some() == true. Otherwise generates Option with is_none() == true.
     *  @param some_fn function to generate the some value for the new Option. Takes no arguments, i.e. () -> T.
     *  @see Option::is_some
     *  @see Option::is_none
     */
    template <class SomeFn>
    auto opt_if(const bool cond, SomeFn &&some_fn) -> Option<std::result_of_t<SomeFn()>>;

    // implementation section

    namespace details
    {
        template <class T>
        class SomeImpl
        {
            template <class Tx>
            friend class Option;

        public:
            template <class Tx>
            explicit SomeImpl(Tx &&value) :
                value(std::forward<Tx>(value))
            {
            }

            inline auto get() const -> const T &
            {
                return value;
            }

            inline auto move() && -> reverse_decay_t<T>
            {
                return std::move(value);
            }

        private:
            reverse_decay_t<T> value;
        };

        template <class T>
        inline auto get_ref(T &value) -> T &
        {
            return value;
        }
        
        template <class T>
        inline auto get_ref(std::reference_wrapper<T> &ref) -> T &
        {
            return ref.get();
        }

        template <class T>
        inline auto get_ref(const std::reference_wrapper<T> &ref) -> const T &
        {
            return ref.get();
        }
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(details::SomeImpl<Tx> &&value) :
        opt(std::move(value).move())
    {
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in value constructor of Option<T>");
    }

    template <class T>
    Option<T>::Option(const Option<T> &rhs) :
        opt(rhs.opt)
    {
        static_assert(std::is_copy_constructible<T>::value, "T is not copy constructible");
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(const Option<Tx> &rhs) :
        opt(rhs.opt)
    {
        static_assert(std::is_copy_constructible<Tx>::value, "Tx is not copy constructible");
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in copy constructor of Option<T>");
    }

    template <class T>
    Option<T>::Option(Option<T> &&rhs) :
        opt(std::move(rhs.opt))
    {
        static_assert(std::is_move_constructible<T>::value, "T is not move constructible");
        rhs.opt.reset();
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(Option<Tx> &&rhs) :
        opt(std::move(rhs.opt))
    {
        static_assert(std::is_move_constructible<Tx>::value, "Tx is not move constructible");
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in move constructor of Option<T>");
        rhs.opt.reset();
    }

    template <class T>
    Option<T>::Option(const none_t &) :
        opt(std::experimental::nullopt)
    {
    }

    template <class T>
    template <class Tx>
    auto Option<T>::operator=(details::SomeImpl<Tx> &&value) -> Option<T> &
    {
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in value assignment of Option<T>");

        opt = std::move(value).move();
        return *this;
    }

    template <class T>
    auto Option<T>::operator=(const Option<T> &rhs) -> Option<T> &
    {
        static_assert(std::is_copy_assignable<T>::value, "T is not copy assignable");

        opt = rhs.opt;
        return *this;
    }

    template <class T>
    template <class Tx>
    auto Option<T>::operator=(const Option<Tx> &rhs) -> Option<T> &
    {
        static_assert(std::is_copy_assignable<Tx>::value, "Tx is not copy assignable");
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in copy assignment of Option<T>");

        opt = rhs.opt;
        return *this;
    }

    template <class T>
    auto Option<T>::operator=(Option<T> &&rhs) -> Option<T> &
    {
        static_assert(std::is_move_assignable<T>::value, "T is not move assignable");

        opt = std::move(rhs.opt);
        return *this;
    }

    template <class T>
    template <class Tx>
    auto Option<T>::operator=(Option<Tx> &&rhs) -> Option<T> &
    {
        static_assert(std::is_move_constructible<Tx>::value, "Tx is not move constructible");
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in move assignment of Option<T>");

        opt = std::move(rhs.opt);
        return *this;
    }

    template <class T>
    auto Option<T>::operator=(const none_t &) -> Option<T> &
    {
        opt.reset();
        return *this;
    }

    template <class T>
    template <class FnTToTx>
    auto Option<T>::map(FnTToTx &&fn) && -> Option<std::result_of_t<FnTToTx(T &&)>>
    {
        return is_some()
            ? [this, &fn]
            {
                return Some(fn(unwrap_unchecked()));
            }()

            : None;
    }

    template <class T>
    template <class FnTToOptTx>
    auto Option<T>::and_then(FnTToOptTx &&fn) && -> Option<typename std::result_of_t<FnTToOptTx(T &&)>::SomeType>
    {
        return is_some()
            ? [this, &fn]
            {
                return fn(unwrap_unchecked());
            }()

            : None;
    }

    template <class T>
    template <class FnToOptTx>
    auto Option<T>::or_else(FnToOptTx &&fn) && -> Option<T>
    {
        return is_none()
            ? fn()
            : std::move(*this);
    }

    template <class T>
    auto Option<T>::is_some() const -> bool
    {
        return opt.has_value();
    }

    template <class T>
    auto Option<T>::is_none() const -> bool
    {
        return !opt.has_value();
    }

    template <class T>
    auto Option<T>::get_unchecked() const -> const T &
    {
        return details::get_ref(*opt);
    }

    template <class T>
    auto Option<T>::unwrap_unchecked() -> reverse_decay_t<T>
    {
        // forward moves rref value only, lref remains as lref
        reverse_decay_t<T> value = std::forward<T>(*opt);
        opt.reset();
        return std::move(value);
    }

    template <class T>
    auto Some(T &&value) -> Option<special_decay_t<T>>
    {
        return Option<special_decay_t<T>>(details::SomeImpl<special_decay_t<T>>(std::forward<T>(value)));
    }

    template <class SomeFn>
    auto opt_if(const bool cond, SomeFn &&some_fn) -> Option<std::result_of_t<SomeFn()>>
    {
        if (cond)
        {
            return Some(some_fn());
        }
        else
        {
            return None;
        }
    }
}

#include "result.h"