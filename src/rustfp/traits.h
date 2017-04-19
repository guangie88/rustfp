#pragma once

#include <functional>
#include <type_traits>

namespace rustfp
{
    namespace details
    {
        template <class T>
        struct special_decay_impl
        {
            using type = T;
        };

        template <class T>
        struct special_decay_impl<std::reference_wrapper<T>>
        {
            using type = T &;
        };

        template <class T>
        struct reverse_decay_impl
        {
            using type = T;
        };

        template <class T>
        struct reverse_decay_impl<T &>
        {
            using type = std::reference_wrapper<T>;
        };
    }

    template <class T>
    using special_decay_t = typename details::special_decay_impl<T>::type;

    template <class T>
    using reverse_decay_t = typename details::reverse_decay_impl<T>::type;

    template <class T>
    using special_move_t = T &&;
}