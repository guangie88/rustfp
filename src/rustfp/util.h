#pragma once

#include <type_traits>
#include <utility>

// declaration section

template <
    class Iterator,
    class Op,
    class = typename Iterator::Item,
    class = decltype(std::declval<Op>()(std::move(std::declval<Iterator>())))>
auto operator|(Iterator &&it, Op &&op) -> std::result_of_t<Op(Iterator)>;

// implementation section

template <class Iterator, class Op, class, class>
auto operator|(Iterator &&it, Op &&op) -> std::result_of_t<Op(Iterator)> {
    static_assert(
        !std::is_lvalue_reference<Iterator>::value &&
        !std::is_lvalue_reference<Iterator>::value,
        "rustfp operator| can only be applied on rvalues "
        "for both Iterator trait and Op trait based objects");

    return std::move(op)(std::move(it));
}
