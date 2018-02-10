#include "catch.hpp"

#define RUSTFP_SIMPLIFIED_LET
#include "rustfp/all.h"
#include "rustfp/any.h"
#include "rustfp/cloned.h"
#include "rustfp/collect.h"
#include "rustfp/cycle.h"
#include "rustfp/enumerate.h"
#include "rustfp/filter.h"
#include "rustfp/filter_map.h"
#include "rustfp/find.h"
#include "rustfp/find_map.h"
#include "rustfp/flat_map.h"
#include "rustfp/fold.h"
#include "rustfp/for_each.h"
#include "rustfp/iter.h"
#include "rustfp/let.h"
#include "rustfp/map.h"
#include "rustfp/max.h"
#include "rustfp/min.h"
#include "rustfp/once.h"
#include "rustfp/option.h"
#include "rustfp/range.h"
#include "rustfp/result.h"
#include "rustfp/skip.h"
#include "rustfp/take.h"
#include "rustfp/unit.h"
#include "rustfp/zip.h"

#include <algorithm>
#include <array>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// rustfp
using rustfp::all;
using rustfp::any;
using rustfp::cloned;
using rustfp::collect;
using rustfp::cycle;
using rustfp::enumerate;
using rustfp::filter;
using rustfp::filter_map;
using rustfp::find;
using rustfp::find_map;
using rustfp::flat_map;
using rustfp::fold;
using rustfp::for_each;
using rustfp::into_iter;
using rustfp::iter;
using rustfp::iter_begin_end;
using rustfp::iter_mut;
using rustfp::map;
using rustfp::max;
using rustfp::max_by;
using rustfp::min;
using rustfp::min_by;
using rustfp::once;
using rustfp::range;
using rustfp::skip;
using rustfp::take;
using rustfp::zip;

using rustfp::Unit;
using rustfp::unit_t;

using rustfp::None;
using rustfp::opt_if;
using rustfp::Option;
using rustfp::Some;

using rustfp::Err;
using rustfp::Ok;
using rustfp::res_if_else;
using rustfp::Result;

// std
using std::accumulate;
using std::array;
using std::cbegin;
using std::cend;
using std::cout;
using std::cref;
using std::deque;
using std::extent;
using std::forward;
using std::is_same;
using std::list;
using std::make_unique;
using std::mismatch;
using std::move;
using std::pair;
using std::plus;
using std::queue;
using std::ref;
using std::reference_wrapper;
using std::remove_reference_t;
using std::set;
using std::stack;
using std::string;
using std::stringstream;
using std::to_string;
using std::unique_ptr;
using std::unordered_map;
using std::unordered_set;
using std::vector;

// helper functions

namespace details {
template <class IterableLhs, class IterableRhs>
auto no_mismatch_values(IterableLhs &&lhs, IterableRhs &&rhs) -> bool {
    const auto input_it_pairs = mismatch(cbegin(lhs), cend(lhs), cbegin(rhs));

    return input_it_pairs.first == lhs.cend()
           && input_it_pairs.second == rhs.cend();
}

template <class IterableLhs, class IterableRhs, class CompFn>
auto no_mismatch_values(IterableLhs &&lhs, IterableRhs &&rhs, CompFn &&comp_fn)
    -> bool {
    const auto input_it_pairs =
        mismatch(cbegin(lhs), cend(lhs), cbegin(rhs), forward<CompFn>(comp_fn));

    return input_it_pairs.first == lhs.cend()
           && input_it_pairs.second == rhs.cend();
}

// weaker than no_mismatch_values,
// does not check for ordering and possibly repeated entries
template <class IterableLhs, class IterableRhs>
auto similar_values(IterableLhs &&lhs, IterableRhs &&rhs) -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (const auto &l_val : lhs) {
        if (std::find(cbegin(rhs), cend(rhs), l_val) == rhs.cend()) {
            return false;
        }
    }

    return true;
}

template <class IterableLhs, class IterableRhs, class CompFn>
auto similar_values(IterableLhs &&lhs, IterableRhs &&rhs, CompFn &&comp_fn)
    -> bool {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (const auto &l_val : lhs) {
        const auto unary_pred = [&l_val, &comp_fn](const auto &r_val) {
            return comp_fn(l_val, r_val);
        };

        if (std::find_if(cbegin(rhs), cend(rhs), unary_pred) == rhs.cend()) {
            return false;
        }
    }

    return true;
}
} // namespace details

// simple ops

