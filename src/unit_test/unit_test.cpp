#define RUSTFP_SIMPLIFIED_LET

#include "rustfp/all.h"
#include "rustfp/any.h"
#include "rustfp/cloned.h"
#include "rustfp/cycle.h"
#include "rustfp/collect.h"
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
#include "rustfp/zip.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <deque>
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

using rustfp::opt_if;
using rustfp::Option;
using rustfp::None;
using rustfp::Some;

using rustfp::res_if_else;
using rustfp::Result;
using rustfp::Ok;
using rustfp::Err;

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
        const auto input_it_pairs = mismatch(
            cbegin(lhs), cend(lhs), cbegin(rhs));

        return input_it_pairs.first == lhs.cend() &&
            input_it_pairs.second == rhs.cend();
    }

    template <class IterableLhs, class IterableRhs, class CompFn>
    auto no_mismatch_values(IterableLhs &&lhs, IterableRhs &&rhs, CompFn &&comp_fn) -> bool {
        const auto input_it_pairs = mismatch(
            cbegin(lhs), cend(lhs), cbegin(rhs), forward<CompFn>(comp_fn));

        return input_it_pairs.first == lhs.cend() &&
            input_it_pairs.second == rhs.cend();
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
    auto similar_values(IterableLhs &&lhs, IterableRhs &&rhs, CompFn &&comp_fn) -> bool {
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
}

// simple tests

class Ops : public ::testing::Test {
protected:
    void SetUp() override {
        int_vec = vector<int>{0, 1, 2, 3, 4, 5};
        str_vec = vector<string>{"Hello", "World", "How", "Are", "You", "?"};
    }

    vector<int> int_vec;
    vector<string> str_vec;
};

class ComplexOps : public ::testing::Test {
protected:
    void SetUp() override {
        int_vec = vector<int>{0, 1, 2, 3, 4, 5};
    }

    vector<int> int_vec;
};

TEST_F(Ops, Iter) {
    auto it = iter(int_vec);

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        const int &>::value,
        "Ops::Iter failed Iter type checking!");

    const auto opt0 = it.next();
    ASSERT_TRUE(opt0.is_some());
    ASSERT_EQ(0, opt0.get_unchecked());

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        const int &>::value,
        "Ops::Iter failed Option type checking!");
    
    const auto opt1 = it.next();
    ASSERT_TRUE(opt1.is_some());
    ASSERT_EQ(1, opt1.get_unchecked());

    const auto opt2 = it.next();
    ASSERT_TRUE(opt2.is_some());
    ASSERT_EQ(2, opt2.get_unchecked());

    const auto opt3 = it.next();
    ASSERT_TRUE(opt3.is_some());
    ASSERT_EQ(3, opt3.get_unchecked());

    const auto opt4 = it.next();
    ASSERT_TRUE(opt4.is_some());
    ASSERT_EQ(4, opt4.get_unchecked());

    const auto opt5 = it.next();
    ASSERT_TRUE(opt5.is_some());
    ASSERT_EQ(5, opt5.get_unchecked());

    const auto opt6 = it.next();
    ASSERT_TRUE(opt6.is_none());
}

TEST_F(Ops, IterMut) {
    auto v = int_vec;

    // modify the values
    iter_mut(v)
        | map([](auto &val) {
            static_assert(is_same<decltype(val), int &>::value,
                "val is expected to be of int & type");

            val *= val;
            return ref(val);
        })
        | for_each([](auto &val) {
            static_assert(is_same<decltype(val), int &>::value,
                "val is expected to be of int & type");

            val += 1;
        });

    auto it = iter_mut(v);

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        int &>::value,
        "Ops::IterMut failed Iter type checking!");

    const auto opt0 = it.next();
    ASSERT_TRUE(opt0.is_some());
    ASSERT_EQ(1, opt0.get_unchecked());

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        int &>::value,
        "Ops::IterMut failed Option type checking!");
    
    const auto opt1 = it.next();
    ASSERT_TRUE(opt1.is_some());
    ASSERT_EQ(2, opt1.get_unchecked());

    const auto opt2 = it.next();
    ASSERT_TRUE(opt2.is_some());
    ASSERT_EQ(5, opt2.get_unchecked());

    const auto opt3 = it.next();
    ASSERT_TRUE(opt3.is_some());
    ASSERT_EQ(10, opt3.get_unchecked());

    const auto opt4 = it.next();
    ASSERT_TRUE(opt4.is_some());
    ASSERT_EQ(17, opt4.get_unchecked());

    const auto opt5 = it.next();
    ASSERT_TRUE(opt5.is_some());
    ASSERT_EQ(26, opt5.get_unchecked());

    const auto opt6 = it.next();
    ASSERT_TRUE(opt6.is_none());
}

TEST_F(Ops, IterMutChain) {
    auto v = int_vec;

    // modify the values
    const auto v2 = iter_mut(v)
        | filter([](const auto val) {
            static_assert(is_same<decltype(val), const int>::value,
                "val is expected to be of const int type");

            return val >= 2;
        })
        | map([](auto &val) {
            static_assert(is_same<decltype(val), int &>::value,
                "val is expected to be of int & type");

            val -= 2;
            return ref(val);
        })
        | collect<vector<reference_wrapper<int>>>();

    ASSERT_EQ(4, v2.size());
    ASSERT_EQ(0, v2.at(0));
    ASSERT_EQ(1, v2.at(1));
    ASSERT_EQ(2, v2.at(2));
    ASSERT_EQ(3, v2.at(3));
}

TEST_F(Ops, IntoIter) {
    vector<unique_ptr<int>> v;
    v.push_back(make_unique<int>(0));
    v.push_back(make_unique<int>(1));
    v.push_back(make_unique<int>(2));

    auto it = into_iter(move(v));

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        unique_ptr<int>>::value,
        "Ops::IntoIter failed IntoIter type checking!");

    auto opt0 = it.next();

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        unique_ptr<int>>::value,
        "Ops::IntoIter failed Option type checking!");

    ASSERT_TRUE(opt0.is_some());
    auto val0(move(opt0).unwrap_unchecked());
    ASSERT_EQ(0, *val0);

    auto opt1 = it.next();
    ASSERT_TRUE(opt1.is_some());
    auto val1(move(opt1).unwrap_unchecked());
    ASSERT_EQ(1, *val1);

    auto opt2 = it.next();
    ASSERT_TRUE(opt2.is_some());
    auto val2(move(opt2).unwrap_unchecked());
    ASSERT_EQ(2, *val2);

    ASSERT_TRUE(it.next().is_none());
}

