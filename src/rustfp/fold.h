#pragma once

#include "traits.h"
#include "util.h"

#include <type_traits>
#include <utility>

namespace rustfp {

    // implementation section

    namespace details {
        template <class Acc, class AccRhsToAcc>
        class FoldOp {
        public:
            template <class Accx, class AccRhsToAccx>
            FoldOp(Accx &&acc, AccRhsToAccx &&fn) :
                acc(std::forward<Accx>(acc)),
                fn(std::forward<AccRhsToAccx>(fn)) {

            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> Acc {
                static_assert(!std::is_lvalue_reference<Iterator>::value,
                    "fold can only take rvalue ref object with Iterator traits");

                auto op_acc = std::move(acc);

                while (true)
                {
                    auto next_opt = it.next();

                    if (next_opt.is_none()) {
                        break;
                    }

                    op_acc = fn(std::move(op_acc), std::move(next_opt).unwrap_unchecked());
                }

                return op_acc;
            }

        private:
            Acc acc;
            AccRhsToAcc fn;
        };
    }

    template <class Acc, class AccRhsToAcc>
    auto fold(Acc &&acc, AccRhsToAcc &&fn)
        -> details::FoldOp<special_decay_t<Acc>, special_decay_t<AccRhsToAcc>> {

        return details::FoldOp<
            special_decay_t<Acc>,
            special_decay_t<AccRhsToAcc>>(std::forward<Acc>(acc), std::forward<AccRhsToAcc>(fn));
    }
}