TEST_CASE("Ops section", "[Ops]") {
    const auto int_vec = vector<int>{0, 1, 2, 3, 4, 5};

    const auto str_vec =
        vector<string>{"Hello", "World", "How", "Are", "You", "?"};

    SECTION("Iter") {
        auto it = iter(int_vec);

        static_assert(
            is_same<
                typename remove_reference_t<decltype(it)>::Item,
                const int &>::value,
            "Ops::Iter failed Iter type checking!");

        const auto opt0 = it.next();
        REQUIRE(opt0.is_some());
        REQUIRE(0 == opt0.get_unchecked());

        static_assert(
            is_same<
                typename remove_reference_t<decltype(opt0)>::some_t,
                const int &>::value,
            "Ops::Iter failed Option type checking!");

        const auto opt1 = it.next();
        REQUIRE(opt1.is_some());
        REQUIRE(1 == opt1.get_unchecked());

        const auto opt2 = it.next();
        REQUIRE(opt2.is_some());
        REQUIRE(2 == opt2.get_unchecked());

        const auto opt3 = it.next();
        REQUIRE(opt3.is_some());
        REQUIRE(3 == opt3.get_unchecked());

        const auto opt4 = it.next();
        REQUIRE(opt4.is_some());
        REQUIRE(4 == opt4.get_unchecked());

        const auto opt5 = it.next();
        REQUIRE(opt5.is_some());
        REQUIRE(5 == opt5.get_unchecked());

        const auto opt6 = it.next();
        REQUIRE(opt6.is_none());
    }

    SECTION("IterMut") {
        auto v = int_vec;

        // modify the values
        iter_mut(v) | map([](auto &val) {
            static_assert(
                is_same<decltype(val), int &>::value,
                "val is expected to be of int & type");

            val *= val;
            return ref(val);
        }) | for_each([](auto &val) {
            static_assert(
                is_same<decltype(val), int &>::value,
                "val is expected to be of int & type");

            val += 1;
        });

        auto it = iter_mut(v);

        static_assert(
            is_same<typename remove_reference_t<decltype(it)>::Item, int &>::
                value,
            "Ops::IterMut failed Iter type checking!");

        const auto opt0 = it.next();
        REQUIRE(opt0.is_some());
        REQUIRE(1 == opt0.get_unchecked());

        static_assert(
            is_same<
                typename remove_reference_t<decltype(opt0)>::some_t,
                int &>::value,
            "Ops::IterMut failed Option type checking!");

        const auto opt1 = it.next();
        REQUIRE(opt1.is_some());
        REQUIRE(2 == opt1.get_unchecked());

        const auto opt2 = it.next();
        REQUIRE(opt2.is_some());
        REQUIRE(5 == opt2.get_unchecked());

        const auto opt3 = it.next();
        REQUIRE(opt3.is_some());
        REQUIRE(10 == opt3.get_unchecked());

        const auto opt4 = it.next();
        REQUIRE(opt4.is_some());
        REQUIRE(17 == opt4.get_unchecked());

        const auto opt5 = it.next();
        REQUIRE(opt5.is_some());
        REQUIRE(26 == opt5.get_unchecked());

        const auto opt6 = it.next();
        REQUIRE(opt6.is_none());
    }

    SECTION("IterMutChain") {
        auto v = int_vec;

        // modify the values
        const auto v2 = iter_mut(v) | filter([](const auto val) {
                            static_assert(
                                is_same<decltype(val), const int>::value,
                                "val is expected to be of const int type");

                            return val >= 2;
                        })
                        | map([](auto &val) {
                              static_assert(
                                  is_same<decltype(val), int &>::value,
                                  "val is expected to be of int & type");

                              val -= 2;
                              return ref(val);
                          })
                        | collect<vector<reference_wrapper<int>>>();

        REQUIRE(4 == v2.size());
        REQUIRE(0 == v2.at(0));
        REQUIRE(1 == v2.at(1));
        REQUIRE(2 == v2.at(2));
        REQUIRE(3 == v2.at(3));
    }

    SECTION("IntoIter") {
        vector<unique_ptr<int>> v;
        v.push_back(make_unique<int>(0));
        v.push_back(make_unique<int>(1));
        v.push_back(make_unique<int>(2));

        auto it = into_iter(move(v));

        static_assert(
            is_same<
                typename remove_reference_t<decltype(it)>::Item,
                unique_ptr<int>>::value,
            "Ops::IntoIter failed IntoIter type checking!");

        auto opt0 = it.next();

        static_assert(
            is_same<
                typename remove_reference_t<decltype(opt0)>::some_t,
                unique_ptr<int>>::value,
            "Ops::IntoIter failed Option type checking!");

        REQUIRE(opt0.is_some());
        auto val0(move(opt0).unwrap_unchecked());
        REQUIRE(0 == *val0);

        auto opt1 = it.next();
        REQUIRE(opt1.is_some());
        auto val1(move(opt1).unwrap_unchecked());
        REQUIRE(1 == *val1);

        auto opt2 = it.next();
        REQUIRE(opt2.is_some());
        auto val2(move(opt2).unwrap_unchecked());
        REQUIRE(2 == *val2);

        REQUIRE(it.next().is_none());
    }

    SECTION("IterBeginEndVec") {
        vector<unique_ptr<int>> v;
        v.push_back(make_unique<int>(0));
        v.push_back(make_unique<int>(1));
        v.push_back(make_unique<int>(2));

        auto it = iter_begin_end(v.begin(), v.cend());

        static_assert(
            is_same<
                typename remove_reference_t<decltype(it)>::Item,
                const unique_ptr<int> &>::value,
            "Ops::IterBeginEndVec failed IterBeginEnd type checking!");

        auto opt0 = it.next();

        static_assert(
            is_same<
                typename remove_reference_t<decltype(opt0)>::some_t,
                const unique_ptr<int> &>::value,
            "Ops::IterBeginEndVec failed Option type checking!");

        REQUIRE(opt0.is_some());
        const auto &val0 = opt0.get_unchecked();
        REQUIRE(0 == *val0);

        auto opt1 = it.next();
        REQUIRE(opt1.is_some());
        const auto &val1 = opt1.get_unchecked();
        REQUIRE(1 == *val1);

        auto opt2 = it.next();
        REQUIRE(opt2.is_some());
        const auto &val2 = opt2.get_unchecked();
        REQUIRE(2 == *val2);

        REQUIRE(it.next().is_none());
    }

    SECTION("IterBeginEndPtrs") {
        // note the length is 4 since there is null character
        const char VALUES[] = {"bad"};
        auto it = iter_begin_end(
            VALUES + 0, VALUES + extent<decltype(VALUES)>::value);

        static_assert(
            is_same<
                typename remove_reference_t<decltype(it)>::Item,
                const char &>::value,
            "Ops::IterBeginEndPtrs failed IterBeginEnd type checking!");

        auto opt0 = it.next();

        static_assert(
            is_same<
                typename remove_reference_t<decltype(opt0)>::some_t,
                const char &>::value,
            "Ops::IterBeginEndPtrs failed Option type checking!");

        REQUIRE(opt0.is_some());
        const auto &val0 = opt0.get_unchecked();
        REQUIRE('b' == val0);

        auto opt1 = it.next();
        REQUIRE(opt1.is_some());
        const auto &val1 = opt1.get_unchecked();
        REQUIRE('a' == val1);

        auto opt2 = it.next();
        REQUIRE(opt2.is_some());
        const auto &val2 = opt2.get_unchecked();
        REQUIRE('d' == val2);

        auto opt3 = it.next();
        REQUIRE(opt3.is_some());
        const auto &val3 = opt3.get_unchecked();
        REQUIRE('\0' == val3);

        REQUIRE(it.next().is_none());
    }

    SECTION("AllTrue") {
        const auto result =
            iter(int_vec) | all([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value < 6;
            });

        REQUIRE(result);
    }

    SECTION("AllFalse") {
        const auto result =
            iter(int_vec) | all([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value > 0;
            });

        REQUIRE(!result);
    }

    SECTION("AnyTrue") {
        const auto result =
            iter(int_vec) | any([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == 5;
            });

        REQUIRE(result);
    }

    SECTION("AnyFalse") {
        const auto result =
            iter(int_vec) | any([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == 7;
            });

        REQUIRE(!result);
    }

    SECTION("ClonedRef") {
        const auto str_dup_vec =
            iter(str_vec) | cloned() | collect<vector<string>>();

        REQUIRE(details::no_mismatch_values(
            str_vec, str_dup_vec, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const string &>::value,
                    "lhs is expected to be of const string & type");

                static_assert(
                    is_same<decltype(rhs), const string &>::value,
                    "rhs is expected to be of const string & type");

                // same value but different addresses
                return (lhs == rhs) && (&lhs != &rhs);
            }));
    }

    SECTION("ClonedValue") {
        const auto int_str_vec =
            iter(int_vec) | map([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return to_string(value);
            })
            | cloned() | cloned() | cloned() | collect<vector<string>>();

        REQUIRE(details::no_mismatch_values(
            int_vec, int_str_vec, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const int &>::value,
                    "lhs is expected to be of const int & type");

                static_assert(
                    is_same<decltype(rhs), const string &>::value,
                    "rhs is expected to be of const string & type");

                return to_string(lhs) == rhs;
            }));
    }

    SECTION("CollectVec") {
        const auto dup_vec = range(0, int_vec.size()) | collect<vector<int>>();

        REQUIRE(details::no_mismatch_values(int_vec, dup_vec));
    }

    SECTION("CollectList") {
        const auto dup_cont =
            iter(int_vec) | collect<list<reference_wrapper<const int>>>();

        REQUIRE(details::no_mismatch_values(int_vec, dup_cont));
    }

    SECTION("CollectSet") {
        const auto dup_cont =
            iter(int_vec) | collect<set<reference_wrapper<const int>>>();

        REQUIRE(details::no_mismatch_values(int_vec, dup_cont));
    }

    SECTION("CollectUnorderedSet") {
        // unordered_set cannot take in reference_wrapper as template type by
        // default
        const auto dup_cont = iter(int_vec) | collect<unordered_set<int>>();

        REQUIRE(details::similar_values(int_vec, dup_cont));
    }

    SECTION("CollectMap") {
        const auto dup_cont = iter(int_vec) | zip(iter(str_vec))
                              | collect<std::map<
                                    reference_wrapper<const int>,
                                    reference_wrapper<const string>>>();

        // check the keys
        REQUIRE(details::no_mismatch_values(
            int_vec, dup_cont, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const int &>::value,
                    "lhs is expected to be of const int & type");

                static_assert(
                    is_same<
                        decltype(rhs),
                        const pair<
                            const reference_wrapper<const int>,
                            reference_wrapper<const string>> &>::value,
                    "rhs is expected to be of const pair<...> & type");

                return lhs == rhs.first.get();
            }));

        // check the values
        const auto dup_vals =
            iter(dup_cont) | map([](const auto &p) {
                static_assert(
                    is_same<
                        decltype(p),
                        const pair<
                            const reference_wrapper<const int>,
                            reference_wrapper<const string>> &>::value,
                    "lhs is expected to be of const pair<...> & type");

                return cref(p.second);
            })
            | collect<vector<reference_wrapper<const string>>>();

        REQUIRE(details::no_mismatch_values(
            str_vec, dup_vals, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const string &>::value,
                    "lhs is expected to be of const string & type");

                static_assert(
                    is_same<
                        decltype(rhs),
                        const reference_wrapper<const string> &>::value,
                    "rhs is expected to be of const ref<const string> & type");

                return lhs == rhs.get();
            }));
    }

    SECTION("CollectUnorderedMap") {
        const auto dup_cont =
            iter(int_vec) | zip(iter(str_vec))
            | collect<unordered_map<int, reference_wrapper<const string>>>();

        // check the keys
        const auto dup_keys =
            iter(dup_cont) | map([](const auto &p) {
                static_assert(
                    is_same<
                        decltype(p),
                        const pair<const int, reference_wrapper<const string>>
                            &>::value,
                    "p is expected to be of const pair<...> & type");

                return cref(p.first);
            })
            | collect<vector<reference_wrapper<const int>>>();

        REQUIRE(details::similar_values(int_vec, dup_keys));

        // check the values
        const auto dup_vals =
            iter(dup_cont) | map([](const auto &p) {
                static_assert(
                    is_same<
                        decltype(p),
                        const pair<const int, reference_wrapper<const string>>
                            &>::value,
                    "p is expected to be of const pair<...> & type");

                return cref(p.second);
            })
            | collect<vector<reference_wrapper<const string>>>();

        REQUIRE(details::similar_values(
            str_vec, dup_vals, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const string &>::value,
                    "lhs is expected to be of const string & type");

                static_assert(
                    is_same<
                        decltype(rhs),
                        const reference_wrapper<const string> &>::value,
                    "rhs is expected to be of const ref<const string> & type");

                return lhs == rhs.get();
            }));
    }

    SECTION("CollectStack") {
        auto dup_cont =
            iter(int_vec) | collect<stack<reference_wrapper<const int>>>();

        while (!dup_cont.empty()) {
            const auto top = dup_cont.top();
            REQUIRE(int_vec[dup_cont.size() - 1] == top);
            dup_cont.pop();
        }
    }

    SECTION("CollectQueue") {
        auto dup_cont =
            iter(int_vec) | collect<queue<reference_wrapper<const int>>>();

        for (size_t i = 0; i < int_vec.size(); ++i) {
            const auto front = dup_cont.front();
            REQUIRE(int_vec[i] == front);
            dup_cont.pop();
        }
    }

    SECTION("CollectResultOk") {
        vector<Result<int, string>> res_vec{Ok(0), Ok(1), Ok(2)};

        auto collected_res =
            into_iter(move(res_vec)) | collect<Result<vector<int>, string>>();

        REQUIRE(collected_res.is_ok());

        const auto collected = move(collected_res).unwrap_unchecked();
        REQUIRE(3 == collected.size());
        REQUIRE(0 == collected.at(0));
        REQUIRE(1 == collected.at(1));
        REQUIRE(2 == collected.at(2));
    }

    SECTION("CollectResultErr") {
        const int zero = 0;
        const int two = 2;
        const string err_msg = "ERROR!";

        vector<Result<const int &, const string &>> res_vec{
            Ok(cref(zero)), Err(cref(err_msg)), Ok(cref(two))};

        auto collected_res = into_iter(move(res_vec))
                             | collect<Result<
                                   vector<reference_wrapper<const int>>,
                                   const string &>>();

        REQUIRE(collected_res.is_err());

        const auto collected = move(collected_res).unwrap_err_unchecked();
        REQUIRE(err_msg == collected);
    }

    SECTION("CollectVecRef") {
        const auto str_ref_vec =
            iter(str_vec) | collect<vector<reference_wrapper<const string>>>();

        REQUIRE(details::no_mismatch_values(
            str_vec, str_ref_vec, [](const auto &lhs, const auto &rhs) {
                static_assert(
                    is_same<decltype(lhs), const string &>::value,
                    "lhs is expected to be of const string & type");

                static_assert(
                    is_same<
                        decltype(rhs),
                        const reference_wrapper<const string> &>::value,
                    "rhs is expected to be of const ref<const string> & type");

                return &lhs == &rhs.get();
            }));
    }

    SECTION("CollectMapVecSum") {
        const auto dup_vec =
            range(0, int_vec.size()) | map([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value + 0.5;
            })
            | collect<vector<double>>();

        const auto fold_sum = iter(dup_vec) | fold(0.0, plus<double>());

        const auto expected_sum = accumulate(
            cbegin(int_vec),
            cend(int_vec),
            0.0,
            [](const auto acc, const auto value) {
                static_assert(
                    is_same<decltype(acc), const double>::value,
                    "acc is expected to be of const double type");

                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return acc + value + 0.5;
            });

        REQUIRE(expected_sum == fold_sum);
    }

    SECTION("Cycle") {
        const vector<string> vs{"Hello", "World"};

        const auto output_vs =
            iter(vs) | cycle() | take(5) | collect<vector<string>>();

        REQUIRE(5 == output_vs.size());
        REQUIRE("Hello" == output_vs[0]);
        REQUIRE("World" == output_vs[1]);
        REQUIRE("Hello" == output_vs[2]);
        REQUIRE("World" == output_vs[3]);
        REQUIRE("Hello" == output_vs[4]);
    }

    SECTION("CycleNone") {
        const vector<string> vs{};
        const auto output_vs = iter(vs) | cycle() | collect<vector<string>>();
        REQUIRE(output_vs.empty());
    }

    SECTION("Once") {
        auto movable_value = make_unique<string>("Hello");

        const auto vec = once(move(movable_value)) | take(5)
                         | collect<vector<unique_ptr<string>>>();

        REQUIRE(1 == vec.size());
        REQUIRE("Hello" == *vec[0]);
    }

    SECTION("Enumerate") {
        int sum = 0;

        iter(int_vec) | enumerate() | for_each([&sum](const auto &p) {
            static_assert(
                is_same<decltype(p), const pair<size_t, const int &> &>::value,
                "p is expected to be of const pair<...> & type");

            // first and second are the same values
            sum += p.first + p.second;
        });

        // * 2 because the index and values are the same values
        // thus adding both together create a * 2 effect
        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 0) * 2 == sum);
    }

    SECTION("Filter") {
        int sum = 0;

        iter(int_vec) | filter([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 2 == 1;
        }) | for_each([&sum](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            sum += value;
        });

        REQUIRE(9 == sum);
    }

    SECTION("FilterMap") {
        double sum = 0;

        iter(int_vec) | filter_map([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 2 == 1 ? Some(value + 0.5) : None;
        }) | for_each([&sum](const auto value) {
            static_assert(
                is_same<decltype(value), const double>::value,
                "value is expected to be of const double type");

            sum += value;
        });

        REQUIRE(10.5 == sum);
    }

    SECTION("FindSome") {
        const auto find_some_opt =
            iter(int_vec) | find([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == 5;
            });

        REQUIRE(find_some_opt.is_some());
        REQUIRE(5 == find_some_opt.get_unchecked());
    }

    SECTION("FindNone") {
        const auto find_none_opt =
            iter(int_vec) | find([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == 6;
            });

        REQUIRE(find_none_opt.is_none());
    }

    SECTION("FindMapSome") {
        const auto find_some_opt =
            iter(int_vec) | find_map([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == 4 ? Some(value + 0.5) : None;
            });

        REQUIRE(find_some_opt.is_some());
        REQUIRE(4.5 == find_some_opt.get_unchecked());
    }

    SECTION("FindMapNone") {
        const auto find_none_opt =
            iter(int_vec) | find_map([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value == -1 ? Some(value) : None;
            });

        REQUIRE(find_none_opt.is_none());
    }

    SECTION("FlatMapEmpty") {
        auto vv = vector<vector<int>>{};

        const auto v_out =
            into_iter(move(vv)) | flat_map([](auto &&v) {
                static_assert(
                    is_same<decltype(v), vector<int> &&>::value,
                    "v is expected to be of vector<int> && type");

                return move(v);
            })
            | collect<vector<int>>();

        REQUIRE(v_out.empty());
    }

    SECTION("FlatMapSimple") {
        auto vv = vector<vector<int>>{{}, {0, 1, 2}, {3}, {}, {4, 5}};

        const auto v_out =
            into_iter(move(vv)) | flat_map([](auto &&v) {
                static_assert(
                    is_same<decltype(v), vector<int> &&>::value,
                    "v is expected to be of vector<int> && type");

                return move(v);
            })
            | collect<vector<int>>();

        REQUIRE(details::no_mismatch_values(
            array<int, 6>{0, 1, 2, 3, 4, 5}, v_out));
    }

    SECTION("Fold") {
        const auto fold_sum = iter(int_vec) | fold(10, plus<int>());

        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 10) == fold_sum);
    }

    SECTION("ForEach") {
        int sum = 0;

        iter(int_vec) | for_each([&sum](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            sum += value;
        });

        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 0) == sum);
    }

    SECTION("Map") {
        double sum = 0.0;

        iter(int_vec) | map([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value * 0.5;
        }) | for_each([&sum](const auto value) {
            static_assert(
                is_same<decltype(value), const double>::value,
                "value is expected to be of const double type");

            sum += value;
        });

        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 0) * 0.5 == sum);
    }

    SECTION("MaxNone") {
        const vector<int> VALS;
        const auto max_opt = iter(VALS) | max();
        REQUIRE(max_opt.is_none());
    }

    SECTION("MaxSomeEasy") {
        const vector<string> VALS{"zzz", "hello", "world"};
        const auto max_opt = iter(VALS) | max();

        REQUIRE(max_opt.is_some());
        REQUIRE("zzz" == max_opt.get_unchecked());
    }

    SECTION("MaxSomeHard") {
        const vector<int> VALS{-5, 1, 777, 123, 25, 0, 777, 777};

        // hand coded max to get the address eventually
        REQUIRE(!VALS.empty());

        size_t max_index = 0;

        for (size_t i = 1; i < VALS.size(); ++i) {
            if (VALS[i] > VALS[max_index]) {
                max_index = i;
            }
        }

        // found the value and referencing it
        const auto &max_val = VALS[max_index];
        const auto max_opt = iter(VALS) | max();

        REQUIRE(max_opt.is_some());
        REQUIRE(777 == max_opt.get_unchecked());
        REQUIRE(&max_val == &max_opt.get_unchecked());
    }

    SECTION("MaxByNone") {
        const vector<int> VALS;

        const auto max_opt =
            iter(VALS) | max_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs >= rhs;
            });

        REQUIRE(max_opt.is_none());
    }

    SECTION("MaxBySomeEasy") {
        const vector<int> VALS{3, 1, 2};

        const auto max_opt =
            iter(VALS) | max_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs >= rhs;
            });

        REQUIRE(max_opt.is_some());
        REQUIRE(3 == max_opt.get_unchecked());
    }

    SECTION("MaxBySomeHard") {
        const vector<int> VALS{5, 2, 7, 3, 3, 2, 2, 9};

        // hand coded max to get the address eventually
        REQUIRE(!VALS.empty());

        size_t max_index = 0;

        for (size_t i = 1; i < VALS.size(); ++i) {
            if (VALS[i] > VALS[max_index]) {
                max_index = i;
            }
        }

        // found the value and referencing it
        const auto &max_val = VALS[max_index];

        const auto max_opt =
            iter(VALS) | max_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs >= rhs;
            });

        REQUIRE(max_opt.is_some());
        REQUIRE(9 == max_opt.get_unchecked());
        REQUIRE(&max_val == &max_opt.get_unchecked());
    }

    SECTION("MinNone") {
        const vector<int> VALS;
        const auto min_opt = iter(VALS) | min();

        REQUIRE(min_opt.is_none());
    }

    SECTION("MinSomeEasy") {
        const vector<string> VALS{"hello", "world", "zzz"};
        const auto min_opt = iter(VALS) | min();

        REQUIRE(min_opt.is_some());
        REQUIRE("hello" == min_opt.get_unchecked());
    }

    SECTION("MinSomeHard") {
        const vector<int> VALS{-5, 1, -777, 123, 25, 0, -777, -777};

        // hand coded min to get the address eventually
        REQUIRE(!VALS.empty());

        size_t min_index = 0;

        for (size_t i = 1; i < VALS.size(); ++i) {
            if (VALS[i] < VALS[min_index]) {
                min_index = i;
            }
        }

        // found the value and referencing it
        const auto &min_val = VALS[min_index];
        const auto min_opt = iter(VALS) | min();

        REQUIRE(min_opt.is_some());
        REQUIRE(-777 == min_opt.get_unchecked());
        REQUIRE(&min_val == &min_opt.get_unchecked());
    }

    SECTION("MinByNone") {
        const vector<int> VALS;

        const auto min_opt =
            iter(VALS) | min_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs <= rhs;
            });

        REQUIRE(min_opt.is_none());
    }

    SECTION("MinBySomeEasy") {
        const vector<int> VALS{1, 3, 2};

        const auto min_opt =
            iter(VALS) | min_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs <= rhs;
            });

        REQUIRE(min_opt.is_some());
        REQUIRE(1 == min_opt.get_unchecked());
    }

    SECTION("MinBySomeHard") {
        const vector<int> VALS{5, 2, 7, 3, 3, 2, 2, 9};

        // hand coded min to get the address eventually
        REQUIRE(!VALS.empty());

        size_t min_index = 0;

        for (size_t i = 1; i < VALS.size(); ++i) {
            if (VALS[i] < VALS[min_index]) {
                min_index = i;
            }
        }

        // found the value and referencing it
        const auto &min_val = VALS[min_index];

        const auto min_opt =
            iter(VALS) | min_by([](const auto lhs, const auto rhs) {
                static_assert(
                    is_same<decltype(lhs), const int>::value,
                    "lhs is expected to be of const int type");

                static_assert(
                    is_same<decltype(rhs), const int>::value,
                    "rhs is expected to be of const int type");

                return lhs <= rhs;
            });

        REQUIRE(min_opt.is_some());
        REQUIRE(2 == min_opt.get_unchecked());
        REQUIRE(&min_val == &min_opt.get_unchecked());
    }

    SECTION("Range") {
        const auto sum = range(0, 6) | fold(5, plus<int>());
        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 5) == sum);
    }

    SECTION("SkipWithin") {
        const auto sum = iter(int_vec) | skip(3) | fold(0, plus<int>());
        REQUIRE(12 == sum);
    }

    SECTION("SkipPass") {
        const auto sum = iter(int_vec) | skip(100) | fold(0, plus<int>());
        REQUIRE(0 == sum);
    }

    SECTION("TakeWithin") {
        const auto sum = iter(int_vec) | take(3) | fold(0, plus<int>());
        REQUIRE(3 == sum);
    }

    SECTION("TakeExceed") {
        const auto sum = iter(int_vec) | take(100) | fold(0, plus<int>());
        REQUIRE(accumulate(cbegin(int_vec), cend(int_vec), 0) == sum);
    }
}

