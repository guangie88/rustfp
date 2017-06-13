#pragma once

#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>
#include <vector>

namespace rustfp {
    namespace details {
        
        // declaration section

        template <class Into>
        class CollectOp {
        public:
            /**
             * Uses expression SFINAE to accept only container types with method
             * push_back(value) to collect the values into.
             * @param it moved rustfp iterator.
             * @return specified container type with all the values collected
             * via push_back. Order of insertion is done via the order of .next().
             */
            template <class Iterator, class Intox = Into>
            auto operator()(
                Iterator &&it,
                decltype(std::declval<Intox>().push_back(
                    std::declval<typename Intox::value_type>())) * = nullptr) && -> Intox;

            /**
             * Uses expression SFINAE to accept only container types with method
             * insert(value) to collect the values into.
             * @param it moved rustfp iterator.
             * @return specified container type with all the values collected
             * via insert. Order of insertion is done via the order of .next() and
             * collecting into set types can remove any subsequent repeated entries.
             */
            template <class Iterator, class Intox = Into>
            auto operator()(
                Iterator &&it,
                decltype(std::declval<Intox>().insert(
                    std::declval<typename Intox::value_type>())) * = nullptr) && -> Intox;

            /**
             * Uses expression SFINAE to accept only container types with method
             * push(value) to collect the values into.
             * @param it moved rustfp iterator.
             * @return specified container type with all the values collected
             * via insert. Order of insertion is done via the order of .next().
             */
            template <class Iterator, class Intox = Into>
            auto operator()(
                Iterator &&it,
                decltype(std::declval<Intox>().push(
                    std::declval<typename Intox::value_type>())) * = nullptr) && -> Intox;
        };

        // implementation section

        namespace details {
            template <class Into, class Iterator, class InsertFn>
            auto collect_impl(Iterator &&it, InsertFn &&insert_fn) -> Into {
                Into container;

                while (true) {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    insert_fn(container, std::move(next_opt).unwrap_unchecked());
                }

                return container;
            }
        }

        template <class Into>
        template <class Iterator, class Intox>
        auto CollectOp<Into>::operator()(
            Iterator &&it,
            decltype(std::declval<Intox>().push_back(
                std::declval<typename Intox::value_type>())) *) && -> Intox {

            static_assert(!std::is_lvalue_reference<Iterator>::value,
                "CollectOp<Into> for types with push_back method "
                "can only take rvalue ref object with Iterator traits");

            return details::collect_impl<Intox>(std::move(it),
                [](Intox &container, typename Iterator::Item item) {
                    container.push_back(std::forward<typename Iterator::Item>(item));
                });
        }

        template <class Into>
        template <class Iterator, class Intox>
        auto CollectOp<Into>::operator()(
            Iterator &&it,
            decltype(std::declval<Intox>().insert(
                std::declval<typename Intox::value_type>())) *) && -> Intox {

            static_assert(!std::is_lvalue_reference<Iterator>::value,
                "CollectOp<Into> for types with insert method "
                "can only take rvalue ref object with Iterator traits");

            return details::collect_impl<Intox>(std::move(it),
                [](Intox &container, typename Iterator::Item item) {
                    container.insert(std::forward<typename Iterator::Item>(item));
                });
        }

        template <class Into>
        template <class Iterator, class Intox>
        auto CollectOp<Into>::operator()(
            Iterator &&it,
            decltype(std::declval<Intox>().push(
                std::declval<typename Intox::value_type>())) *) && -> Intox {

            static_assert(!std::is_lvalue_reference<Iterator>::value,
                "CollectOp<Into> for types with push method "
                "can only take rvalue ref object with Iterator traits");

            return details::collect_impl<Intox>(std::move(it),
                [](Intox &container, typename Iterator::Item item) {
                    container.push(std::forward<typename Iterator::Item>(item));
                });
        }
    }

    template <class Into>
    auto collect() -> details::CollectOp<Into> {
        return details::CollectOp<Into>();
    }
}
