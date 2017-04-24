#pragma once

#include "traits.h"

#include "akrzemi1/optional.hpp"

#include <type_traits>
#include <utility>

namespace rustfp
{
    // declaration section

    template <class T>
    class Option;

    struct none_t {};
    none_t None;

    template <class T>
    auto Some(T &&value) -> Option<special_decay_t<T>>;

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
    class Option
    {
    public:
        using SomeType = T;
        
        template <class Tx>
        explicit Option(Tx &&value) :
            opt(std::forward<Tx>(value))
        {
        }

        Option(Option<T> &rhs) :
            opt(rhs.opt)
        {
            static_assert(std::is_copy_constructible<T>::value, "Option<T> is not copy constructible");
        }

        Option(const Option<T> &rhs) :
            opt(rhs.opt)
        {
            static_assert(std::is_copy_constructible<T>::value, "Option<T> is not copy constructible");
        }

        Option(Option<T> &&rhs) :
            opt(std::move(rhs.opt))
        {
        }

        Option(none_t) :
            opt()
        {
        }

        template <class FnToType>
        auto map(FnToType &&fn) -> Option<std::result_of_t<FnToType(T)>>
        {
            return is_some()
                ? [this, &fn]
                {
                    return Some(fn(unwrap_unchecked()));
                }()

                : None;
        }

        auto is_some() const -> bool
        {
            return static_cast<bool>(opt);
        }

        auto is_none() const -> bool
        {
            return !is_some();
        }

        auto get_unchecked() const -> const T &
        {
            return details::get_ref(*opt);
        }

        auto unwrap_unchecked() -> reverse_decay_t<T>
        {
            // forward moves rref value only, lref remains as lref
            reverse_decay_t<T> value = std::forward<T>(*opt);
            opt.reset();
            return std::move(value);
        }

    private:
        std::experimental::optional<reverse_decay_t<T>> opt;
    };

    template <class T>
    auto Some(T &&value) -> Option<special_decay_t<T>>
    {
        return Option<special_decay_t<T>>(std::forward<T>(value));
    }
}