// complex tests

TEST_CASE("Complex ops section", "[ComplexOps]") {
    const auto int_vec = vector<int>{0, 1, 2, 3, 4, 5};

    SECTION("OnceCycleFindMap") {
        const auto opt =
            once(Unit) | cycle() | find_map([](auto) { return Some("Hello"); });

        REQUIRE(opt.is_some());
        REQUIRE("Hello" == string(opt.get_unchecked()));
    }

    SECTION("ManyIterCollect") {
        vector<unique_ptr<int>> v;
        v.push_back(make_unique<int>(0));
        v.push_back(make_unique<int>(1));
        v.push_back(make_unique<int>(2));
        v.push_back(make_unique<int>(3));
        v.push_back(make_unique<int>(4));

        // collect as reference once
        const auto v2 =
            iter(v)
            | collect<vector<reference_wrapper<const unique_ptr<int>>>>();

        // collect as reference twice
        // note how reference_wrapper<reference_wrapper>
        // got collapsed back into just reference_wrapper
        // this is because c++ does not allow
        // reference_wrapper<reference_wrapper> so it is a must to collapse
        // anyway which makes a slightly deviation from Rust ref of ref (which
        // is possible)

        const auto v3 =
            iter(v2)
            | collect<vector<reference_wrapper<const unique_ptr<int>>>>();

        const auto v4 =
            iter(v3) | map([](const auto &r_ptr) {
                static_assert(
                    is_same<decltype(r_ptr), const unique_ptr<int> &>::value,
                    "r_ptr is expected to be of const unique_ptr<int> & type");

                return cref(*r_ptr);
            })
            | filter([](const auto &r) {
                  static_assert(
                      is_same<decltype(r), const int &>::value,
                      "r is expected to be of const int & type");

                  return r % 2 == 1;
              })
            | collect<vector<reference_wrapper<const int>>>();

        // no copy of any value was ever done from the original v

        REQUIRE(2 == v4.size());
        REQUIRE(1 == v4.at(0));
        REQUIRE(3 == v4.at(1));
    }

    SECTION("FilterMapFold") {
        const auto eleven_div_str =
            range(1, 100) | filter([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value % 11 == 0;
            })
            | map([](const auto value) {
                  static_assert(
                      is_same<decltype(value), const int>::value,
                      "value is expected to be of const int type");

                  return to_string(value);
              })
            | fold(string{}, [](const auto acc, const auto &rhs) {
                  static_assert(
                      is_same<decltype(acc), const string>::value,
                      "acc is expected to be of const string type");

                  static_assert(
                      is_same<decltype(rhs), const string &>::value,
                      "rhs is expected to be of const string & type");

                  return acc + rhs + " ";
              });

        REQUIRE("11 22 33 44 55 66 77 88 99 " == eleven_div_str);
    }

    SECTION("FilterMapFind") {
        const auto find_opt =
            range(1, 100) | filter([](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return value % 17 == 0;
            })
            | map([](const auto value) {
                  static_assert(
                      is_same<decltype(value), const int>::value,
                      "value is expected to be of const int type");

                  // .5 is a easily representable value in mantissa
                  // so that the floating float comparison can directly compare
                  // the values
                  return value + 0.5;
              })
            | find([](const auto value) {
                  static_assert(
                      is_same<decltype(value), const double>::value,
                      "value is expected to be of const double type");

                  return value == 34.5;
              });

        REQUIRE(find_opt.is_some());
        REQUIRE(34.5 == find_opt.get_unchecked());
    }

    SECTION("ZipRefMapFold") {
        const auto zipped_int_vec =
            iter(int_vec) | zip(iter(int_vec) | skip(1))
            | collect<vector<pair<const int &, const int &>>>();

        const auto folded_str =
            iter(zipped_int_vec) | map([](const auto &index_value_pair) {
                static_assert(
                    is_same<
                        decltype(index_value_pair),
                        const pair<const int &, const int &> &>::value,
                    "index_value_pair is expected to be of const pair<...> "
                    "type");

                stringstream ss;
                ss << '(' << index_value_pair.first << ','
                   << index_value_pair.second << ')';
                return ss.str();
            })
            | fold(string{}, [](const auto acc, const auto value) {
                  static_assert(
                      is_same<decltype(acc), const string>::value,
                      "acc is expected to be of const string type");

                  static_assert(
                      is_same<decltype(value), const string>::value,
                      "value is expected to be of const string type");

                  stringstream ss;
                  ss << acc << value << ' ';
                  return ss.str();
              });

        REQUIRE("(0,1) (1,2) (2,3) (3,4) (4,5) " == folded_str);
    }
}

