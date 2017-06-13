#pragma once

#include "result.h"
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

        template <class OkInto, class ErrType>
        class CollectOp<Result<OkInto, ErrType>> {
        public:
            /**
             * Accept only Result<container type, error type>, where the
             * container type must have either push_back, insert or push
             * method.
             * @param it moved rustfp iterator.
             * @return Ok(container type) if no error while collecting,
             * Err(error type) upon the first error encountered while
             * collecting. Order of insertion is done via the order of .next().
             */
            template <class Iterator>
            auto operator()(Iterator &&it) && -> Result<OkInto, ErrType>;
        };

        // implementation section

        namespace details {
            template <class Into, class Item>
            auto inserter(decltype(std::declval<Into>().push_back(
                std::declval<typename Into::value_type>())) * = nullptr) {

                return [](Into &container, Item item) { 
                    return container.push_back(std::forward<Item>(item));
                };
            }

            template <class Into, class Item>
            auto inserter(decltype(std::declval<Into>().insert(
                std::declval<typename Into::value_type>())) * = nullptr) {

                return [](Into &container, Item item) { 
                    return container.insert(std::forward<Item>(item));
                };
            }

            template <class Into, class Item>
            auto inserter(decltype(std::declval<Into>().push(
                std::declval<typename Into::value_type>())) * = nullptr) {

                return [](Into &container, Item item) { 
                    return container.push(std::forward<Item>(item));
                };
            }

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
                details::inserter<Intox, typename Iterator::Item>());
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
                details::inserter<Intox, typename Iterator::Item>());
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
                details::inserter<Intox, typename Iterator::Item>());
        }

        template <class OkInto, class ErrType>
        template <class Iterator>
        auto CollectOp<Result<OkInto, ErrType>>::operator()(Iterator &&it) &&
            -> Result<OkInto, ErrType> {

            using Item = typename Iterator::Item;
            using iter_err_t = typename Item::err_t;

            static_assert(!std::is_lvalue_reference<Iterator>::value,
                "CollectOp<Result<OkInto, Err>> for types with push method "
                "can only take rvalue ref object with Iterator traits");

            const auto insert_fn = details::inserter<OkInto, typename OkInto::value_type>();
            OkInto container;

            while (true) {
                auto next_res_opt = it.next();

                if (next_res_opt.is_none()) {
                    break;
                }

                auto next_res = std::move(next_res_opt).unwrap_unchecked();

                if (next_res.is_err()) {
                    // because error type may be a reference
                    // must perform reverse decay here since Err will special
                    // decay the given type
                    return Err(reverse_decay_t<iter_err_t>(
                        std::move(next_res).unwrap_err_unchecked()));
                }

                insert_fn(container, std::move(next_res).unwrap_unchecked());
            }

            return Ok(std::move(container));
        }
    }

    template <class Into>
    auto collect() -> details::CollectOp<Into> {
        return details::CollectOp<Into>();
    }
}
