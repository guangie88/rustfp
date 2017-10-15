/**
 * Contains type transformations used within the namespace.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include <functional>
#include <type_traits>

namespace rustfp {
    
    // declaration section

    namespace details {
        template <class T>
        struct special_decay_impl {
            using type = std::decay_t<T>;
        };

        template <class T>
        struct special_decay_impl<std::reference_wrapper<T>> {
            using type = T &;
        };

        template <class T>
        struct special_decay_impl<std::reference_wrapper<T> &> {
            using type = T &;
        };

        template <class T>
        struct special_decay_impl<const std::reference_wrapper<T> &> {
            using type = T &;
        };

        template <class T>
        struct reverse_decay_impl {
            using type = T;
        };

        template <class T>
        struct reverse_decay_impl<T &> {
            using type = std::reference_wrapper<T>;
        };

        template <class T>
        struct simplify_ref_impl {
            using type = T;
        };

        template <class T>
        struct simplify_ref_impl<std::reference_wrapper<T>> {
            using type = T &;
        };

        template <class T>
        struct simplify_ref_impl<std::reference_wrapper<T> &> {
            using type = T &;
        };

        template <class T>
        struct simplify_ref_impl<const std::reference_wrapper<T> &> {
            using type = T &;
        };
    }

    template <class T>
    using special_decay_t = typename details::special_decay_impl<T>::type;

    template <class T>
    using reverse_decay_t = typename details::reverse_decay_impl<T>::type;

    template <class T>
    using simplify_ref_t = typename details::simplify_ref_impl<T>::type;

    template <class T>
    using special_move_t = T &&;

    template <class T>
    auto special_decay(T &&val) -> special_decay_t<T>;

    template <class T>
    auto reverse_decay(T &&val) -> reverse_decay_t<T>;

    // implementation section

    template <class T>
    inline auto special_decay(T &&val) -> special_decay_t<T> {
        return special_decay_t<T>(std::forward<T>(val));
    }

    template <class T>
    inline auto reverse_decay(T &&val) -> reverse_decay_t<T> {
        return reverse_decay_t<T>(std::forward<T>(val));
    }
}
