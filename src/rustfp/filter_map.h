#pragma once

#include "option.h"
#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {
    
    // implementation section

    namespace details {
        template <class Iterator, class OptPred>
        class FilterMap {
        public:
            using Item = typename std::result_of_t<OptPred(typename Iterator::Item)>::some_t;

            template <class Iteratorx, class OptPredx>
            FilterMap(Iteratorx &&it, OptPredx &&opt_pred) :
                it(std::forward<Iteratorx>(it)),
                opt_pred(std::forward<OptPredx>(opt_pred)) {

            }

            auto next() -> Option<Item> {
                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    auto mapped_opt = opt_pred(std::move(next_opt).unwrap_unchecked());

                    if (mapped_opt.is_some()) {
                        return std::move(mapped_opt);
                    }
                }

                return None;
            }

        private:
            Iterator it;
            OptPred opt_pred;
        };

        template <class OptPred>
        class FilterMapOp {
        public:
            template <class OptPredx>
            explicit FilterMapOp(OptPredx &&opt_pred) :
                opt_pred(std::forward<OptPredx>(opt_pred)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> FilterMap<Iterator, OptPred> {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "filter_map can only take rvalue ref object with Iterator traits");

                return FilterMap<Iterator, OptPred>(std::move(it), std::move(opt_pred));
            }

        private:
            OptPred opt_pred;
        };
    }

    template <class OptPred>
    auto filter_map(OptPred &&opt_pred) -> details::FilterMapOp<special_decay_t<OptPred>>
    {
        return details::FilterMapOp<special_decay_t<OptPred>>(std::forward<OptPred>(opt_pred));
    }
}