TEST_F(Ops, IterBeginEndVec) {
    vector<unique_ptr<int>> v;
    v.push_back(make_unique<int>(0));
    v.push_back(make_unique<int>(1));
    v.push_back(make_unique<int>(2));

    auto it = iter_begin_end(v.begin(), v.cend());

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        const unique_ptr<int> &>::value,
        "Ops::IterBeginEndVec failed IterBeginEnd type checking!");

    auto opt0 = it.next();

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        const unique_ptr<int> &>::value,
        "Ops::IterBeginEndVec failed Option type checking!");

    ASSERT_TRUE(opt0.is_some());
    const auto &val0 = opt0.get_unchecked();
    ASSERT_EQ(0, *val0);

    auto opt1 = it.next();
    ASSERT_TRUE(opt1.is_some());
    const auto &val1 = opt1.get_unchecked();
    ASSERT_EQ(1, *val1);

    auto opt2 = it.next();
    ASSERT_TRUE(opt2.is_some());
    const auto &val2 = opt2.get_unchecked();
    ASSERT_EQ(2, *val2);

    ASSERT_TRUE(it.next().is_none());
}

TEST_F(Ops, IterBeginEndPtrs) {
    // note the length is 4 since there is null character
    const char VALUES[] = {"bad"};
    auto it = iter_begin_end(VALUES + 0, VALUES + extent<decltype(VALUES)>::value);

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        const char &>::value,
        "Ops::IterBeginEndPtrs failed IterBeginEnd type checking!");

    auto opt0 = it.next();

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        const char &>::value,
        "Ops::IterBeginEndPtrs failed Option type checking!");

    ASSERT_TRUE(opt0.is_some());
    const auto &val0 = opt0.get_unchecked();
    ASSERT_EQ('b', val0);

    auto opt1 = it.next();
    ASSERT_TRUE(opt1.is_some());
    const auto &val1 = opt1.get_unchecked();
    ASSERT_EQ('a', val1);

    auto opt2 = it.next();
    ASSERT_TRUE(opt2.is_some());
    const auto &val2 = opt2.get_unchecked();
    ASSERT_EQ('d', val2);

    auto opt3 = it.next();
    ASSERT_TRUE(opt3.is_some());
    const auto &val3 = opt3.get_unchecked();
    ASSERT_EQ('\0', val3);

    ASSERT_TRUE(it.next().is_none());
}

TEST_F(Ops, AllTrue) {
    const auto result = iter(int_vec)
        | all([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value < 6;
        });

    ASSERT_TRUE(result);
}

TEST_F(Ops, AllFalse) {
    const auto result = iter(int_vec)
        | all([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value > 0;
        });

    ASSERT_FALSE(result);
}

TEST_F(Ops, AnyTrue) {
    const auto result = iter(int_vec)
        | any([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == 5;
        });

    ASSERT_TRUE(result);
}

TEST_F(Ops, AnyFalse) {
    const auto result = iter(int_vec)
        | any([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == 7;
        });

    ASSERT_FALSE(result);
}

TEST_F(Ops, ClonedRef) {
    const auto str_dup_vec = iter(str_vec)
        | cloned()
        | collect<vector<string>>();

    ASSERT_TRUE(details::no_mismatch_values(str_vec, str_dup_vec,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const string &>::value,
                "lhs is expected to be of const string & type");

            static_assert(is_same<decltype(rhs), const string &>::value,
                "rhs is expected to be of const string & type");

            // same value but different addresses
            return (lhs == rhs) && (&lhs != &rhs);
        }));
}

TEST_F(Ops, ClonedValue) {
    const auto int_str_vec = iter(int_vec)
        | map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        })
        | cloned() | cloned() | cloned()
        | collect<vector<string>>();

    ASSERT_TRUE(details::no_mismatch_values(int_vec, int_str_vec,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const int &>::value,
                "lhs is expected to be of const int & type");

            static_assert(is_same<decltype(rhs), const string &>::value,
                "rhs is expected to be of const string & type");

            return to_string(lhs) == rhs;
        }));
}

TEST_F(Ops, CollectVec) {
    const auto dup_vec = range(0, int_vec.size())
        | collect<vector<int>>();

    ASSERT_TRUE(details::no_mismatch_values(int_vec, dup_vec));
}

TEST_F(Ops, CollectList) {
    const auto dup_cont = iter(int_vec)
        | collect<list<reference_wrapper<const int>>>();

    ASSERT_TRUE(details::no_mismatch_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectSet) {
    const auto dup_cont = iter(int_vec)
        | collect<set<reference_wrapper<const int>>>();

    ASSERT_TRUE(details::no_mismatch_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectUnorderedSet) {
    // unordered_set cannot take in reference_wrapper as template type by default
    const auto dup_cont = iter(int_vec)
        | collect<unordered_set<int>>();

    ASSERT_TRUE(details::similar_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectMap) {
    const auto dup_cont = iter(int_vec)
        | zip(iter(str_vec))
        | collect<std::map<reference_wrapper<const int>, reference_wrapper<const string>>>();

    // check the keys
    ASSERT_TRUE(details::no_mismatch_values(int_vec, dup_cont,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const int &>::value,
                "lhs is expected to be of const int & type");

            static_assert(
                is_same<decltype(rhs),
                const pair<const reference_wrapper<const int>, reference_wrapper<const string>> &>::value,
                "rhs is expected to be of const pair<...> & type");

            return lhs == rhs.first.get();
        }));

    // check the values
    const auto dup_vals = iter(dup_cont)
        | map([](const auto &p) {
            static_assert(
                is_same<decltype(p),
                const pair<const reference_wrapper<const int>, reference_wrapper<const string>> &>::value,
                "lhs is expected to be of const pair<...> & type");

            return cref(p.second);
        })
        | collect<vector<reference_wrapper<const string>>>();

    ASSERT_TRUE(details::no_mismatch_values(str_vec, dup_vals,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const string &>::value,
                "lhs is expected to be of const string & type");

            static_assert(is_same<decltype(rhs), const reference_wrapper<const string> &>::value,
                "rhs is expected to be of const ref<const string> & type");

            return lhs == rhs.get();
        }));
}

