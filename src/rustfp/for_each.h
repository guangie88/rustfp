#pragma once

#include "pipe.h"
#include "unit.h"

#include <type_traits>
#include <utility>

namespace rustfp
{
    namespace details
    {
        template <class FnToVoid>
        class ForEachOp
        {
        public:
            template <class FnToVoidx>
            ForEachOp(FnToVoidx &&fn) :
                fn(std::forward<FnToVoidx>(fn))
            {
            }

            template <class Iterator>
            auto operator()(Iterator &&it) && -> unit_t
            {
                static_assert(!std::is_lvalue_reference<Iterator>::value, "for_each can only take rvalue ref object with Iterator traits");

                while (true)
                {
                    auto valueOpt = it.next();

                    if (valueOpt.is_none())
                    {
                        break;
                    }

                    fn(valueOpt.unwrap_unchecked());
                }

                return Unit;
            }

        private:
            FnToVoid fn;
        };
    }

    template <class FnToVoid>
    auto for_each(FnToVoid &&fn) -> details::ForEachOp<std::remove_reference_t<std::remove_const_t<FnToVoid>>>
    {
        return details::ForEachOp<std::remove_reference_t<std::remove_const_t<FnToVoid>>>(std::forward<FnToVoid>(fn));
    }
}