// option

TEST_CASE("Option methods", "[Option]") {
    SECTION("CtorSome") {
        const auto opt(Some(7));
        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("CopyCtor") {
        auto opt_rhs = Some(7);
        const auto opt(opt_rhs);

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_some());

        REQUIRE(7 == opt.get_unchecked());
        REQUIRE(7 == opt_rhs.get_unchecked());
    }

    SECTION("CopyConstCtor") {
        const auto opt_rhs = Some(7);
        const auto opt(opt_rhs);

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_some());

        REQUIRE(7 == opt.get_unchecked());
        REQUIRE(7 == opt_rhs.get_unchecked());
    }

    SECTION("CopyCtorConvert") {
        const auto opt_rhs = Some("Hello");
        const Option<string> opt(opt_rhs);

        REQUIRE(opt.is_some());
        REQUIRE("Hello" == opt.get_unchecked());
    }

    SECTION("MoveCtorSimple") {
        Option<int> opt_rhs(Some(7));
        const Option<int> opt(move(opt_rhs));

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("MoveCtorComplex") {
        auto opt_rhs = Some(make_unique<int>(7));
        const Option<unique_ptr<int>> opt(move(opt_rhs));

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(7 == *opt.get_unchecked());
    }

    SECTION("MoveCtorConvert") {
        auto opt_rhs = Some("Hello");
        const Option<string> opt(move(opt_rhs));

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_none());
        REQUIRE("Hello" == opt.get_unchecked());
    }

    SECTION("CtorNone") {
        Option<int> opt(None);
        REQUIRE(opt.is_none());
    }

    SECTION("AssignSome") {
        Option<int> opt = None;
        REQUIRE(opt.is_none());

        opt = Some(7);
        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("CopyAssign") {
        Option<int> opt = None;
        const auto opt_rhs = Some(7);
        opt = opt_rhs;

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_some());
        REQUIRE(7 == opt.get_unchecked());
        REQUIRE(7 == opt_rhs.get_unchecked());
    }

    SECTION("CopyAssignConvert") {
        Option<string> opt = None;
        const auto opt_rhs = Some("Hello");
        opt = opt_rhs;

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_some());
        REQUIRE("Hello" == opt.get_unchecked());
        REQUIRE("Hello" == string(opt_rhs.get_unchecked()));
    }

    SECTION("MoveAssign") {
        Option<int> opt = None;
        auto opt_rhs = Some(7);
        opt = move(opt_rhs);

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("MoveAssignConvert") {
        Option<string> opt = None;
        auto opt_rhs = Some("Hello");
        opt = move(opt_rhs);

        REQUIRE(opt.is_some());
        REQUIRE(opt_rhs.is_none());
        REQUIRE("Hello" == opt.get_unchecked());
    }

    SECTION("AssignNone") {
        auto opt = Some(7);
        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());

        opt = None;
        REQUIRE(opt.is_none());
    }

    SECTION("GetUncheckedRef") {
        const string value = "Hello";
        auto opt = Some(cref(value));

        REQUIRE(opt.is_some());
        const auto &v = opt.get_unchecked();

        static_assert(
            is_same<decltype(v), const string &>::value,
            "v is expected to be of const string & type");

        REQUIRE("Hello" == value);
    }

    SECTION("GetMutUncheckedRef") {
        string value = "Hello";
        auto opt = Some(ref(value));

        REQUIRE(opt.is_some());
        auto &v = opt.get_mut_unchecked();

        static_assert(
            is_same<decltype(v), string &>::value,
            "v is expected to be of string & type");

        REQUIRE("Hello" == value);

        v = "World";
        REQUIRE("World" == value);
    }

    SECTION("UnwrapUnchecked") {
        auto value = make_unique<string>("Hello");
        auto opt = Some(move(value));

        REQUIRE(opt.is_some());
        auto v = move(opt).unwrap_unchecked();

        static_assert(
            is_same<decltype(v), unique_ptr<string>>::value,
            "v is expected to be of const string type");

        REQUIRE("Hello" == *v);
    }

    SECTION("UnwrapUncheckedRef") {
        string x = "Hello";
        auto opt = Some(ref(x));

        REQUIRE(opt.is_some());
        const auto &v = move(opt).unwrap_unchecked();

        static_assert(
            is_same<decltype(v), const string &>::value,
            "v is expected to be of const string & type");

        REQUIRE("Hello" == v);
    }

    SECTION("MatchXValueSome") {
        const auto value = Some(7).match(
            [](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return to_string(value);
            },
            [] { return "NONE"; });

        REQUIRE("7" == value);
    }

    SECTION("MatchMoveSome") {
        auto opt = Some(make_unique<int>(8));

        const auto value = move(opt).match(
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const unique_ptr<int> &>::value,
                    "value is expected to be of const unique_ptr<int> & type");

                return to_string(*value);
            },
            [] { return "NONE"; });

        REQUIRE(opt.is_none());
        REQUIRE("8" == value);
    }

    SECTION("MatchXValueNone") {
        const auto value = Option<int>(None).match(
            [](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return to_string(value);
            },
            [] { return "NONE"; });

        REQUIRE("NONE" == value);
    }

    SECTION("MatchRefSome") {
        const auto opt = Some(7);

        const auto value = opt.match(
            [](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return to_string(value);
            },
            [] { return "NONE"; });

        REQUIRE(opt.is_some());
        REQUIRE("7" == value);
    }

    SECTION("MatchRefNone") {
        const auto opt = Option<int>(None);

        const auto value = opt.match(
            [](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return to_string(value);
            },
            [] { return "NONE"; });

        REQUIRE(opt.is_none());
        REQUIRE("NONE" == value);
    }

    SECTION("MatchSomeXValueSomeAssignBack") {
        auto ptr = make_unique<int>(7);

        Some(move(ptr)).match_some([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            // assign back so that there is something to test
            ptr = move(value);
        });

        REQUIRE(static_cast<bool>(ptr));
        REQUIRE(7 == *ptr);
    }

    SECTION("MatchSomeXValueSomeNoEffect") {
        auto ptr = make_unique<int>(7);

        Some(move(ptr)).match_some([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            // do nothing
        });

        REQUIRE(!static_cast<bool>(ptr));
    }

    SECTION("MatchSomeXValueNone") {
        auto ptr = make_unique<int>(7);

        Option<int>(None).match_some([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), int &&>::value,
                "value is expected to be of int && type");

            // not happening
            ptr = nullptr;
        });

        REQUIRE(static_cast<bool>(ptr));
        REQUIRE(7 == *ptr);
    }

    SECTION("MatchSomeRefDoSomething") {
        const auto opt = Some(make_unique<int>(7));
        bool flag = false;

        opt.match_some([&flag](const auto &value) {
            static_assert(
                is_same<decltype(value), const unique_ptr<int> &>::value,
                "value is expected to be of const unique_ptr<int> & type");

            flag = true;
        });

        REQUIRE(flag);
        REQUIRE(opt.is_some());
        REQUIRE(7 == *opt.get_unchecked());
    }

    SECTION("MatchSomeRefNoEffect") {
        const auto opt = Option<int>(None);
        bool flag = false;

        opt.match_some([&flag](const auto &value) {
            static_assert(
                is_same<decltype(value), const int &>::value,
                "value is expected to be of const int & type");

            flag = true;
        });

        REQUIRE(!flag);
        REQUIRE(opt.is_none());
    }

    SECTION("MatchNoneXValueNoneDoSomething") {
        unique_ptr<int> ptr;

        Option<int>(None).match_none([&ptr] {
            // assign back so that there is something to test
            ptr = make_unique<int>(8);
        });

        REQUIRE(static_cast<bool>(ptr));
        REQUIRE(8 == *ptr);
    }

    SECTION("MatchNoneXValueSomeNoEffect") {
        unique_ptr<int> ptr;

        Some(777).match_none([&ptr] {
            // assign back so that there is something to test
            ptr = make_unique<int>(777);
        });

        REQUIRE(!static_cast<bool>(ptr));
    }

    SECTION("MatchNoneRefDoSomething") {
        const auto opt = Option<int>(None);
        bool flag = false;

        opt.match_none([&flag] { flag = true; });

        REQUIRE(flag);
        REQUIRE(opt.is_none());
    }

    SECTION("MatchNoneRefNoEffect") {
        const auto opt = Some(7);
        bool flag = false;

        opt.match_none([&flag] { flag = true; });

        REQUIRE(!flag);
        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("IsSomeTrue") {
        const auto opt = Some(0);
        REQUIRE(opt.is_some());
    }

    SECTION("IsSomeFalse") {
        const Option<int> opt = None;
        REQUIRE(!opt.is_some());
    }

    SECTION("IsNoneTrue") {
        const Option<int> opt = None;
        REQUIRE(opt.is_none());
    }

    SECTION("IsNoneFalse") {
        const auto opt = Some(0);
        REQUIRE(!opt.is_none());
    }

    SECTION("AsRefOnSomeValue") {
        const auto opt = Some(make_unique<int>(7));
        const auto opt_ref = opt.as_ref();

        REQUIRE(opt_ref.is_some());

        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");

        REQUIRE(7 == *opt_ref.get_unchecked());
    }

    SECTION("AsRefOnNoneValue") {
        const Option<unique_ptr<int>> opt = None;
        const auto opt_ref = opt.as_ref();

        REQUIRE(opt_ref.is_none());

        // does not evaluate at runtime, so no issues
        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");
    }

    SECTION("AsRefOnSomeRef") {
        const auto v = make_unique<int>(7);
        const auto opt = Some(cref(v));
        const auto opt_ref = opt.as_ref();

        REQUIRE(opt_ref.is_some());

        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");

        REQUIRE(7 == *opt_ref.get_unchecked());
    }

    SECTION("AsRefOnNoneRef") {
        const Option<const unique_ptr<int> &> opt = None;
        const auto opt_ref = opt.as_ref();

        REQUIRE(opt_ref.is_none());

        // does not evaluate at runtime, so no issues
        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");
    }

    SECTION("AsMutOnSomeValue") {
        auto opt = Some(make_unique<int>(7));
        const auto opt_ref = opt.as_mut();

        REQUIRE(opt_ref.is_some());

        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");

        REQUIRE(7 == *opt_ref.get_unchecked());

        *opt_ref.get_unchecked() = 8;
        REQUIRE(8 == *opt_ref.get_unchecked());
    }

    SECTION("AsMutOnNoneValue") {
        Option<unique_ptr<int>> opt = None;
        const auto opt_ref = opt.as_mut();

        REQUIRE(opt_ref.is_none());

        // does not evaluate at runtime, so no issues
        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");
    }

    SECTION("AsMutOnSomeRef") {
        auto v = make_unique<int>(7);
        auto opt = Some(ref(v));
        const auto opt_ref = opt.as_mut();

        REQUIRE(opt_ref.is_some());

        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");

        REQUIRE(7 == *opt_ref.get_unchecked());

        *opt_ref.get_unchecked() = 8;
        REQUIRE(8 == *opt_ref.get_unchecked());
    }

    SECTION("AsMutOnNoneRef") {
        Option<unique_ptr<int> &> opt = None;
        const auto opt_ref = opt.as_mut();

        REQUIRE(opt_ref.is_none());

        // does not evaluate at runtime, so no issues
        static_assert(
            is_same<
                const unique_ptr<int> &,
                decltype(opt_ref.get_unchecked())>::value,
            "opt_ref does not contain const unique_ptr<int> & type, which is "
            "unexpected");
    }

    SECTION("UnwrapOrSome") {
        auto opt = Some(0);
        const auto v = move(opt).unwrap_or(1);
        REQUIRE(0 == v);
    }

    SECTION("UnwrapOrNone") {
        Option<int> opt = None;
        const auto v = move(opt).unwrap_or(1);
        REQUIRE(1 == v);
    }

    SECTION("UnwrapOrElseSome") {
        auto opt = Some(0);
        const auto v = move(opt).unwrap_or_else([] { return 1; });
        REQUIRE(0 == v);
    }

    SECTION("UnwrapOrElseNone") {
        Option<int> opt = None;
        const auto v = move(opt).unwrap_or_else([] { return 1; });
        REQUIRE(1 == v);
    }

    SECTION("MapSome") {
        const auto opt =
            Some(make_unique<int>(777))
                .map([](auto &&value) {
                    static_assert(
                        is_same<decltype(value), unique_ptr<int> &&>::value,
                        "value is expected to be of unique_ptr<int> && type");

                    return make_unique<string>(to_string(*value));
                })
                .map([](auto &&value) {
                    static_assert(
                        is_same<decltype(value), unique_ptr<string> &&>::value,
                        "value is expected to be of unique_ptr<string> && "
                        "type");

                    return "*" + *value + "*";
                });

        REQUIRE(opt.is_some());
        REQUIRE("*777*" == opt.get_unchecked());
    }

    SECTION("MapNone") {
        auto opt = Option<int>(None);

        move(opt).map([](auto &&value) {
            static_assert(
                is_same<decltype(value), int &&>::value,
                "value is expected to be of int && type");

            return "Hello";
        });

        REQUIRE(opt.is_none());
    }

    SECTION("MapOrSome") {
        auto opt = Some(0);

        const auto v = move(opt).map_or(string("one"), [](auto &&value) {
            static_assert(
                is_same<decltype(value), int &&>::value,
                "value is expected to be of int && type");

            return string("zero");
        });

        REQUIRE("zero" == v);
    }

    SECTION("MapOrNone") {
        Option<int> opt = None;

        const auto v = move(opt).map_or(string("one"), [](auto &&value) {
            static_assert(
                is_same<decltype(value), int &&>::value,
                "value is expected to be of int && type");

            return string("zero");
        });

        REQUIRE("one" == v);
    }

    SECTION("MapOrElseSome") {
        auto opt = Some(0);

        const auto v = move(opt).map_or_else(
            [] { return string("one"); },
            [](auto &&value) {
                static_assert(
                    is_same<decltype(value), int &&>::value,
                    "value is expected to be of int && type");

                return string("zero");
            });

        REQUIRE("zero" == v);
    }

    SECTION("MapOrElseNone") {
        Option<int> opt = None;

        const auto v = move(opt).map_or_else(
            [] { return string("one"); },
            [](auto &&value) {
                static_assert(
                    is_same<decltype(value), int &&>::value,
                    "value is expected to be of int && type");

                return string("zero");
            });

        REQUIRE("one" == v);
    }

    SECTION("OkOrSome") {
        auto opt = Some(1);
        const auto res = move(opt).ok_or(string("Error"));

        REQUIRE(opt.is_none());
        REQUIRE(res.is_ok());
        REQUIRE(1 == res.get_unchecked());
    }

    SECTION("OkOrNone") {
        Option<int> opt = None;
        const auto res = move(opt).ok_or(string("Error"));

        REQUIRE(opt.is_none());
        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
    }

    SECTION("OkOrElseSome") {
        auto opt = Some(1);
        const auto res = move(opt).ok_or_else([] { return string("Error"); });

        REQUIRE(opt.is_none());
        REQUIRE(res.is_ok());
        REQUIRE(1 == res.get_unchecked());
    }

    SECTION("OkOrElseNone") {
        Option<int> opt = None;
        const auto res = move(opt).ok_or_else([] { return string("Error"); });

        REQUIRE(opt.is_none());
        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
    }

    SECTION("OkOrElseRefOk") {
        const string ok_value = "Okay";
        const string err_value = "Error";

        const auto res = Some(cref(ok_value)).ok_or_else([&err_value] {
            return cref(err_value);
        });

        REQUIRE(res.is_ok());
        REQUIRE("Okay" == res.get_unchecked());
        REQUIRE(&ok_value == &res.get_unchecked());
    }

    SECTION("OkOrElseRefErr") {
        const string err_value = "Error";

        const auto res = Option<string>(None).ok_or_else(
            [&err_value] { return cref(err_value); });

        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
        REQUIRE(&err_value == &res.get_err_unchecked());
    }

    SECTION("AndLhsSomeRhsSome") {
        auto opt_lhs = Some(1);
        auto opt_rhs = Some("one");

        const auto opt = move(opt_lhs).and_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());

        REQUIRE(opt.is_some());
        REQUIRE("one" == string(opt.get_unchecked()));
    }

    SECTION("AndLhsNoneRhsSome") {
        Option<int> opt_lhs = None;
        auto opt_rhs = Some("one");

        const auto opt = move(opt_lhs).and_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(opt.is_none());
    }

    SECTION("AndLhsSomeRhsNone") {
        auto opt_lhs = Some(1);
        Option<const char *> opt_rhs = None;

        const auto opt = move(opt_lhs).and_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(opt.is_none());
    }

    SECTION("AndLhsNoneRhsNone") {
        Option<int> opt_lhs = None;
        Option<const char *> opt_rhs = None;

        const auto opt = move(opt_lhs).and_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());
        REQUIRE(opt.is_none());
    }

    SECTION("AndThenSomeToSome") {
        const auto opt = Some(make_unique<int>(777)).and_then([](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            return Some(make_unique<string>(to_string(*value)));
        });

        REQUIRE(opt.is_some());
        REQUIRE("777" == *opt.get_unchecked());
    }

    SECTION("AndThenSomeToNone") {
        const auto opt =
            Some(make_unique<int>(777))
                .and_then([](auto &&value) -> Option<string> {
                    static_assert(
                        is_same<decltype(value), unique_ptr<int> &&>::value,
                        "value is expected to be of unique_ptr<int> && type");

                    return None;
                });

        REQUIRE(opt.is_none());
    }

    SECTION("AndThenNone") {
        const auto opt =
            Option<unique_ptr<int>>(None).and_then([](auto &&value) {
                static_assert(
                    is_same<decltype(value), unique_ptr<int> &&>::value,
                    "value is expected to be of unique_ptr<int> && type");

                return Some(make_unique<string>(to_string(*value)));
            });

        REQUIRE(opt.is_none());
    }

    SECTION("OrLhsSomeRhsSome") {
        auto opt_lhs = Some(1);
        auto opt_rhs = Some(7);

        const auto opt = move(opt_lhs).or_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());

        REQUIRE(opt.is_some());
        REQUIRE(1 == opt.get_unchecked());
    }

    SECTION("OrLhsNoneRhsSome") {
        Option<int> opt_lhs = None;
        auto opt_rhs = Some(7);

        const auto opt = move(opt_lhs).or_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());

        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("OrLhsSomeRhsNone") {
        auto opt_lhs = Some(1);
        Option<int> opt_rhs = None;

        const auto opt = move(opt_lhs).or_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());

        REQUIRE(opt.is_some());
        REQUIRE(1 == opt.get_unchecked());
    }

    SECTION("OrLhsNoneRhsNone") {
        Option<int> opt_lhs = None;
        Option<int> opt_rhs = None;

        const auto opt = move(opt_lhs).or_(move(opt_rhs));
        REQUIRE(opt_lhs.is_none());
        REQUIRE(opt_rhs.is_none());

        REQUIRE(opt.is_none());
    }

    SECTION("OrElseSome") {
        const auto opt = Some(7).or_else([] { return None; });

        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("OrElseNoneToNone") {
        const auto opt =
            Option<unique_ptr<int>>(None).or_else([] { return None; });

        REQUIRE(opt.is_none());
    }

    SECTION("OrElseNoneToSome") {
        const auto opt = Option<unique_ptr<int>>(None).or_else(
            [] { return Some(make_unique<int>(7)); });

        REQUIRE(opt.is_some());
        REQUIRE(7 == *opt.get_unchecked());
    }

    SECTION("GetOrInsertSome") {
        auto opt = Some(1);
        auto &v = opt.get_or_insert(7);

        REQUIRE(1 == v);
        v = 8;

        REQUIRE(opt.is_some());
        REQUIRE(8 == opt.get_unchecked());
    }

    SECTION("GetOrInsertNone") {
        Option<int> opt = None;
        auto &v = opt.get_or_insert(7);

        REQUIRE(7 == v);
        v = 8;

        REQUIRE(opt.is_some());
        REQUIRE(8 == opt.get_unchecked());
    }

    SECTION("GetOrInsertWithSome") {
        auto opt = Some(1);
        auto &v = opt.get_or_insert_with([] { return 7; });

        REQUIRE(1 == v);
        v = 8;

        REQUIRE(opt.is_some());
        REQUIRE(8 == opt.get_unchecked());
    }

    SECTION("GetOrInsertWithNone") {
        Option<int> opt = None;
        auto &v = opt.get_or_insert_with([] { return 7; });

        REQUIRE(7 == v);
        v = 8;

        REQUIRE(opt.is_some());
        REQUIRE(8 == opt.get_unchecked());
    }

    SECTION("TakeSome") {
        auto opt = Some(make_unique<int>(7));
        const auto optb = opt.take();

        REQUIRE(opt.is_none());
        REQUIRE(optb.is_some());

        REQUIRE(7 == *optb.get_unchecked());
    }

    SECTION("TakeNone") {
        Option<int> opt = None;
        const auto optb = opt.take();

        REQUIRE(opt.is_none());
        REQUIRE(optb.is_none());
    }

    SECTION("ClonedRefSome") {
        const string value = "Hello";
        auto opt = Some(cref(value));
        const auto optb = move(opt).cloned();

        REQUIRE(opt.is_none());
        REQUIRE(optb.is_some());
        REQUIRE("Hello" == optb.get_unchecked());
        REQUIRE(&value != &optb.get_unchecked());
    }

    SECTION("ClonedRefNone") {
        Option<const string &> opt = None;
        const auto optb = move(opt).cloned();

        REQUIRE(opt.is_none());
        REQUIRE(optb.is_none());
    }

    SECTION("UnwrapOrDefaultSome") {
        auto opt = Some(string("Hello"));
        const auto v = move(opt).unwrap_or_default();

        REQUIRE(opt.is_none());
        REQUIRE("Hello" == v);
    }

    SECTION("UnwrapOrDefaultNone") {
        Option<string> opt = None;
        const auto v = move(opt).unwrap_or_default();

        REQUIRE(opt.is_none());
        REQUIRE(v.empty());
    }

    SECTION("OptIfTrue") {
        const auto opt = opt_if(true, [] { return 7; });

        REQUIRE(opt.is_some());
        REQUIRE(7 == opt.get_unchecked());
    }

    SECTION("OptIfFalse") {
        const auto opt = opt_if(false, [] { return 0; });

        REQUIRE(opt.is_none());
    }
}