TEST_F(Ops, CollectUnorderedMap) {
    const auto dup_cont = iter(int_vec)
        | zip(iter(str_vec))
        | collect<unordered_map<int, reference_wrapper<const string>>>();

    // check the keys
    const auto dup_keys = iter(dup_cont)
        | map([](const auto &p) {
            static_assert(is_same<
                decltype(p),
                const pair<const int, reference_wrapper<const string>> &>::value,
                "p is expected to be of const pair<...> & type");

            return cref(p.first);
        })
        | collect<vector<reference_wrapper<const int>>>();

    ASSERT_TRUE(details::similar_values(int_vec, dup_keys));
 
    // check the values
    const auto dup_vals = iter(dup_cont)
        | map([](const auto &p) {
            static_assert(is_same<
                decltype(p),
                const pair<const int, reference_wrapper<const string>> &>::value,
                "p is expected to be of const pair<...> & type");

            return cref(p.second);
        })
        | collect<vector<reference_wrapper<const string>>>();

    ASSERT_TRUE(details::similar_values(str_vec, dup_vals,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const string &>::value,
                "lhs is expected to be of const string & type");

            static_assert(is_same<decltype(rhs), const reference_wrapper<const string> &>::value,
                "rhs is expected to be of const ref<const string> & type");

            return lhs == rhs.get();
        }));
}

TEST_F(Ops, CollectStack) {
    auto dup_cont = iter(int_vec)
        | collect<stack<reference_wrapper<const int>>>();

    while (!dup_cont.empty()) {
        const auto top = dup_cont.top();
        ASSERT_EQ(int_vec[dup_cont.size() - 1], top);
        dup_cont.pop();
    }
}

TEST_F(Ops, CollectQueue) {
    auto dup_cont = iter(int_vec)
        | collect<queue<reference_wrapper<const int>>>();

    for (size_t i = 0; i < int_vec.size(); ++i) {
        const auto front = dup_cont.front();
        ASSERT_EQ(int_vec[i], front);
        dup_cont.pop();
    }
}

TEST_F(Ops, CollectResultOk) {
    vector<Result<int, string>> res_vec{
        Ok(0), Ok(1), Ok(2)
    };

    auto collected_res = into_iter(move(res_vec))
        | collect<Result<vector<int>, string>>();

    ASSERT_TRUE(collected_res.is_ok());

    const auto collected = move(collected_res).unwrap_unchecked();
    ASSERT_EQ(3, collected.size());
    ASSERT_EQ(0, collected.at(0));
    ASSERT_EQ(1, collected.at(1));
    ASSERT_EQ(2, collected.at(2));
}

TEST_F(Ops, CollectResultErr) {
    static const int ZERO = 0;
    static const int TWO = 2;
    static const string ERR_MSG("ERROR!");

    vector<Result<const int &, const string &>> res_vec{
        Ok(cref(ZERO)), Err(cref(ERR_MSG)), Ok(cref(TWO))
    };

    auto collected_res = into_iter(move(res_vec))
        | collect<Result<vector<reference_wrapper<const int>>, const string &>>();

    ASSERT_TRUE(collected_res.is_err());

    const auto collected = move(collected_res).unwrap_err_unchecked();
    ASSERT_EQ("ERROR!", collected);
}

TEST_F(Ops, CollectVecRef) {
    const auto str_ref_vec = iter(str_vec)
        | collect<vector<reference_wrapper<const string>>>();

    ASSERT_TRUE(details::no_mismatch_values(str_vec, str_ref_vec,
        [](const auto &lhs, const auto &rhs) {
            static_assert(is_same<decltype(lhs), const string &>::value,
                "lhs is expected to be of const string & type");

            static_assert(is_same<decltype(rhs), const reference_wrapper<const string> &>::value,
                "rhs is expected to be of const ref<const string> & type");

            return &lhs == &rhs.get();
        }));
}

TEST_F(Ops, CollectMapVecSum) {
    const auto dup_vec = range(0, int_vec.size())
        | map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value + 0.5;
        })
        | collect<vector<double>>();

    const auto fold_sum = iter(dup_vec)
        | fold(0.0, plus<double>());

    const auto expected_sum = accumulate(
        cbegin(int_vec), cend(int_vec),
        0.0,
        [](const auto acc, const auto value) {
            static_assert(is_same<decltype(acc), const double>::value,
                "acc is expected to be of const double type");

            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return acc + value + 0.5;
        });

    ASSERT_EQ(expected_sum, fold_sum);
}

TEST_F(Ops, Cycle) {
    const vector<string> vs{"Hello", "World"};

    const auto output_vs = iter(vs)
        | cycle()
        | take(5)
        | collect<vector<string>>();

    ASSERT_EQ(5, output_vs.size());
    ASSERT_EQ("Hello", output_vs[0]);
    ASSERT_EQ("World", output_vs[1]);
    ASSERT_EQ("Hello", output_vs[2]);
    ASSERT_EQ("World", output_vs[3]);
    ASSERT_EQ("Hello", output_vs[4]);
}

TEST_F(Ops, CycleNone) {
    const vector<string> vs{};

    const auto output_vs = iter(vs)
        | cycle()
        | collect<vector<string>>();

    ASSERT_TRUE(output_vs.empty());
}

TEST_F(Ops, Once) {
    auto movable_value = make_unique<string>("Hello");

    const auto vec = once(move(movable_value))
        | take(5)
        | collect<vector<unique_ptr<string>>>();

    ASSERT_EQ(1, vec.size());
    ASSERT_EQ("Hello", *vec[0]);
}

TEST_F(Ops, Enumerate) {
    int sum = 0;

    iter(int_vec)
        | enumerate()
        | for_each([&sum](const auto &p) {
            static_assert(is_same<decltype(p), const pair<size_t, const int &> &>::value,
                "p is expected to be of const pair<...> & type");

            // first and second are the same values
            sum += p.first + p.second;
        });

    // * 2 because the index and values are the same values
    // thus adding both together create a * 2 effect
    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 2, sum);
}

TEST_F(Ops, Filter) {
    int sum = 0;

    iter(int_vec)
        | filter([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 2 == 1;
        })
        | for_each([&sum](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            sum += value;
        });

    ASSERT_EQ(9, sum);
}

TEST_F(Ops, FilterMap) {
    double sum = 0;

    iter(int_vec)
        | filter_map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 2 == 1
                ? Some(value + 0.5)
                : None;
        })
        | for_each([&sum](const auto value) {
            static_assert(is_same<decltype(value), const double>::value,
                "value is expected to be of const double type");

            sum += value;
        });

    ASSERT_EQ(10.5, sum);
}

