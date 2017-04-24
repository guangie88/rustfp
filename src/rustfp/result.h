#pragma once

#include "mapbox/variant.hpp"

#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class T>
        struct OkImpl
        {
            template <class Tx>
            explicit OkImpl(Tx &&value) :
                value(std::forward<Tx>(value))
            {
            }

            T value;
        };

        template <class E>
        struct ErrImpl
        {
            template <class Ex>
            explicit ErrImpl(Ex &&err) :
                err(std::forward<Ex>(err))
            {
            }
            
            E err;
        };
    }
    
    template <class T, class E>
    class Result
    {
    public:
        template <class Tx>
        Result(details::OkImpl<Tx> &&value) :
            value_err(std::move(value))
        {
        }

        template <class Ex>
        Result(details::ErrImpl<Ex> &&err) :
            value_err(std::move(err))
        {
        }

        auto is_ok() const -> bool
        {
            return value_err.which() == 0;
        }

        auto is_err() const -> bool
        {
            return !is_ok();
        }

    private:
        mapbox::util::variant<details::OkImpl<T>, details::ErrImpl<E>> value_err;
    };

    template <class T>
    auto Ok(T &&value) -> details::OkImpl<special_decay_t<T>>
    {
        return details::OkImpl<special_decay_t<T>>(std::forward<T>(value));
    }

    template <class E>
    auto Err(E &&error) -> details::ErrImpl<special_decay_t<E>>
    {
        return details::ErrImpl<special_decay_t<E>>(std::forward<E>(error));
    }

    template <class OkFn, class ErrFn>
    auto if_else_res(const bool cond, OkFn &&ok_fn, ErrFn &&err_fn) -> Result<std::result_of_t<OkFn()>, std::result_of_t<ErrFn()>>
    {
        if (cond)
        {
            return Ok(ok_fn());
        }
        else
        {
            return Err(err_fn());
        }
    }
}
