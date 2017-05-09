#pragma once

#include "option_fwd.h"
#include "traits.h"

#include "akrzemi1/optional.hpp"

#include <type_traits>
#include <utility>

namespace rustfp
{
    // declaration section

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
        
        //! Constructor to take in the perfect forwarded item and the constructed instance is_some() will return true.
        /*! @param value perfect forwarded item to be passed into the Option wrapper. The item must be convertible to T.
         *  @see is_some
         */
        template <class Tx>
        explicit Option(Tx &&value);

        //! Copy constructor to target RHS non-const Option.
        /*! Required because of the previous perfect forwarding constructor.
         *  @param RHS option to be copy constructed from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        Option(Option<Tx> &rhs);

        //! Copy constructor to target RHS const Option.
        /*! @param RHS option to be copy constructed from. RHS Tx item type must be convertible to T.
         */
        template <class Tx>
        Option(const Option<Tx> &rhs);

        //! Move constructor to target RHS non-const Option.
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

        //! Performs a map of current item type to another item type.
        /*! Only performs the given fn if is_some() == true. Changes the output type to Option<Tx>, given that fn: T -> Tx.
         *  @param fn function that takes in a rvalue reference of T to generate a new item type in the new Option.
         *  @see is_some
         */
        template <class FnToType>
        auto map(FnToType &&fn) && -> Option<std::result_of_t<FnToType(T &&)>>;

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
    Option<T>::Option(Tx &&value) :
        opt(std::forward<Tx>(value))
    {
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(Option<Tx> &rhs) :
        opt(rhs.opt)
    {
        static_assert(std::is_copy_constructible<Tx>::value, "Option<T> is not copy constructible");
        static_assert(std::is_convertible<Tx, T>::value, "Tx is not convertible to T in non-const copy constructor of Option<T>");
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(const Option<Tx> &rhs) :
        opt(rhs.opt)
    {
        static_assert(std::is_copy_constructible<T>::value, "Option<T> is not copy constructible");
        static_assert(std::is_convertible<Tx, T>::Value, "Tx is not convertible to T in const copy constructor of Option<T>");
    }

    template <class T>
    template <class Tx>
    Option<T>::Option(Option<Tx> &&rhs) :
        opt(std::move(rhs.opt))
    {
        static_assert(std::is_convertible<Tx, T>::Value, "Tx is not convertible to T in move constructor of Option<T>");
    }

    template <class T>
    Option<T>::Option(const none_t &) :
        opt()
    {
    }

    template <class T>
    template <class FnToType>
    auto Option<T>::map(FnToType &&fn) && -> Option<std::result_of_t<FnToType(T &&)>>
    {
        return is_some()
            ? [this, &fn]
            {
                return Some(fn(unwrap_unchecked()));
            }()

            : None;
    }

    template <class T>
    auto Option<T>::is_some() const -> bool
    {
        return static_cast<bool>(opt);
    }

    template <class T>
    auto Option<T>::is_none() const -> bool
    {
        return !is_some();
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
        return Option<special_decay_t<T>>(std::forward<T>(value));
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