TEST_F(Ops, FindSome) {
    const auto find_some_opt = iter(int_vec)
        | find([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == 5;
        });

    ASSERT_TRUE(find_some_opt.is_some());
    ASSERT_EQ(5, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindNone) {
    const auto find_none_opt = iter(int_vec)
        | find([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == 6;
        });

    ASSERT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, FindMapSome) {
    const auto find_some_opt = iter(int_vec)
        | find_map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == 4
                ? Some(value + 0.5)
                : None;
        });
    
    ASSERT_TRUE(find_some_opt.is_some());
    ASSERT_EQ(4.5, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindMapNone) {
    const auto find_none_opt = iter(int_vec)
        | find_map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value == -1 
                ? Some(value)
                : None;
        });

    ASSERT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, FlatMapEmpty) {
    auto vv = vector<vector<int>>{};

    const auto v_out = into_iter(move(vv))
        | flat_map([](auto &&v) {
            static_assert(is_same<decltype(v), vector<int> &&>::value,
                "v is expected to be of vector<int> && type");
                
            return move(v);
        })
        | collect<vector<int>>();

    ASSERT_TRUE(v_out.empty());
}

TEST_F(Ops, FlatMapSimple) {
    auto vv = vector<vector<int>>{
        {}, {0, 1, 2}, {3}, {}, {4, 5}};

    const auto v_out = into_iter(move(vv))
        | flat_map([](auto &&v) {
            static_assert(is_same<decltype(v), vector<int> &&>::value,
                "v is expected to be of vector<int> && type");

            return move(v);
        })
        | collect<vector<int>>();

    ASSERT_TRUE(details::no_mismatch_values(array<int, 6>{0, 1, 2, 3, 4, 5}, v_out));
}

TEST_F(Ops, Fold) {
    const auto fold_sum = iter(int_vec)
        | fold(10, plus<int>());

    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 10), fold_sum);
}

TEST_F(Ops, ForEach) {
    int sum = 0;

    iter(int_vec)
        | for_each([&sum](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            sum += value;
        });

    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

TEST_F(Ops, Map) {
    double sum = 0.0;

    iter(int_vec)
        | map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value * 0.5;
        })
        | for_each([&sum](const auto value) {
            static_assert(is_same<decltype(value), const double>::value,
                "value is expected to be of const double type");

            sum += value;
        });

    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 0.5, sum);
}

TEST_F(Ops, MaxNone) {
    static const vector<int> VALS;
    const auto max_opt = iter(VALS) | max();

    ASSERT_TRUE(max_opt.is_none());
}

TEST_F(Ops, MaxSomeEasy) {
    static const vector<string> VALS{"zzz", "hello", "world"};
    const auto max_opt = iter(VALS) | max();

    ASSERT_TRUE(max_opt.is_some());
    ASSERT_EQ("zzz", max_opt.get_unchecked());
}

TEST_F(Ops, MaxSomeHard) {
    static const vector<int> VALS{-5, 1, 777, 123, 25, 0, 777, 777};

    // hand coded max to get the address eventually
    ASSERT_TRUE(!VALS.empty());

    size_t max_index = 0;

    for (size_t i = 1; i < VALS.size(); ++i) {
        if (VALS[i] > VALS[max_index]) {
            max_index = i;
        }
    }

    // found the value and referencing it
    const auto &max_val = VALS[max_index];
    const auto max_opt = iter(VALS) | max();

    ASSERT_TRUE(max_opt.is_some());
    ASSERT_EQ(777, max_opt.get_unchecked());
    ASSERT_EQ(&max_val, &max_opt.get_unchecked());
}

TEST_F(Ops, MaxByNone) {
    static const vector<int> VALS;

    const auto max_opt = iter(VALS)
        | max_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs >= rhs;
        });

    ASSERT_TRUE(max_opt.is_none());
}

TEST_F(Ops, MaxBySomeEasy) {
    static const vector<int> VALS{3, 1, 2};

    const auto max_opt = iter(VALS)
        | max_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs >= rhs;
        });

    ASSERT_TRUE(max_opt.is_some());
    ASSERT_EQ(3, max_opt.get_unchecked());
}

TEST_F(Ops, MaxBySomeHard) {
    static const vector<int> VALS{5, 2, 7, 3, 3, 2, 2, 9};

    // hand coded max to get the address eventually
    ASSERT_TRUE(!VALS.empty());

    size_t max_index = 0;

    for (size_t i = 1; i < VALS.size(); ++i) {
        if (VALS[i] > VALS[max_index]) {
            max_index = i;
        }
    }

    // found the value and referencing it
    const auto &max_val = VALS[max_index];

    const auto max_opt = iter(VALS)
        | max_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs >= rhs;
        });

    ASSERT_TRUE(max_opt.is_some());
    ASSERT_EQ(9, max_opt.get_unchecked());
    ASSERT_EQ(&max_val, &max_opt.get_unchecked());
}

TEST_F(Ops, MinNone) {
    static const vector<int> VALS;
    const auto min_opt = iter(VALS) | min();

    ASSERT_TRUE(min_opt.is_none());
}

TEST_F(Ops, MinSomeEasy) {
    static const vector<string> VALS{"hello", "world", "zzz"};
    const auto min_opt = iter(VALS) | min();

    ASSERT_TRUE(min_opt.is_some());
    ASSERT_EQ("hello", min_opt.get_unchecked());
}

TEST_F(Ops, MinSomeHard) {
    static const vector<int> VALS{-5, 1, -777, 123, 25, 0, -777, -777};

    // hand coded min to get the address eventually
    ASSERT_TRUE(!VALS.empty());

    size_t min_index = 0;

    for (size_t i = 1; i < VALS.size(); ++i) {
        if (VALS[i] < VALS[min_index]) {
            min_index = i;
        }
    }

    // found the value and referencing it
    const auto &min_val = VALS[min_index];
    const auto min_opt = iter(VALS) | min();

    ASSERT_TRUE(min_opt.is_some());
    ASSERT_EQ(-777, min_opt.get_unchecked());
    ASSERT_EQ(&min_val, &min_opt.get_unchecked());
}

TEST_F(Ops, MinByNone) {
    static const vector<int> VALS;

    const auto min_opt = iter(VALS)
        | min_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs <= rhs;
        });

    ASSERT_TRUE(min_opt.is_none());
}

TEST_F(Ops, MinBySomeEasy) {
    static const vector<int> VALS{1, 3, 2};

    const auto min_opt = iter(VALS)
        | min_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs <= rhs;
        });

    ASSERT_TRUE(min_opt.is_some());
    ASSERT_EQ(1, min_opt.get_unchecked());
}

TEST_F(Ops, MinBySomeHard) {
    static const vector<int> VALS{5, 2, 7, 3, 3, 2, 2, 9};

    // hand coded min to get the address eventually
    ASSERT_TRUE(!VALS.empty());

    size_t min_index = 0;

    for (size_t i = 1; i < VALS.size(); ++i) {
        if (VALS[i] < VALS[min_index]) {
            min_index = i;
        }
    }

    // found the value and referencing it
    const auto &min_val = VALS[min_index];

    const auto min_opt = iter(VALS)
        | min_by([](const auto lhs, const auto rhs) {
            static_assert(is_same<decltype(lhs), const int>::value,
                "lhs is expected to be of const int type");

            static_assert(is_same<decltype(rhs), const int>::value,
                "rhs is expected to be of const int type");

            return lhs <= rhs;
        });

    ASSERT_TRUE(min_opt.is_some());
    ASSERT_EQ(2, min_opt.get_unchecked());
    ASSERT_EQ(&min_val, &min_opt.get_unchecked());
}

