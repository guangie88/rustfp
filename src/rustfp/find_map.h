#pragma once

#include "option.h"
#include "traits.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class OptPred, class Iterator>
        auto find_map_op_impl(OptPred &&opt_pred, Iterator &&it) -> std::result_of_t<OptPred(typename Iterator::Item)>
        {
            auto next_opt = it.next();

            if (next_opt.is_none())
            {
                return None;
            }

            auto mapped_opt = opt_pred(next_opt.unwrap_unchecked());

            return mapped_opt.is_some()
                ? std::move(mapped_opt)
                : find_map_op_impl(std::move(opt_pred), std::move(it));
        }

        template <class OptPred>
        class FindMapOp
        {
        public:
            template <class OptPredx>
            explicit FindMapOp(OptPredx &&opt_pred) :
                opt_pred(std::forward<OptPredx>(opt_pred))
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> std::result_of_t<OptPred(typename Iterator::Item)>
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "find_map can only take rvalue ref object with Iterator traits");
                return find_map_op_impl(std::move(opt_pred), std::move(it));
            }

        private:
            OptPred opt_pred;
        };
    }

    template <class OptPred>
    auto find_map(OptPred &&opt_pred) -> details::FindMapOp<special_decay_t<OptPred>>
    {
        return details::FindMapOp<special_decay_t<OptPred>>(std::forward<OptPred>(opt_pred));
    }
}