#pragma once

#include "mapbox/variant.hpp"

#include "traits.h"

#include <cassert>
#include <type_traits>
#include <utility>

#include "option_fwd.h"

namespace rustfp
{
    namespace details
    {
        template <class T>
        class OkImpl
        {
            template <class Tx, class Ex>
            friend class Result;

        public:
            template <class Tx>
            explicit OkImpl(Tx &&value) :
                value(std::forward<Tx>(value))
            {
            }

            auto get() const -> const T &
            {
                return value;
            }

            auto move() && -> reverse_decay_t<T>
            {
                return std::move(value);
            }

        private:
            reverse_decay_t<T> value;
        };

        template <class E>
        class ErrImpl
        {
            template <class Tx, class Ex>
            friend class Result;

        public:
            template <class Ex>
            explicit ErrImpl(Ex &&err) :
                err(std::forward<Ex>(err))
            {
            }

            auto get() const -> const E &
            {
                return err;
            }

            auto move() && -> reverse_decay_t<E>
            {
                return std::move(err);
            }
            
        private:
            reverse_decay_t<E> err;
        };

        template <class T, class E>
        auto get_unchecked(const mapbox::util::variant<details::OkImpl<T>, details::ErrImpl<E>> &value_err) -> const T &
        {
            return value_err.template get_unchecked<details::OkImpl<T>>().get();
        }

        template <class T, class E>
        auto get_err_unchecked(const mapbox::util::variant<details::OkImpl<T>, details::ErrImpl<E>> &value_err) -> const E &
        {
            return value_err.template get_unchecked<details::ErrImpl<E>>().get();
        }

        template <class T, class E>
        auto move_unchecked(mapbox::util::variant<details::OkImpl<T>, details::ErrImpl<E>> &&value_err) -> reverse_decay_t<T>
        {
            return std::move(value_err.template get_unchecked<details::OkImpl<T>>()).move();
        }

        template <class T, class E>
        auto move_err_unchecked(mapbox::util::variant<details::OkImpl<T>, details::ErrImpl<E>> &&value_err) -> reverse_decay_t<E>
        {
            return std::move(value_err.template get_unchecked<details::ErrImpl<E>>()).move();
        }
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
            return value_err.which() == 1;
        }

        auto get_unchecked() const -> const T &
        {
            assert(is_ok());
            return details::get_unchecked(value_err);
        }

        auto get_err_unchecked() const -> const E &
        {
            assert(is_err());
            return details::get_err_unchecked(value_err);
        }

        auto ok() && -> Option<T>
        {
            return is_ok()
                ? Some(details::move_unchecked(std::move(value_err)))
                : None;
        }

        auto err() && -> Option<E>
        {
            return is_err()
                ? Some(details::move_err_unchecked(std::move(value_err)))
                : None;
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

#include "option.h"