TEST_F(Ops, Range) {
    const auto sum = range(0, 6)
        | fold(5, plus<int>());

    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 5), sum);
}

TEST_F(Ops, SkipWithin) {
    const auto sum = iter(int_vec)
        | skip(3)
        | fold(0, plus<int>());

    ASSERT_EQ(12, sum);
}

TEST_F(Ops, SkipPass) {
    const auto sum = iter(int_vec)
        | skip(100)
        | fold(0, plus<int>());

    ASSERT_EQ(0, sum);
}

TEST_F(Ops, TakeWithin) {
    const auto sum = iter(int_vec)
        | take(3)
        | fold(0, plus<int>());

    ASSERT_EQ(3, sum);
}

TEST_F(Ops, TakeExceed) {
    const auto sum = iter(int_vec)
        | take(100)
        | fold(0, plus<int>());

    ASSERT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

// complex tests

TEST_F(ComplexOps, ManyIterCollect) {
    vector<unique_ptr<int>> v;
    v.push_back(make_unique<int>(0));
    v.push_back(make_unique<int>(1));
    v.push_back(make_unique<int>(2));
    v.push_back(make_unique<int>(3));
    v.push_back(make_unique<int>(4));

    // collect as reference once
    const auto v2 = iter(v)
        | collect<vector<reference_wrapper<const unique_ptr<int>>>>();

    // collect as reference twice
    // note how reference_wrapper<reference_wrapper>
    // got collapsed back into just reference_wrapper
    // this is because c++ does not allow reference_wrapper<reference_wrapper>
    // so it is a must to collapse anyway
    // which makes a slightly deviation from Rust ref of ref (which is possible)

    const auto v3 = iter(v2)
        | collect<vector<reference_wrapper<const unique_ptr<int>>>>();

    const auto v4 = iter(v3)
        | map([](const auto &r_ptr) {
            static_assert(is_same<decltype(r_ptr), const unique_ptr<int> &>::value,
                "r_ptr is expected to be of const unique_ptr<int> & type");

            return cref(*r_ptr);
        })
        | filter([](const auto &r) {
            static_assert(is_same<decltype(r), const int &>::value,
                "r is expected to be of const int & type");

            return r % 2 == 1;
        })
        | collect<vector<reference_wrapper<const int>>>();

    // no copy of any value was ever done from the original v

    ASSERT_EQ(2, v4.size());
    ASSERT_EQ(1, v4.at(0));
    ASSERT_EQ(3, v4.at(1));
}

TEST_F(ComplexOps, FilterMapFold) {
    const auto eleven_div_str = range(1, 100)
        | filter([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 11 == 0;
        })
        | map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        })
        | fold(string{}, [](const auto acc, const auto &rhs) {
            static_assert(is_same<decltype(acc), const string>::value,
                "acc is expected to be of const string type");

            static_assert(is_same<decltype(rhs), const string &>::value,
                "rhs is expected to be of const string & type");
                
            return acc + rhs + " ";
        });

    ASSERT_EQ("11 22 33 44 55 66 77 88 99 ", eleven_div_str);
}

TEST_F(ComplexOps, FilterMapFind) {
    const auto find_opt = range(1, 100)
        | filter([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return value % 17 == 0;
        })
        | map([](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            // .5 is a easily representable value in mantissa
            // so that the floating float comparison can directly compare the values
            return value + 0.5;
        })
        | find([](const auto value) {
            static_assert(is_same<decltype(value), const double>::value,
                "value is expected to be of const double type");

            return value == 34.5;
        });

    ASSERT_TRUE(find_opt.is_some());
    ASSERT_EQ(34.5, find_opt.get_unchecked());
}

TEST_F(ComplexOps, ZipRefMapFold) {
    const auto zipped_int_vec = iter(int_vec)
        | zip(iter(int_vec) | skip(1))
        | collect<vector<pair<const int &, const int &>>>();

    const auto folded_str = iter(zipped_int_vec)
        | map([](const auto &index_value_pair) {
            static_assert(is_same<
                decltype(index_value_pair),
                const pair<const int &, const int &> &>::value,
                "index_value_pair is expected to be of const pair<...> type");
                
            stringstream ss;
            ss << '(' << index_value_pair.first << ',' << index_value_pair.second << ')';
            return ss.str();
        })
        | fold(string{}, [](const auto acc, const auto value) {
            static_assert(is_same<decltype(acc), const string>::value,
                "acc is expected to be of const string type");

            static_assert(is_same<decltype(value), const string>::value,
                "value is expected to be of const string type");

            stringstream ss;
            ss << acc << value << ' ';
            return ss.str();
        });

    ASSERT_EQ("(0,1) (1,2) (2,3) (3,4) (4,5) ", folded_str);
}

// option

TEST(Option, Assignment) {
    auto opt = Some(1);
    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(1, opt.get_unchecked());

    opt = None;
    ASSERT_TRUE(opt.is_none());

    opt = Some(7);
    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorNone) {
    Option<int> opt(None);
    ASSERT_TRUE(opt.is_none());
}

TEST(Option, CtorLRef) {
    auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    ASSERT_TRUE(opt.is_some());
    ASSERT_TRUE(opt_rhs.is_some());

    ASSERT_EQ(7, opt.get_unchecked());
    ASSERT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorConstLRef) {
    const auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    ASSERT_TRUE(opt.is_some());
    ASSERT_TRUE(opt_rhs.is_some());

    ASSERT_EQ(7, opt.get_unchecked());
    ASSERT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorMoveSimple) {
    Option<int> opt_rhs(Some(7));
    const Option<int> opt(move(opt_rhs));

    ASSERT_TRUE(opt.is_some());
    ASSERT_TRUE(opt_rhs.is_none());
    ASSERT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorMoveComplex) {
    auto opt_rhs = Some(make_unique<int>(7));
    const Option<unique_ptr<int>> opt(move(opt_rhs));

    ASSERT_TRUE(opt.is_some());
    ASSERT_TRUE(opt_rhs.is_none());
    ASSERT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MapSome) {
    const auto opt = Some(make_unique<int>(777))
        .map([](auto &&value) {
            static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            return make_unique<string>(to_string(*value));
        })
        .map([](auto &&value) {
            static_assert(is_same<decltype(value), unique_ptr<string> &&>::value,
                "value is expected to be of unique_ptr<string> && type");

            return "*" + *value + "*";
        });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("*777*", opt.get_unchecked());
}

