#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section

    namespace details {
        template <class Iterator, class IteratorRhs>
        class Zip {
        public:
            using Item = std::pair<typename Iterator::Item, typename IteratorRhs::Item>;

            template <class Iteratorx, class IteratorRhsx>
            Zip(Iteratorx &&it, IteratorRhsx &&it_rhs) :
                it(std::forward<Iteratorx>(it)),
                it_rhs(std::forward<IteratorRhsx>(it_rhs)) {

            }

            auto next() -> Option<Item> {
                auto opt_next = it.next();
                auto opt_rhs_next = it_rhs.next();

                if (opt_next.is_some() && opt_rhs_next.is_some()) {
                    return Some(Item(
                        std::move(opt_next).unwrap_unchecked(),
                        std::move(opt_rhs_next).unwrap_unchecked()));
                }

                return None;
            }

        private:
            Iterator it;
            IteratorRhs it_rhs;
        };

        template <class IteratorRhs>
        class ZipOp {
        public:
            template <class IteratorRhsx>
            explicit ZipOp(IteratorRhsx &&it_rhs) :
                it_rhs(std::move(it_rhs)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Zip<Iterator, IteratorRhs> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "zip can only take rvalue ref object with Iterator traits");

                return Zip<Iterator, IteratorRhs>(std::move(it), std::move(it_rhs));
            }

        private:
            IteratorRhs it_rhs;
        };
    }

    template <class IteratorRhs>
    inline auto zip(IteratorRhs &&it_rhs) -> details::ZipOp<IteratorRhs> {
        static_assert(!std::is_lvalue_reference<IteratorRhs>::value,
            "zip can only take second rvalue ref object with Iterator traits");

        return details::ZipOp<IteratorRhs>(std::move(it_rhs));
    }
}
