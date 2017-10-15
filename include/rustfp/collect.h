/**
 * Contains Rust Iterator collect equivalent implementation
 * collect function: https://doc.rust-lang.org/std/iter/trait.Iterator.html#method.collect
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#include "result.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>
#include <vector>

namespace rustfp {

    // declaration section

    template <class B>
    class CollectOp {
    public:
        /**
         * Use expression SFINAE to accept only container types with method
         * push_back(value), insert(value) or push(value) to collect the values.
         * @param self moved rustfp iterator.
         * @return specified container type with all the values collected
         * via push_back. Order of insertion is done via the order of .next().
         */
        template <class Self>
        auto operator()(Self &&self) && -> B;
    };

    template <class OkType, class ErrType>
    class CollectOp<Result<OkType, ErrType>> {
    public:
        /**
         * Accept only Result<container type, error type>, where the
         * container type must have either push_back, insert or push
         * method.
         * @param self moved rustfp iterator.
         * @return Ok(container type) if no error while collecting,
         * Err(error type) upon the first error encountered while
         * collecting. Order of insertion is done via the order of .next().
         */
        template <class Self>
        auto operator()(Self &&self) && -> Result<OkType, ErrType>;
    };

    /**
     * fn collect<B>(self) -> B 
     * where
     *     B: FromIterator<Self::Item>,
     *
     * Collect into any container type that is able to invoke
     * push_back(value), insert(value) or push(value) method.
     * Can also collect into Result<container type, error type>.
     */
    template <class B>
    auto collect() -> CollectOp<B>;

    // implementation section

    namespace details {
        template <class B, class Item>
        auto inserter(decltype(std::declval<B>().push_back(
            std::declval<typename B::value_type>())) * = nullptr) {

            return [](B &container, Item item) { 
                return container.push_back(std::forward<Item>(item));
            };
        }

        template <class B, class Item>
        auto inserter(decltype(std::declval<B>().insert(
            std::declval<typename B::value_type>())) * = nullptr,
            void * = nullptr) {

            return [](B &container, Item item) { 
                return container.insert(std::forward<Item>(item));
            };
        }

        template <class B, class Item>
        auto inserter(decltype(std::declval<B>().push(
            std::declval<typename B::value_type>())) * = nullptr,
            void * = nullptr,
            void * = nullptr) {

            return [](B &container, Item item) { 
                return container.push(std::forward<Item>(item));
            };
        }

        template <class B, class Self, class InsertFn>
        auto collect_impl(Self &&self, InsertFn &&insert_fn) -> B {
            B container;

            while (true) {
                auto next_opt = self.next();

                if (next_opt.is_none()) {
                    break;
                }

                insert_fn(container, std::move(next_opt).unwrap_unchecked());
            }

            return container;
        }
    }

    template <class B>
    template <class Self>
    auto CollectOp<B>::operator()(Self &&self) && -> B {
        static_assert(!std::is_lvalue_reference<Self>::value,
            "CollectOp<B> for types with push_back, insert or push method "
            "can only take rvalue ref object with Iterator traits");

        return details::collect_impl<B>(std::move(self),
            details::inserter<B, typename Self::Item>());
    }

    template <class OkType, class ErrType>
    template <class Self>
    auto CollectOp<Result<OkType, ErrType>>::operator()(Self &&self) &&
        -> Result<OkType, ErrType> {

        static_assert(!std::is_lvalue_reference<Self>::value,
            "CollectOp<Result<OkType, Err>> for types with push method "
            "can only take rvalue ref object with Iterator traits");

        const auto insert_fn = details::inserter<OkType, typename OkType::value_type>();
        OkType container;

        while (true) {
            auto next_res_opt = self.next();

            if (next_res_opt.is_none()) {
                break;
            }

            auto next_res = std::move(next_res_opt).unwrap_unchecked();

            if (next_res.is_err()) {
                // because error type may be a reference
                // must perform reverse decay here since Err will special
                // decay the given type
                return Err(reverse_decay(
                    std::move(next_res).unwrap_err_unchecked()));
            }

            insert_fn(container, std::move(next_res).unwrap_unchecked());
        }

        return Ok(std::move(container));
    }

    template <class B>
    auto collect() -> CollectOp<B> {
        return CollectOp<B>();
    }
}