TEST(Option, MapNone) {
    auto opt = Option<int>(None);

    move(opt).map([](auto &&value) {
        static_assert(is_same<decltype(value), int &&>::value,
            "value is expected to be of int && type");

        return "Hello";
    });

    ASSERT_TRUE(opt.is_none());
}

TEST(Option, AndThenSomeToSome) {
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value) {
            static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            return Some(make_unique<string>(to_string(*value)));
        });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("777", *opt.get_unchecked());
}

TEST(Option, AndThenSomeToNone) {
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value) -> Option<string> {
            static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");

            return None;
        });

    ASSERT_TRUE(opt.is_none());
}

TEST(Option, AndThenNone) {
    const auto opt = Option<unique_ptr<int>>(None)
        .and_then([](auto &&value) {
            static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
                "value is expected to be of unique_ptr<int> && type");
                
            return Some(make_unique<string>(to_string(*value)));
        });

    ASSERT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToNone) {
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return None; });

    ASSERT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToSome) {
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return Some(make_unique<int>(7)); });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, *opt.get_unchecked());
}

TEST(Option, OkOrElseSome) {
    const auto res = Some(7)
        .ok_or_else([] { return string("Hello"); });

    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ(7, res.get_unchecked());
}

TEST(Option, OkOrElseNone) {
    const auto res = Option<int>(None)
        .ok_or_else([] { return string("Hello"); });

    ASSERT_TRUE(res.is_err());
    ASSERT_EQ("Hello", res.get_err_unchecked());
}

TEST(Option, OkOrElseRefOk) {
    const string ok_value = "Okay";
    const string err_value = "Error";

    const auto res = Some(cref(ok_value))
        .ok_or_else([&err_value] { return cref(err_value); });

    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ("Okay", res.get_unchecked());
    ASSERT_EQ(&ok_value, &res.get_unchecked());
}

TEST(Option, OkOrElseRefErr) {
    const string err_value = "Error";

    const auto res = Option<string>(None)
        .ok_or_else([&err_value] { return cref(err_value); });

    ASSERT_TRUE(res.is_err());
    ASSERT_EQ("Error", res.get_err_unchecked());
    ASSERT_EQ(&err_value, &res.get_err_unchecked());
}

TEST(Option, OrElseSome) {
    const auto opt = Some(7)
        .or_else([] { return None; });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, opt.get_unchecked());
}

TEST(Option, MatchXValueSome) {
    const auto value = Some(7).match(
        [](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");
                
            return to_string(value);
        },
        [] { return "NONE"; });

    ASSERT_EQ("7", value);
}

TEST(Option, MatchMoveSome) {
    auto opt = Some(make_unique<int>(8));

    const auto value = move(opt).match(
        [](const auto &value) {
            static_assert(is_same<decltype(value), const unique_ptr<int> &>::value,
                "value is expected to be of const unique_ptr<int> & type");

            return to_string(*value);
        },
        [] { return "NONE"; });

    ASSERT_TRUE(opt.is_none());
    ASSERT_EQ("8", value);
}

TEST(Option, MatchXValueNone) {
    const auto value = Option<int>(None).match(
        [](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        },
        [] { return "NONE"; });

    ASSERT_EQ("NONE", value);
}

TEST(Option, MatchRefSome) {
    const auto opt = Some(7);

    const auto value = opt.match(
        [](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        },
        [] { return "NONE"; });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("7", value);
}

TEST(Option, MatchRefNone) {
    const auto opt = Option<int>(None);
    
    const auto value = opt.match(
        [](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return to_string(value);
        },
        [] { return "NONE"; });

    ASSERT_TRUE(opt.is_none());
    ASSERT_EQ("NONE", value);
}

TEST(Option, MatchSomeXValueSomeAssignBack) {
    auto ptr = make_unique<int>(7);

    Some(move(ptr)).match_some([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
            "value is expected to be of unique_ptr<int> && type");

        // assign back so that there is something to test
        ptr = move(value);
    });

    ASSERT_TRUE(static_cast<bool>(ptr));
    ASSERT_EQ(7, *ptr);
}

TEST(Option, MatchSomeXValueSomeNoEffect) {
    auto ptr = make_unique<int>(7);

    Some(move(ptr)).match_some([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
            "value is expected to be of unique_ptr<int> && type");

        // do nothing
    });

    ASSERT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchSomeXValueNone) {
    auto ptr = make_unique<int>(7);

    Option<int>(None).match_some([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), int &&>::value,
            "value is expected to be of int && type");

        // not happening
        ptr = nullptr;
    });

    ASSERT_TRUE(static_cast<bool>(ptr));
    ASSERT_EQ(7, *ptr);
}

TEST(Option, MatchSomeRefDoSomething) {
    const auto opt = Some(make_unique<int>(7));
    bool flag = false;

    opt.match_some([&flag](const auto &value) {
        static_assert(is_same<decltype(value), const unique_ptr<int> &>::value,
            "value is expected to be of const unique_ptr<int> & type");

        flag = true;
    });

    ASSERT_TRUE(flag);
    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MatchSomeRefNoEffect) {
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_some([&flag](const auto &value) {
        static_assert(is_same<decltype(value), const int &>::value,
            "value is expected to be of const int & type");

        flag = true;
    });

    ASSERT_FALSE(flag);
    ASSERT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneXValueNoneDoSomething) {
    unique_ptr<int> ptr;

    Option<int>(None).match_none([&ptr] {
        // assign back so that there is something to test
        ptr = make_unique<int>(8);
    });

    ASSERT_TRUE(static_cast<bool>(ptr));
    ASSERT_EQ(8, *ptr);
}

TEST(Option, MatchNoneXValueSomeNoEffect) {
    unique_ptr<int> ptr;

    Some(777).match_none([&ptr] {
        // assign back so that there is something to test
        ptr = make_unique<int>(777);
    });

    ASSERT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchNoneRefDoSomething) {
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_none([&flag] {
        flag = true;
    });

    ASSERT_TRUE(flag);
    ASSERT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneRefNoEffect) {
    const auto opt = Some(7);
    bool flag = false;

    opt.match_none([&flag] {
        flag = true;
    });

    ASSERT_FALSE(flag);
    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, opt.get_unchecked());
}

TEST(Option, GetUnchecked) {
    const auto opt = Some(make_unique<string>("Hello"));

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("Hello", *opt.get_unchecked());
}