// result

TEST_CASE("Result methods", "[Result]") {
    SECTION("CtorOk") {
        const Result<unique_ptr<int>, unique_ptr<string>> res(
            Ok(make_unique<int>(7)));

        REQUIRE(res.is_ok());
        REQUIRE(7 == *res.get_unchecked());
    }

    SECTION("CtorOkConvert") {
        // converts from const char * to string
        const Result<string, string> res(Ok("Hello"));
        REQUIRE(res.is_ok());
        REQUIRE("Hello" == res.get_unchecked());
    }

    SECTION("CtorErr") {
        const Result<unique_ptr<int>, unique_ptr<string>> res(
            Err(make_unique<string>("Error")));

        REQUIRE(res.is_err());
        REQUIRE("Error" == *res.get_err_unchecked());
    }

    SECTION("CtorErrConvert") {
        // converts from const char * to string
        const Result<string, string> res(Err("Error"));
        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
    }

    SECTION("AssignmentOkToOkConvert") {
        Result<string, double> res = Ok(string("Hello"));

        // converts from const char * to string
        res = Ok("World");

        REQUIRE(res.is_ok());
        REQUIRE("World" == res.get_unchecked());
    }

    SECTION("AssignmentOkToErr") {
        Result<unique_ptr<string>, string> res =
            Ok(make_unique<string>("Hello"));
        res = Err(string("World"));

        REQUIRE(res.is_err());
        REQUIRE("World" == res.get_err_unchecked());
    }

    SECTION("AssignmentErrToErrConvert") {
        Result<double, string> res = Err(string("Hello"));

        // converts from const char * to string
        res = Err("World");

        REQUIRE(res.is_err());
        REQUIRE("World" == res.get_err_unchecked());
    }

    SECTION("AssignmentErrToOk") {
        Result<unique_ptr<string>, string> res = Err(string("Hello"));
        res = Ok(make_unique<string>("World"));

        REQUIRE(res.is_ok());
        REQUIRE("World" == *res.get_unchecked());
    }

    SECTION("GetUnchecked") {
        const Result<int, string> res = Ok(7);

        REQUIRE(res.is_ok());
        REQUIRE(7 == res.get_unchecked());
    }

    SECTION("GetMutUnchecked") {
        Result<int, string> res = Ok(7);

        REQUIRE(res.is_ok());
        REQUIRE(7 == res.get_mut_unchecked());

        res.get_mut_unchecked() = 8;
        REQUIRE(res.is_ok());
        REQUIRE(8 == res.get_mut_unchecked());
    }

    SECTION("GetErrUnchecked") {
        const Result<int, string> res = Err(string{"Hello"});

        REQUIRE(res.is_err());
        REQUIRE("Hello" == res.get_err_unchecked());
    }

    SECTION("GetErrMutUnchecked") {
        Result<int, string> res = Err(string{"Hello"});

        REQUIRE(res.is_err());
        REQUIRE("Hello" == res.get_err_mut_unchecked());

        res.get_err_mut_unchecked() = "World";
        REQUIRE(res.is_err());
        REQUIRE("World" == res.get_err_mut_unchecked());
    }

    SECTION("UnwrapUnchecked") {
        int x = 7;
        Result<int &, int> res = Ok(ref(x));

        REQUIRE(res.is_ok());

        // to show that it returns reference
        static_assert(
            is_same<decltype(move(res).unwrap_unchecked()), int &>::value,
            "Result::UnwrapUnchecked does not return reference directly!");

        REQUIRE(7 == move(res).unwrap_unchecked());
    }

    SECTION("UnwrapErrUnchecked") {
        Result<string, unique_ptr<string>> res =
            Err(make_unique<string>("Hello"));

        REQUIRE(res.is_err());

        // to prove that the pointer can move
        auto err_ptr = move(res).unwrap_err_unchecked();
        REQUIRE("Hello" == *err_ptr);
    }

    SECTION("MatchOk") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(2));

        const auto match_res = move(res).match(
            [](auto &&value) {
                // static_assert(is_same<decltype(value),
                // unique_ptr<int>>::value,
                //    "value is expected to be of unique_ptr<int> type");

                return true;
            },
            [](auto &&value) {
                // static_assert(is_same<decltype(value), unique_ptr<string>
                // &&>::value,
                //     "value is expected to be of unique_ptr<string> && type");

                return false;
            });

        REQUIRE(match_res);
    }

    SECTION("MatchErr") {
        Result<int, string> res = Err(string{"Hello"});

        const auto match_res = move(res).match(
            [](const auto value) {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return 0.5;
            },
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return 0.25;
            });

        REQUIRE(0.25 == match_res);
    }

    SECTION("MatchOkRef") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(777));

        const auto match_res = res.match(
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const unique_ptr<int> &>::value,
                    "value is expected to be of const unique_ptr<int> & type");

                return to_string(*value);
            },
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const unique_ptr<string> &>::value,
                    "value is expected to be of const unique_ptr<string> & "
                    "type");

                return *value;
            });

        REQUIRE("777" == match_res);
    }

    SECTION("MatchErrRef") {
        Result<string, string> res = Err(string{"World"});

        const auto match_res = res.match(
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return cref(value);
            },
            [](const auto &value) {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return cref(value);
            });

        REQUIRE(res.is_err());
        REQUIRE(&res.get_err_unchecked() == &match_res.get());
        REQUIRE("World" == match_res.get());
    }

    SECTION("MatchOkOk") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(2));
        unique_ptr<int> ptr;

        move(res).match_ok([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            ptr = move(value);
        });

        REQUIRE(static_cast<bool>(ptr));
        REQUIRE(2 == *ptr);
    }

    SECTION("MatchOkErr") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Err(make_unique<string>("Hello"));

        unique_ptr<int> ptr;

        move(res).match_ok([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            ptr = move(value);
        });

        REQUIRE(!static_cast<bool>(ptr));
    }

    SECTION("MatchOkRefOk") {
        const Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(2));
        int value = 0;

        res.match_ok([&value](const auto &res_value) {
            static_assert(
                is_same<decltype(res_value), const unique_ptr<int> &>::value,
                "res_value is expected to be of const unique_ptr<int> & type");

            value = *res_value;
        });

        REQUIRE(2 == value);
        REQUIRE(res.is_ok());
        REQUIRE(2 == *res.get_unchecked());
    }

    SECTION("MatchOkRefErr") {
        const Result<unique_ptr<int>, unique_ptr<string>> res =
            Err(make_unique<string>("Hello"));
        int value = 7;

        res.match_ok([&value](const auto &res_value) {
            static_assert(
                is_same<decltype(res_value), const unique_ptr<int> &>::value,
                "res_value is expected to be of const unique_ptr<int> & type");

            value = *res_value;
        });

        REQUIRE(7 == value);
        REQUIRE(res.is_err());
        REQUIRE("Hello" == *res.get_err_unchecked());
    }

    SECTION("MatchErrOk") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(2));

        unique_ptr<string> ptr;

        move(res).match_err([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<string> &&>::value,
                "value is expected to be of unique_ptr<string> && type");

            ptr = move(value);
        });

        REQUIRE(!static_cast<bool>(ptr));
    }

    SECTION("MatchErrErr") {
        Result<unique_ptr<int>, unique_ptr<string>> res =
            Err(make_unique<string>("Hello"));
        unique_ptr<string> ptr;

        move(res).match_err([&ptr](auto &&value) {
            static_assert(
                is_same<decltype(value), unique_ptr<string> &&>::value,
                "value is expected to be of unique_ptr<string> && type");

            ptr = move(value);
        });

        REQUIRE(static_cast<bool>(ptr));
        REQUIRE("Hello" == *ptr);
    }

    SECTION("MatchErrRefOk") {
        const Result<unique_ptr<int>, unique_ptr<string>> res =
            Ok(make_unique<int>(2));
        string value = "World";

        res.match_err([&value](const auto &res_value) {
            static_assert(
                is_same<decltype(res_value), const unique_ptr<string> &>::value,
                "res_value is expected to be of const unique_ptr<string> & "
                "type");

            value = *res_value;
        });

        REQUIRE("World" == value);
        REQUIRE(res.is_ok());
        REQUIRE(2 == *res.get_unchecked());
    }

    SECTION("MatchErrRefErr") {
        const Result<unique_ptr<int>, unique_ptr<string>> res =
            Err(make_unique<string>("Hello"));
        string value = "World";

        res.match_err([&value](auto &&res_value) {
            static_assert(
                is_same<decltype(res_value), const unique_ptr<string> &>::value,
                "res_value is expected to be of const unique_ptr<string> & "
                "type");

            value = *res_value;
        });

        REQUIRE("Hello" == value);
        REQUIRE(res.is_err());
        REQUIRE("Hello" == *res.get_err_unchecked());
    }

    SECTION("MapOk") {
        Result<int, string> res = Ok(1);
        const auto mapped_res = move(res).map([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value + 0.5;
        });

        REQUIRE(mapped_res.is_ok());
        REQUIRE(1.5 == mapped_res.get_unchecked());
    }

    SECTION("MapErr") {
        Result<int, string> res = Err(string{"Error"});
        const auto mapped_res = move(res).map([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        });

        REQUIRE(mapped_res.is_err());
        REQUIRE("Error" == mapped_res.get_err_unchecked());
    }

    SECTION("MapErrErr") {
        Result<int, string> res = Err(string{"Error"});
        const auto mapped_res = move(res).map_err([](const auto &value) {
            static_assert(
                is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return value + " value";
        });

        REQUIRE(mapped_res.is_err());
        REQUIRE("Error value" == mapped_res.get_err_unchecked());
    }

    SECTION("MapErrOk") {
        Result<int, int> res = Ok(-1);
        const auto mapped_res = move(res).map_err([](const auto value) {
            static_assert(
                is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        });

        REQUIRE(mapped_res.is_ok());
        REQUIRE(-1 == mapped_res.get_unchecked());
    }

    SECTION("AndOkWithOk") {
        Result<int, string> res_left = Ok(1);
        Result<double, string> res_right = Ok(3.14);

        const auto res = move(res_left).and_(move(res_right));

        REQUIRE(res.is_ok());
        REQUIRE(3.14 == res.get_unchecked());
    }

    SECTION("AndErrWithOk") {
        Result<int, string> res_left = Err(string("Error"));
        Result<double, string> res_right = Ok(3.14);

        const auto res = move(res_left).and_(move(res_right));

        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
    }

    SECTION("AndOkWithErr") {
        Result<int, string> res_left = Ok(1);
        Result<double, string> res_right = Err(string("Error"));

        const auto res = move(res_left).and_(move(res_right));

        REQUIRE(res.is_err());
        REQUIRE("Error" == res.get_err_unchecked());
    }

    SECTION("AndErrWithErr") {
        Result<int, string> res_left = Err(string("Left Error"));
        Result<double, string> res_right = Err(string("Right Error"));

        const auto res = move(res_left).and_(move(res_right));

        REQUIRE(res.is_err());
        REQUIRE("Left Error" == res.get_err_unchecked());
    }

    SECTION("AndThenOkWithOk") {
        Result<int, string> res = Ok(1);

        const auto mapped_res =
            move(res).and_then([](const auto value) -> Result<string, string> {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return Ok(to_string(value));
            });

        REQUIRE(mapped_res.is_ok());
        REQUIRE("1" == mapped_res.get_unchecked());
    }

    SECTION("AndThenOkWithErr") {
        Result<int, string> res = Ok(2);

        const auto mapped_res =
            move(res).and_then([](const auto value) -> Result<int, string> {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return Err(to_string(value));
            });

        REQUIRE(mapped_res.is_err());
        REQUIRE("2" == mapped_res.get_err_unchecked());
    }

    SECTION("AndThenErr") {
        Result<int, string> res = Err(string{"Error"});

        const auto mapped_res =
            move(res).and_then([](const auto value) -> Result<int, string> {
                static_assert(
                    is_same<decltype(value), const int>::value,
                    "value is expected to be of const int type");

                return Ok(value);
            });

        REQUIRE(mapped_res.is_err());
        REQUIRE("Error" == mapped_res.get_err_unchecked());
    }

    SECTION("OrOkWithOk") {
        Result<int, string> res_left = Ok(1);
        Result<int, double> res_right = Ok(2);

        const auto res = move(res_left).or_(move(res_right));

        REQUIRE(res.is_ok());
        REQUIRE(1 == res.get_unchecked());
    }

    SECTION("OrErrWithOk") {
        Result<int, string> res_left = Err(string("Error"));
        Result<int, double> res_right = Ok(2);

        const auto res = move(res_left).or_(move(res_right));

        REQUIRE(res.is_ok());
        REQUIRE(2 == res.get_unchecked());
    }

    SECTION("OrOkWithErr") {
        Result<int, string> res_left = Ok(1);
        Result<int, double> res_right = Err(3.14);

        const auto res = move(res_left).or_(move(res_right));

        REQUIRE(res.is_ok());
        REQUIRE(1 == res.get_unchecked());
    }

    SECTION("OrErrWithErr") {
        Result<int, string> res_left = Err(string("Error"));
        Result<int, double> res_right = Err(3.14);

        const auto res = move(res_left).or_(move(res_right));

        REQUIRE(res.is_err());
        REQUIRE(3.14 == res.get_err_unchecked());
    }

    SECTION("OrElseErrWithOk") {
        Result<int, string> res = Err(string{"Error"});

        const auto mapped_res =
            move(res).or_else([](const auto &value) -> Result<int, int> {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return Ok(7);
            });

        REQUIRE(mapped_res.is_ok());
        REQUIRE(7 == mapped_res.get_unchecked());
    }

    SECTION("OrElseErrWithErr") {
        Result<int, string> res = Err(string{"Error"});

        const auto mapped_res =
            move(res).or_else([](const auto &value) -> Result<int, string> {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return Err(string{"Still error"});
            });

        REQUIRE(mapped_res.is_err());
        REQUIRE("Still error" == mapped_res.get_err_unchecked());
    }

    SECTION("OrElseOk") {
        Result<int, string> res = Ok(5);

        const auto mapped_res =
            move(res).or_else([](const auto &value) -> Result<int, double> {
                static_assert(
                    is_same<decltype(value), const string &>::value,
                    "value is expected to be of const string & type");

                return Ok(6);
            });

        REQUIRE(mapped_res.is_ok());
        REQUIRE(5 == mapped_res.get_unchecked());
    }

    SECTION("UnwrapOrOk") {
        Result<int, string> res = Ok(1);
        const auto v = move(res).unwrap_or(7);
        REQUIRE(1 == v);
    }

    SECTION("UnwrapOrErr") {
        Result<int, string> res = Err(string("Error"));
        const auto v = move(res).unwrap_or(7);
        REQUIRE(7 == v);
    }

    SECTION("UnwrapOrElseOk") {
        Result<int, string> res = Ok(1);
        const auto v = move(res).unwrap_or_else([] { return 7; });
        REQUIRE(1 == v);
    }

    SECTION("UnwrapOrElseErr") {
        Result<int, string> res = Err(string("Error"));
        const auto v = move(res).unwrap_or_else([] { return 7; });
        REQUIRE(7 == v);
    }

    SECTION("UnwrapOrDefaultOk") {
        Result<string, double> res = Ok(string("Hello"));
        const auto v = move(res).unwrap_or_default();
        REQUIRE("Hello" == v);
    }

    SECTION("UnwrapOrDefaultErr") {
        Result<string, double> res = Err(3.14);
        const auto v = move(res).unwrap_or_default();
        REQUIRE(v.empty());
    }

    SECTION("ResIfElseTrue") {
        const auto res =
            res_if_else(true, [] { return 1; }, [] { return 3.14; });

        REQUIRE(res.is_ok());
    }

    SECTION("ResIfElseFalse") {
        const auto res =
            res_if_else(false, [] { return 1; }, [] { return 3.14; });

        REQUIRE(res.is_err());
    }

    SECTION("OkSome") {
        Result<int, string> res = Ok(8);
        const auto opt = move(res).ok();

        REQUIRE(opt.is_some());
        REQUIRE(8 == opt.get_unchecked());
    }

    SECTION("OkNone") {
        Result<int, string> res = Err(string{"World"});
        const auto opt = move(res).ok();

        REQUIRE(opt.is_none());
    }

    SECTION("ErrSome") {
        Result<int, string> res = Err(string{"World"});
        const auto opt = move(res).err();

        REQUIRE(opt.is_some());
        REQUIRE("World" == opt.get_unchecked());
    }

    SECTION("ErrNone") {
        Result<int, string> res = Ok(9);
        const auto opt = move(res).err();

        REQUIRE(opt.is_none());
    }
}

TEST_CASE("Macro expansion", "[Macro]") {
    SECTION("Let") {
        const auto fn = [](const bool flag,
                           size_t &count) -> Result<int, string> {
            using inter_result_t = Result<double, string>;

            auto create_res = [](const bool flag, size_t &count) {
                // to prove that the function is only called once
                // despite expression being placed into the macro
                ++count;
                return flag ? inter_result_t(Ok(3.14)) : Err(string("Hello"));
            };

            let(value, create_res(flag, count));
            return Ok(static_cast<int>(value));
        };

        {
            size_t count = 0;
            const auto res = fn(true, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_ok());
            REQUIRE(3 == res.get_unchecked());
        }

        {
            size_t count = 0;
            const auto res = fn(false, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_err());
            REQUIRE("Hello" == res.get_err_unchecked());
        }
    }

    SECTION("LetMut") {
        const auto fn = [](const bool flag,
                           size_t &count) -> Result<int, string> {
            using inter_result_t = Result<double, string>;

            auto create_res = [](const bool flag, size_t &count) {
                // to prove that the function is only called once
                // despite expression being placed into the macro
                ++count;
                return flag ? inter_result_t(Ok(3.14)) : Err(string("Hello"));
            };

            let_mut(value, create_res(flag, count));
            value += 10;
            return Ok(static_cast<int>(value));
        };

        {
            size_t count = 0;
            const auto res = fn(true, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_ok());
            REQUIRE(13 == res.get_unchecked());
        }

        {
            size_t count = 0;
            const auto res = fn(false, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_err());
            REQUIRE("Hello" == res.get_err_unchecked());
        }
    }

    SECTION("LetRef") {
        const int val = 7;

        const auto fn = [&val](
                            const bool flag,
                            size_t &count) -> Result<const int &, string> {

            using inter_result_t = Result<const int &, string>;

            auto create_res = [&val](const bool flag, size_t &count) {
                // to prove that the function is only called once
                // despite expression being placed into the macro
                ++count;
                return flag ? inter_result_t(Ok(cref(val)))
                            : Err(string("Hello"));
            };

            let_ref(value, create_res(flag, count));
            return Ok(cref(value));
        };

        {
            size_t count = 0;
            const auto res = fn(true, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_ok());
            REQUIRE(val == res.get_unchecked());
            REQUIRE(&val == &res.get_unchecked());
        }

        {
            size_t count = 0;
            const auto res = fn(false, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_err());
            REQUIRE("Hello" == res.get_err_unchecked());
        }
    }

    SECTION("LetMutRef") {
        int val = 7;

        const auto fn = [&val](
                            const bool flag,
                            size_t &count) -> Result<const int &, string> {

            using inter_result_t = Result<int &, string>;

            auto create_res = [&val](const bool flag, size_t &count) {
                // to prove that the function is only called once
                // despite expression being placed into the macro
                ++count;
                return flag ? inter_result_t(Ok(ref(val)))
                            : Err(string("Hello"));
            };

            let_mut_ref(value, create_res(flag, count));
            value += 10;
            return Ok(cref(value));
        };

        {
            size_t count = 0;
            const auto res = fn(true, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_ok());
            REQUIRE(17 == res.get_unchecked());
            REQUIRE(&val == &res.get_unchecked());
        }

        {
            size_t count = 0;
            const auto res = fn(false, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_err());
            REQUIRE("Hello" == res.get_err_unchecked());
        }
    }

    SECTION("RetIfErr") {
        const auto fn = [](const bool flag,
                           size_t &count) -> Result<int, string> {

            using inter_result_t = Result<double, string>;

            auto create_res = [](const bool flag, size_t &count) {
                // to prove that the function is only called once
                // despite expression being placed into the macro
                ++count;
                return flag ? inter_result_t(Ok(3.14)) : Err(string("Hello"));
            };

            ret_if_err(create_res(flag, count));
            return Ok(0);
        };

        {
            size_t count = 0;
            const auto res = fn(true, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_ok());
            REQUIRE(0 == res.get_unchecked());
        }

        {
            size_t count = 0;
            const auto res = fn(false, count);

            REQUIRE(1 == count);
            REQUIRE(res.is_err());
            REQUIRE("Hello" == res.get_err_unchecked());
        }
    }
}