TEST(Option, UnwrapUnchecked) {
    string x = "Hello";
    auto opt = Some(ref(x));

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("Hello", move(opt).unwrap_unchecked());
}

TEST(Option, OptIfTrue) {
    const auto opt = opt_if(true,
        [] { return 7; });

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(7, opt.get_unchecked());
}

TEST(Option, OptIfFalse) {
    const auto opt = opt_if(false,
        [] { return 0; });

    ASSERT_TRUE(opt.is_none());
}

// result

TEST(Result, ResIfElseTrue) {
    const auto res = res_if_else(true,
        [] { return 1; },
        [] { return 3.14; });

    ASSERT_TRUE(res.is_ok());
}

TEST(Result, ResIfElseFalse) {
    const auto res = res_if_else(false,
        [] { return 1; },
        [] { return 3.14; });

    ASSERT_TRUE(res.is_err());
}

TEST(Result, OkSome) {
    Result<int, string> res = Ok(8);
    const auto opt = move(res).ok();

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ(8, opt.get_unchecked());
}

TEST(Result, OkNone) {
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).ok();

    ASSERT_TRUE(opt.is_none());
}

TEST(Result, ErrSome) {
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).err();

    ASSERT_TRUE(opt.is_some());
    ASSERT_EQ("World", opt.get_unchecked());
}

TEST(Result, ErrNone) {
    Result<int, string> res = Ok(9);
    const auto opt = move(res).err();

    ASSERT_TRUE(opt.is_none());
}

TEST(Result, MapOk) {
    Result<int, string> res = Ok(1);
    const auto mapped_res = move(res).map([](const auto value) {
        static_assert(is_same<decltype(value), const int>::value,
            "value is expected to be of const int type");

        return value + 0.5;
    });

    ASSERT_TRUE(mapped_res.is_ok());
    ASSERT_EQ(1.5, mapped_res.get_unchecked());
}

TEST(Result, MapErr) {
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map([](const auto value) {
        static_assert(is_same<decltype(value), const int>::value,
            "value is expected to be of const int type");

        return to_string(value);
    });

    ASSERT_TRUE(mapped_res.is_err());
    ASSERT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrErr) {
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map_err([](const auto &value) {
        static_assert(is_same<decltype(value), const string &>::value,
            "value is expected to be of const string & type");

        return value + " value";
    });

    ASSERT_TRUE(mapped_res.is_err());
    ASSERT_EQ("Error value", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrOk) {
    Result<int, int> res = Ok(-1);
    const auto mapped_res = move(res).map_err([](const auto value) {
        static_assert(is_same<decltype(value), const int>::value,
            "value is expected to be of const int type");

        return to_string(value);
    });

    ASSERT_TRUE(mapped_res.is_ok());
    ASSERT_EQ(-1, mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToOk) {
    Result<int, string> res = Ok(1);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<string, string> {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return Ok(to_string(value));
        });

    ASSERT_TRUE(mapped_res.is_ok());
    ASSERT_EQ("1", mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToErr) {
    Result<int, string> res = Ok(2);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return Err(to_string(value));
        });

    ASSERT_TRUE(mapped_res.is_err());
    ASSERT_EQ("2", mapped_res.get_err_unchecked());
}

TEST(Result, AndThenErr) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return Ok(value);
        });

    ASSERT_TRUE(mapped_res.is_err());
    ASSERT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseErrToOk) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto &value) -> Result<int, int> {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return Ok(7);
        });

    ASSERT_TRUE(mapped_res.is_ok());
    ASSERT_EQ(7, mapped_res.get_unchecked());
}

TEST(Result, OrElseErrToErr) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto &value) -> Result<int, string> {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return Err(string{"Still error"});
        });

    ASSERT_TRUE(mapped_res.is_err());
    ASSERT_EQ("Still error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseOk) {
    Result<int, string> res = Ok(5);

    const auto mapped_res = move(res)
        .or_else([](const auto &value) -> Result<int, double> {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return Ok(6);
        });

    ASSERT_TRUE(mapped_res.is_ok());
    ASSERT_EQ(5, mapped_res.get_unchecked());
}

TEST(Result, MatchOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));

    const auto match_res = move(res).match(
        [](auto &&value) {
            // static_assert(is_same<decltype(value), unique_ptr<int>>::value,
            //    "value is expected to be of unique_ptr<int> type");

            return true;
        },
        [](auto &&value) {
            // static_assert(is_same<decltype(value), unique_ptr<string> &&>::value,
            //     "value is expected to be of unique_ptr<string> && type");

            return false;
        });

    ASSERT_TRUE(match_res);
}

TEST(Result, MatchErr) {
    Result<int, string> res = Err(string{"Hello"});

    const auto match_res = move(res).match(
        [](const auto value) {
            static_assert(is_same<decltype(value), const int>::value,
                "value is expected to be of const int type");

            return 0.5;
        },
        [](const auto &value) {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return 0.25;
        });

    ASSERT_EQ(0.25, match_res);
}

TEST(Result, MatchOkRef) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(777));

    const auto match_res = res.match(
        [](const auto &value) {
            static_assert(is_same<decltype(value), const unique_ptr<int> &>::value,
                "value is expected to be of const unique_ptr<int> & type");

            return to_string(*value);
        },
        [](const auto &value) {
            static_assert(is_same<decltype(value), const unique_ptr<string> &>::value,
                "value is expected to be of const unique_ptr<string> & type");

            return *value;
        });

    ASSERT_EQ("777", match_res);
}

TEST(Result, MatchErrRef) {
    Result<string, string> res = Err(string{"World"});

    const auto match_res = res.match(
        [](const auto &value) {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return cref(value);
        },
        [](const auto &value) {
            static_assert(is_same<decltype(value), const string &>::value,
                "value is expected to be of const string & type");

            return cref(value);
        });

    ASSERT_TRUE(res.is_err());
    ASSERT_EQ(&res.get_err_unchecked(), &match_res.get());
    ASSERT_EQ("World", match_res.get());
}

TEST(Result, MatchOkOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    unique_ptr<int> ptr;

    move(res).match_ok([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
            "value is expected to be of unique_ptr<int> && type");

        ptr = move(value);
    });

    ASSERT_TRUE(static_cast<bool>(ptr));
    ASSERT_EQ(2, *ptr);
}

TEST(Result, MatchOkErr) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    unique_ptr<int> ptr;

    move(res).match_ok([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<int> &&>::value,
            "value is expected to be of unique_ptr<int> && type");

        ptr = move(value);
    });

    ASSERT_FALSE(static_cast<bool>(ptr));
}

TEST(Result, MatchOkRefOk) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    int value = 0;

    res.match_ok([&value](const auto &res_value) {
        static_assert(is_same<decltype(res_value), const unique_ptr<int> &>::value,
            "res_value is expected to be of const unique_ptr<int> & type");

        value = *res_value;
    });

    ASSERT_EQ(2, value);
    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ(2, *res.get_unchecked());
}

TEST(Result, MatchOkRefErr) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    int value = 7;

    res.match_ok([&value](const auto &res_value) {
        static_assert(is_same<decltype(res_value), const unique_ptr<int> &>::value,
            "res_value is expected to be of const unique_ptr<int> & type");

        value = *res_value;
    });

    ASSERT_EQ(7, value);
    ASSERT_TRUE(res.is_err());
    ASSERT_EQ("Hello", *res.get_err_unchecked());
}

TEST(Result, MatchErrOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    unique_ptr<string> ptr;

    move(res).match_err([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<string> &&>::value,
            "value is expected to be of unique_ptr<string> && type");

        ptr = move(value);
    });

    ASSERT_FALSE(static_cast<bool>(ptr));
}

TEST(Result, MatchErrErr) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    unique_ptr<string> ptr;

    move(res).match_err([&ptr](auto &&value) {
        static_assert(is_same<decltype(value), unique_ptr<string> &&>::value,
            "value is expected to be of unique_ptr<string> && type");

        ptr = move(value);
    });

    ASSERT_TRUE(static_cast<bool>(ptr));
    ASSERT_EQ("Hello", *ptr);
}

TEST(Result, MatchErrRefOk) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    string value = "World";

    res.match_err([&value](const auto &res_value) {
        static_assert(is_same<decltype(res_value), const unique_ptr<string> &>::value,
            "res_value is expected to be of const unique_ptr<string> & type");
            
        value = *res_value;
    });

    ASSERT_EQ("World", value);
    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ(2, *res.get_unchecked());
}

TEST(Result, MatchErrRefErr) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    string value = "World";

    res.match_err([&value](auto &&res_value) {
        static_assert(is_same<decltype(res_value), const unique_ptr<string> &>::value,
            "res_value is expected to be of const unique_ptr<string> & type");
            
        value = *res_value;
    });

    ASSERT_EQ("Hello", value);
    ASSERT_TRUE(res.is_err());
    ASSERT_EQ("Hello", *res.get_err_unchecked());
}

TEST(Result, GetUnchecked) {
    const Result<int, string> res = Ok(7);

    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ(7, res.get_unchecked());
}

TEST(Result, GetErrUnchecked) {
    const Result<int, string> res = Err(string{"Hello"});

    ASSERT_TRUE(res.is_err());
    ASSERT_EQ("Hello", res.get_err_unchecked());
}

TEST(Result, UnwrapUnchecked) {
    int x = 7;
    Result<int &, int> res = Ok(ref(x));

    ASSERT_TRUE(res.is_ok());

    // to show that it returns reference
    static_assert(is_same<
        decltype(move(res).unwrap_unchecked()),
        int &>::value,
        "Result::UnwrapUnchecked does not return reference directly!");

    ASSERT_EQ(7, move(res).unwrap_unchecked());
}

TEST(Result, UnwrapErrUnchecked) {
    Result<string, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    ASSERT_TRUE(res.is_err());

    // to prove that the pointer can move
    auto err_ptr = move(res).unwrap_err_unchecked();
    ASSERT_EQ("Hello", *err_ptr);
}

TEST(Macro, Let) {
    const auto fn = [](const bool flag) -> Result<int, string> {
        auto res = flag
            ? Result<double, string>(Ok(3.14))
            : Err(string("Hello"));

        let(value, res);
        return Ok(static_cast<int>(value));
    };

    {
        const auto res = fn(true);
        ASSERT_TRUE(res.is_ok());
        ASSERT_EQ(3, res.get_unchecked());
    }

    {
        const auto res = fn(false);
        ASSERT_TRUE(res.is_err());
        ASSERT_EQ("Hello", res.get_err_unchecked());
    }
}

TEST(Macro, LetMut) {
    const auto fn = [](const bool flag) -> Result<int, string> {
        auto res = flag
            ? Result<double, string>(Ok(3.14))
            : Err(string("Hello"));

        let_mut(value, res);
        value += 10;
        return Ok(static_cast<int>(value));
    };

    {
        const auto res = fn(true);
        ASSERT_TRUE(res.is_ok());
        ASSERT_EQ(13, res.get_unchecked());
    }

    {
        const auto res = fn(false);
        ASSERT_TRUE(res.is_err());
        ASSERT_EQ("Hello", res.get_err_unchecked());
    }
}

TEST(Macro, LetRef) {
    static const int VAL = 7;

    const auto fn = [](const bool flag) -> Result<const int &, string> {
        auto res = flag
            ? Result<const int &, string>(Ok(cref(VAL)))
            : Err(string("Hello"));

        let_ref(value, res);
        return Ok(cref(value));
    };

    {
        const auto res = fn(true);
        ASSERT_TRUE(res.is_ok());
        ASSERT_EQ(VAL, res.get_unchecked());
        ASSERT_EQ(&VAL, &res.get_unchecked());
    }

    {
        const auto res = fn(false);
        ASSERT_TRUE(res.is_err());
        ASSERT_EQ("Hello", res.get_err_unchecked());
    }
}

TEST(Macro, LetMutRef) {
    static int VAL = 7;

    const auto fn = [](const bool flag) -> Result<const int &, string> {
        auto res = flag
            ? Result<int &, string>(Ok(ref(VAL)))
            : Err(string("Hello"));

        let_mut_ref(value, res);
        value += 10;
        return Ok(cref(value));
    };

    {
        const auto res = fn(true);
        ASSERT_TRUE(res.is_ok());
        ASSERT_EQ(17, res.get_unchecked());
        ASSERT_EQ(&VAL, &res.get_unchecked());
    }

    {
        const auto res = fn(false);
        ASSERT_TRUE(res.is_err());
        ASSERT_EQ("Hello", res.get_err_unchecked());
    }
}

TEST(Macro, RetIfErr) {
    const auto fn = [](const bool flag) -> Result<int, string> {
        auto res = flag
            ? Result<double, string>(Ok(3.14))
            : Err(string("Hello"));

        ret_if_err(res);
        return Ok(0);
    };

    {
        const auto res = fn(true);
        ASSERT_TRUE(res.is_ok());
        ASSERT_EQ(0, res.get_unchecked());
    }

    {
        const auto res = fn(false);
        ASSERT_TRUE(res.is_err());
        ASSERT_EQ("Hello", res.get_err_unchecked());
    }
}

int main(int argc, char * argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
