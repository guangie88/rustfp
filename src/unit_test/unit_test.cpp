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
#include "rustfp/fold.h"
#include "rustfp/for_each.h"
#include "rustfp/iter.h"
#include "rustfp/let.h"
#include "rustfp/map.h"
#include "rustfp/once.h"
#include "rustfp/option.h"
#include "rustfp/range.h"
#include "rustfp/result.h"
#include "rustfp/skip.h"
#include "rustfp/take.h"
#include "rustfp/zip.h"

#include "gtest/gtest.h"

#include <algorithm>
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
using rustfp::fold;
using rustfp::for_each;
using rustfp::into_iter;
using rustfp::iter;
using rustfp::iter_mut;
using rustfp::map;
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
using std::cbegin;
using std::cend;
using std::cout;
using std::cref;
using std::deque;
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
    EXPECT_TRUE(opt0.is_some());
    EXPECT_EQ(0, opt0.get_unchecked());

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        const int &>::value,
        "Ops::Iter failed Option type checking!");
    
    const auto opt1 = it.next();
    EXPECT_TRUE(opt1.is_some());
    EXPECT_EQ(1, opt1.get_unchecked());

    const auto opt2 = it.next();
    EXPECT_TRUE(opt2.is_some());
    EXPECT_EQ(2, opt2.get_unchecked());

    const auto opt3 = it.next();
    EXPECT_TRUE(opt3.is_some());
    EXPECT_EQ(3, opt3.get_unchecked());

    const auto opt4 = it.next();
    EXPECT_TRUE(opt4.is_some());
    EXPECT_EQ(4, opt4.get_unchecked());

    const auto opt5 = it.next();
    EXPECT_TRUE(opt5.is_some());
    EXPECT_EQ(5, opt5.get_unchecked());

    const auto opt6 = it.next();
    EXPECT_TRUE(opt6.is_none());
}

TEST_F(Ops, IterMut) {
    auto v = int_vec;

    // modify the values
    iter_mut(v)
        // can either explicitly state the reference type
        | map([](int &val) {
            val *= val;
            return ref(val);
        })
        // or use auto to infer
        | for_each([](auto &val) {
            val += 1;
        });

    auto it = iter_mut(v);

    static_assert(is_same<
        typename remove_reference_t<decltype(it)>::Item,
        int &>::value,
        "Ops::IterMut failed Iter type checking!");

    const auto opt0 = it.next();
    EXPECT_TRUE(opt0.is_some());
    EXPECT_EQ(1, opt0.get_unchecked());

    static_assert(is_same<
        typename remove_reference_t<decltype(opt0)>::some_t,
        int &>::value,
        "Ops::IterMut failed Option type checking!");
    
    const auto opt1 = it.next();
    EXPECT_TRUE(opt1.is_some());
    EXPECT_EQ(2, opt1.get_unchecked());

    const auto opt2 = it.next();
    EXPECT_TRUE(opt2.is_some());
    EXPECT_EQ(5, opt2.get_unchecked());

    const auto opt3 = it.next();
    EXPECT_TRUE(opt3.is_some());
    EXPECT_EQ(10, opt3.get_unchecked());

    const auto opt4 = it.next();
    EXPECT_TRUE(opt4.is_some());
    EXPECT_EQ(17, opt4.get_unchecked());

    const auto opt5 = it.next();
    EXPECT_TRUE(opt5.is_some());
    EXPECT_EQ(26, opt5.get_unchecked());

    const auto opt6 = it.next();
    EXPECT_TRUE(opt6.is_none());
}

TEST_F(Ops, IterMutChain) {
    auto v = int_vec;

    // modify the values
    const auto v2 = iter_mut(v)
        | filter([](auto val) {
            return val >= 2;
        })
        | map([](int &val) {
            val -= 2;
            return ref(val);
        })
        | collect<vector<reference_wrapper<int>>>();

    EXPECT_EQ(4, v2.size());
    EXPECT_EQ(0, v2.at(0));
    EXPECT_EQ(1, v2.at(1));
    EXPECT_EQ(2, v2.at(2));
    EXPECT_EQ(3, v2.at(3));
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

    EXPECT_TRUE(opt0.is_some());
    auto val0(move(opt0).unwrap_unchecked());
    EXPECT_EQ(0, *val0);

    auto opt1 = it.next();
    EXPECT_TRUE(opt1.is_some());
    auto val1(move(opt1).unwrap_unchecked());
    EXPECT_EQ(1, *val1);

    auto opt2 = it.next();
    EXPECT_TRUE(opt2.is_some());
    auto val2(move(opt2).unwrap_unchecked());
    EXPECT_EQ(2, *val2);

    EXPECT_TRUE(it.next().is_none());
}

TEST_F(Ops, AllTrue) {
    const auto result = iter(int_vec)
        | all([](const auto value) { return value < 6; });

    EXPECT_TRUE(result);
}

TEST_F(Ops, AllFalse) {
    const auto result = iter(int_vec)
        | all([](const auto value) { return value > 0; });

    EXPECT_FALSE(result);
}

TEST_F(Ops, AnyTrue) {
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 5; });

    EXPECT_TRUE(result);
}

TEST_F(Ops, AnyFalse) {
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 7; });

    EXPECT_FALSE(result);
}

TEST_F(Ops, ClonedRef) {
    const auto str_dup_vec = iter(str_vec)
        | cloned()
        | collect<vector<string>>();

    EXPECT_TRUE(details::no_mismatch_values(str_vec, str_dup_vec,
        [](const auto &lhs, const auto &rhs) {
            // same value but different addresses
            return (lhs == rhs) && (&lhs != &rhs);
        }));
}

TEST_F(Ops, ClonedValue) {
    const auto int_str_vec = iter(int_vec)
        | map([](const auto value) { return to_string(value); })
        | cloned() | cloned() | cloned()
        | collect<vector<string>>();

    EXPECT_TRUE(details::no_mismatch_values(int_vec, int_str_vec,
        [](const auto &lhs, const auto &rhs) {
            return to_string(lhs) == rhs;
        }));
}

TEST_F(Ops, CollectVec) {
    const auto dup_vec = range(0, int_vec.size())
        | collect<vector<int>>();

    EXPECT_TRUE(details::no_mismatch_values(int_vec, dup_vec));
}

TEST_F(Ops, CollectList) {
    const auto dup_cont = iter(int_vec)
        | collect<list<reference_wrapper<const int>>>();

    EXPECT_TRUE(details::no_mismatch_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectSet) {
    const auto dup_cont = iter(int_vec)
        | collect<set<reference_wrapper<const int>>>();

    EXPECT_TRUE(details::no_mismatch_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectUnorderedSet) {
    // unordered_set cannot take in reference_wrapper as template type by default
    const auto dup_cont = iter(int_vec)
        | collect<unordered_set<int>>();

    EXPECT_TRUE(details::similar_values(int_vec, dup_cont));
}

TEST_F(Ops, CollectMap) {
    const auto dup_cont = iter(int_vec)
        | zip(iter(str_vec))
        | collect<std::map<reference_wrapper<const int>, reference_wrapper<const string>>>();

    // check the keys
    EXPECT_TRUE(details::no_mismatch_values(int_vec, dup_cont,
        [](const auto &lhs, const auto &rhs) {
            return lhs == rhs.first.get();
        }));

    // check the values
    const auto dup_vals = iter(dup_cont)
        | map([](const auto &p) { return cref(p.second); })
        | collect<vector<reference_wrapper<const string>>>();

    EXPECT_TRUE(details::no_mismatch_values(str_vec, dup_vals,
        [](const auto &lhs, const auto &rhs) {
            return lhs == rhs.get();
        }));
}

TEST_F(Ops, CollectUnorderedMap) {
    const auto dup_cont = iter(int_vec)
        | zip(iter(str_vec))
        | collect<unordered_map<int, reference_wrapper<const string>>>();

    // check the keys
    const auto dup_keys = iter(dup_cont)
        | map([](const auto &p) { return cref(p.first); })
        | collect<vector<reference_wrapper<const int>>>();

    EXPECT_TRUE(details::similar_values(int_vec, dup_keys));
 
    // check the values
    const auto dup_vals = iter(dup_cont)
        | map([](const auto &p) { return cref(p.second); })
        | collect<vector<reference_wrapper<const string>>>();

    EXPECT_TRUE(details::similar_values(str_vec, dup_vals,
        [](const auto &lhs, const auto &rhs) { return lhs == rhs.get(); }));
}

TEST_F(Ops, CollectStack) {
    auto dup_cont = iter(int_vec)
        | collect<stack<reference_wrapper<const int>>>();

    while (!dup_cont.empty()) {
        const auto top = dup_cont.top();
        EXPECT_EQ(int_vec[dup_cont.size() - 1], top);
        dup_cont.pop();
    }
}

TEST_F(Ops, CollectQueue) {
    auto dup_cont = iter(int_vec)
        | collect<queue<reference_wrapper<const int>>>();

    for (size_t i = 0; i < int_vec.size(); ++i) {
        const auto front = dup_cont.front();
        EXPECT_EQ(int_vec[i], front);
        dup_cont.pop();
    }
}

TEST_F(Ops, CollectResultOk) {
    vector<Result<int, string>> res_vec{
        Ok(0), Ok(1), Ok(2)
    };

    auto collected_res = into_iter(move(res_vec))
        | collect<Result<vector<int>, string>>();

    EXPECT_TRUE(collected_res.is_ok());

    const auto collected = move(collected_res).unwrap_unchecked();
    EXPECT_EQ(3, collected.size());
    EXPECT_EQ(0, collected.at(0));
    EXPECT_EQ(1, collected.at(1));
    EXPECT_EQ(2, collected.at(2));
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

    EXPECT_TRUE(collected_res.is_err());

    const auto collected = move(collected_res).unwrap_err_unchecked();
    EXPECT_EQ("ERROR!", collected);
}

TEST_F(Ops, CollectVecRef) {
    const auto str_ref_vec = iter(str_vec)
        | collect<vector<reference_wrapper<const string>>>();

    EXPECT_TRUE(details::no_mismatch_values(str_vec, str_ref_vec,
        [](const auto &lhs, const auto &rhs) {
            return &lhs == &rhs.get();
        }));
}

TEST_F(Ops, CollectMapVecSum) {
    static constexpr auto COLLECT_MAP_VEC_SUM_ADD = 0.5;

    const auto dup_vec = range(0, int_vec.size())
        | map([](const auto value) { return value + COLLECT_MAP_VEC_SUM_ADD; })
        | collect<vector<double>>();

    const auto fold_sum = iter(dup_vec)
        | fold(0.0, plus<double>());

    const auto expected_sum = accumulate(
        cbegin(int_vec), cend(int_vec),
        0.0, [](const double acc, const int value) { return acc + value + COLLECT_MAP_VEC_SUM_ADD; });

    EXPECT_EQ(expected_sum, fold_sum);
}

TEST_F(Ops, Cycle) {
    const int copyable_value = 7;

    const auto sum = cycle(cref(copyable_value))
        | take(1000)
        | fold(0, [](const auto acc, const auto value) {
            return acc + value;
        });

    EXPECT_EQ(7000, sum);
}

TEST_F(Ops, Once) {
    auto movable_value = make_unique<string>("Hello");

    const auto vec = once(move(movable_value))
        | take(5)
        | collect<vector<unique_ptr<string>>>();

    EXPECT_EQ(1, vec.size());
    EXPECT_EQ("Hello", *vec[0]);
}

TEST_F(Ops, Enumerate) {
    int sum = 0;

    iter(int_vec)
        | enumerate()
        | for_each([&sum](const auto pair) {
            // first and second are the same values
            sum += pair.first + pair.second;
        });

    // * 2 because the index and values are the same values
    // thus adding both together create a * 2 effect
    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 2, sum);
}

TEST_F(Ops, Filter) {
    int sum = 0;

    iter(int_vec)
        | filter([](const auto value) {
            return value % 2 == 1;
        })
        | for_each([&sum](const auto value) {
            sum += value;
        });

    EXPECT_EQ(9, sum);
}

TEST_F(Ops, FilterMap) {
    double sum = 0;

    iter(int_vec)
        | filter_map([](const auto value) {
            return value % 2 == 1
                ? Some(value + 0.5)
                : None;
        })
        | for_each([&sum](const auto value) {
            sum += value;
        });

    EXPECT_EQ(10.5, sum);
}

TEST_F(Ops, FindSome) {
    static constexpr auto FIND_SOME_ACC = 5;

    const auto find_some_opt = iter(int_vec)
        | find([](const auto value) { return value == FIND_SOME_ACC; });

    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(FIND_SOME_ACC, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindNone) {
    const auto find_none_opt = iter(int_vec)
        | find([](const auto value) { return value == 6; });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, FindMapSome) {
    const auto find_some_opt = iter(int_vec)
        | find_map([](const auto value) {
            return value == 4
                ? Some(value + 0.5)
                : None;
        });
    
    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(4.5, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindMapNone) {
    static constexpr auto FIND_MAP_NONE_VAL = -1;

    const auto find_none_opt = iter(int_vec)
        | find_map([](const int value) {
            return value == FIND_MAP_NONE_VAL
                ? Some(value)
                : None;
        });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, Fold) {
    static constexpr int FOLD_ACC = 10;

    const auto fold_sum = iter(int_vec)
        | fold(FOLD_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), FOLD_ACC), fold_sum);
}

TEST_F(Ops, ForEach) {
    int sum = 0;

    iter(int_vec)
        | for_each([&sum](const auto value) {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

TEST_F(Ops, Map) {
    double sum = 0.0;

    iter(int_vec)
        | map([](const auto value) {
            return value * 0.5;
        })
        | for_each([&sum](const auto value) {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 0.5, sum);
}

TEST_F(Ops, Range) {
    static constexpr int RANGE_ACC = 5;

    const auto sum = range(0, 6)
        | fold(RANGE_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), RANGE_ACC), sum);
}

TEST_F(Ops, SkipWithin) {
    const auto sum = iter(int_vec)
        | skip(3)
        | fold(0, plus<int>());

    EXPECT_EQ(12, sum);
}

TEST_F(Ops, SkipPass) {
    const auto sum = iter(int_vec)
        | skip(100)
        | fold(0, plus<int>());

    EXPECT_EQ(0, sum);
}

TEST_F(Ops, TakeWithin) {
    const auto sum = iter(int_vec)
        | take(3)
        | fold(0, plus<int>());

    EXPECT_EQ(3, sum);
}

TEST_F(Ops, TakeExceed) {
    const auto sum = iter(int_vec)
        | take(100)
        | fold(0, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
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
        | map([](const auto &r_ptr) { return cref(*r_ptr); })
        | filter([](const auto &r) { return r % 2 == 1; })
        | collect<vector<reference_wrapper<const int>>>();

    // no copy of any value was ever done from the original v

    EXPECT_EQ(2, v4.size());
    EXPECT_EQ(1, v4.at(0));
    EXPECT_EQ(3, v4.at(1));
}

TEST_F(ComplexOps, FilterMapFold) {
    const auto eleven_div_str = range(1, 100)
        | filter([](const auto value) { return value % 11 == 0; })
        | map([](const auto value) { return to_string(value); })
        | fold(string{}, [](const auto acc, const auto &rhs) {
            return acc + rhs + " ";
        });

    EXPECT_EQ("11 22 33 44 55 66 77 88 99 ", eleven_div_str);
}

TEST_F(ComplexOps, FilterMapFind) {
    // .5 is a easily representable value in mantissa
    // so that the floating float comparison can directly compare the values
    static constexpr auto FILTER_MAP_FIND_VAL = 34.5;

    const auto find_opt = range(1, 100)
        | filter([](const auto value) { return value % 17 == 0; })
        | map([](const auto value) { return value + 0.5; })
        | find([](const auto value) {
            return value == FILTER_MAP_FIND_VAL;
        });

    EXPECT_TRUE(find_opt.is_some());
    EXPECT_EQ(FILTER_MAP_FIND_VAL, find_opt.get_unchecked());
}

TEST_F(ComplexOps, ZipRefMapFold) {
    const auto zipped_int_vec = iter(int_vec)
        | zip(iter(int_vec) | skip(1))
        | collect<vector<pair<const int &, const int &>>>();

    const auto folded_str = iter(zipped_int_vec)
        | map([](const auto &index_value_pair) {
            stringstream ss;
            ss << '(' << index_value_pair.first << ',' << index_value_pair.second << ')';
            return ss.str();
        })
        | fold(string{}, [](const auto acc, const auto value) {
            stringstream ss;
            ss << acc << value << ' ';
            return ss.str();
        });

    EXPECT_EQ("(0,1) (1,2) (2,3) (3,4) (4,5) ", folded_str);
}

// option

TEST(Option, Assignment) {
    auto opt = Some(1);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(1, opt.get_unchecked());

    opt = None;
    EXPECT_TRUE(opt.is_none());

    opt = Some(7);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorNone) {
    Option<int> opt(None);
    EXPECT_TRUE(opt.is_none());
}

TEST(Option, CtorLRef) {
    auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_some());

    EXPECT_EQ(7, opt.get_unchecked());
    EXPECT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorConstLRef) {
    const auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_some());

    EXPECT_EQ(7, opt.get_unchecked());
    EXPECT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorMoveSimple) {
    Option<int> opt_rhs(Some(7));
    const Option<int> opt(move(opt_rhs));

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_none());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorMoveComplex) {
    auto opt_rhs = Some(make_unique<int>(7));
    const Option<unique_ptr<int>> opt(move(opt_rhs));

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_none());
    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MapSome) {
    const auto opt = Some(make_unique<int>(777))
        .map([](auto &&value) {
            return make_unique<string>(to_string(*value));
        })
        .map([](auto &&value) {
            return "*" + *value + "*";
        });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("*777*", opt.get_unchecked());
}

TEST(Option, MapNone) {
    auto opt = Option<int>(None);

    move(opt).map([](auto &&value) {
        return "Hello";
    });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, AndThenSomeToSome) {
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value) {
            return Some(make_unique<string>(to_string(*value)));
        });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("777", *opt.get_unchecked());
}

TEST(Option, AndThenSomeToNone) {
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value) -> Option<string> {
            return None;
        });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, AndThenNone) {
    const auto opt = Option<unique_ptr<int>>(None)
        .and_then([](auto &&value) {
            return Some(make_unique<string>(to_string(*value)));
        });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToNone) {
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return None; });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToSome) {
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return Some(make_unique<int>(7)); });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, OkOrElseSome) {
    const auto res = Some(7)
        .ok_or_else([] { return string("Hello"); });

    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(7, res.get_unchecked());
}

TEST(Option, OkOrElseNone) {
    const auto res = Option<int>(None)
        .ok_or_else([] { return string("Hello"); });

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", res.get_err_unchecked());
}

TEST(Option, OrElseSome) {
    const auto opt = Some(7)
        .or_else([] { return None; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, MatchXValueSome) {
    const auto value = Some(7).match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_EQ("7", value);
}

TEST(Option, MatchMoveSome) {
    auto opt = Some(make_unique<int>(8));

    const auto value = move(opt).match(
        [](const auto &value) { return to_string(*value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_none());
    EXPECT_EQ("8", value);
}

TEST(Option, MatchXValueNone) {
    const auto value = Option<int>(None).match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_EQ("NONE", value);
}

TEST(Option, MatchRefSome) {
    const auto opt = Some(7);

    const auto value = opt.match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("7", value);
}

TEST(Option, MatchRefNone) {
    const auto opt = Option<int>(None);
    
    const auto value = opt.match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_none());
    EXPECT_EQ("NONE", value);
}

TEST(Option, MatchSomeXValueSomeAssignBack) {
    auto ptr = make_unique<int>(7);

    Some(move(ptr)).match_some([&ptr](auto &&value) {
        // assign back so that there is something to test
        ptr = move(value);
    });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(7, *ptr);
}

TEST(Option, MatchSomeXValueSomeNoEffect) {
    auto ptr = make_unique<int>(7);

    Some(move(ptr)).match_some([&ptr](auto &&value) {
        // do nothing
    });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchSomeXValueNone) {
    auto ptr = make_unique<int>(7);

    Option<int>(None).match_some([&ptr](auto &&value) {
        // not happening
        ptr = nullptr;
    });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(7, *ptr);
}

TEST(Option, MatchSomeRefDoSomething) {
    const auto opt = Some(make_unique<int>(7));
    bool flag = false;

    opt.match_some([&flag](auto &&value) {
        flag = true;
    });

    EXPECT_TRUE(flag);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MatchSomeRefNoEffect) {
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_some([&flag](auto &&value) {
        flag = true;
    });

    EXPECT_FALSE(flag);
    EXPECT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneXValueNoneDoSomething) {
    unique_ptr<int> ptr;

    Option<int>(None).match_none([&ptr] {
        // assign back so that there is something to test
        ptr = make_unique<int>(8);
    });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(8, *ptr);
}

TEST(Option, MatchNoneXValueSomeNoEffect) {
    unique_ptr<int> ptr;

    Some(777).match_none([&ptr] {
        // assign back so that there is something to test
        ptr = make_unique<int>(777);
    });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchNoneRefDoSomething) {
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_none([&flag] {
        flag = true;
    });

    EXPECT_TRUE(flag);
    EXPECT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneRefNoEffect) {
    const auto opt = Some(7);
    bool flag = false;

    opt.match_none([&flag] {
        flag = true;
    });

    EXPECT_FALSE(flag);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, GetUnchecked) {
    const auto opt = Some(make_unique<string>("Hello"));

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("Hello", *opt.get_unchecked());
}

TEST(Option, UnwrapUnchecked) {
    string x = "Hello";
    auto opt = Some(ref(x));

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("Hello", move(opt).unwrap_unchecked());
}

TEST(Option, OptIfTrue) {
    const auto opt = opt_if(true,
        [] { return 7; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

// result

TEST(Result, ResIfElseTrue) {
    const auto res = res_if_else(true,
        [] { return 1; },
        [] { return 3.14; });

    EXPECT_TRUE(res.is_ok());
}

TEST(Result, ResIfElseFalse) {
    const auto res = res_if_else(false,
        [] { return 1; },
        [] { return 3.14; });

    EXPECT_TRUE(res.is_err());
}

TEST(Result, OkSome) {
    Result<int, string> res = Ok(8);
    const auto opt = move(res).ok();

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(8, opt.get_unchecked());
}

TEST(Result, OkNone) {
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).ok();

    EXPECT_TRUE(opt.is_none());
}

TEST(Result, ErrSome) {
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).err();

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("World", opt.get_unchecked());
}

TEST(Result, ErrNone) {
    Result<int, string> res = Ok(9);
    const auto opt = move(res).err();

    EXPECT_TRUE(opt.is_none());
}

TEST(Result, MapOk) {
    Result<int, string> res = Ok(1);
    const auto mapped_res = move(res).map([](const auto value) { return value + 0.5; });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(1.5, mapped_res.get_unchecked());
}

TEST(Result, MapErr) {
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map([](const auto value) { return to_string(value); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrErr) {
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map_err([](const auto value) { return value + " value"; });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error value", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrOk) {
    Result<int, int> res = Ok(-1);
    const auto mapped_res = move(res).map_err([](const auto value) { return to_string(value); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(-1, mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToOk) {
    Result<int, string> res = Ok(1);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<string, string> { return Ok(to_string(value)); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ("1", mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToErr) {
    Result<int, string> res = Ok(2);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> { return Err(to_string(value)); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("2", mapped_res.get_err_unchecked());
}

TEST(Result, AndThenErr) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> { return Ok(value); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseErrToOk) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, int> { return Ok(7); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(7, mapped_res.get_unchecked());
}

TEST(Result, OrElseErrToErr) {
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, string> { return Err(string{"Still error"}); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Still error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseOk) {
    Result<int, string> res = Ok(5);

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, double> { return Ok(6); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(5, mapped_res.get_unchecked());
}

TEST(Result, MatchOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));

    const auto match_res = move(res).match(
        [](unique_ptr<int> value) { return true; },
        [](unique_ptr<string> value) { return false; });

    EXPECT_TRUE(match_res);
}

TEST(Result, MatchErr) {
    Result<int, string> res = Err(string{"Hello"});

    const auto match_res = move(res).match(
        [](const auto value) { return 0.5; },
        [](const auto value) { return 0.25; });

    EXPECT_EQ(0.25, match_res);
}

TEST(Result, MatchOkRef) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(777));

    const auto match_res = res.match(
        [](const auto &value) { return to_string(*value); },
        [](const auto &value) { return *value; });

    EXPECT_EQ("777", match_res);
}

TEST(Result, MatchErrRef) {
    Result<string, string> res = Err(string{"World"});

    const auto match_res = res.match(
        [](const auto &value) { return cref(value); },
        [](const auto &value) { return cref(value); });

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ(&res.get_err_unchecked(), &match_res.get());
    EXPECT_EQ("World", match_res.get());
}

TEST(Result, MatchOkOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    unique_ptr<int> ptr;

    move(res).match_ok([&ptr](auto &&value) {
        ptr = move(value);
    });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(2, *ptr);
}

TEST(Result, MatchOkErr) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    unique_ptr<int> ptr;

    move(res).match_ok([&ptr](auto &&value) {
        ptr = move(value);
    });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Result, MatchOkRefOk) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    int value = 0;

    res.match_ok([&value](auto &&res_value) {
        value = *res_value;
    });

    EXPECT_EQ(2, value);
    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(2, *res.get_unchecked());
}

TEST(Result, MatchOkRefErr) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    int value = 7;

    res.match_ok([&value](auto &&res_value) {
        value = *res_value;
    });

    EXPECT_EQ(7, value);
    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", *res.get_err_unchecked());
}

TEST(Result, MatchErrOk) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    unique_ptr<string> ptr;

    move(res).match_err([&ptr](auto &&value) {
        ptr = move(value);
    });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Result, MatchErrErr) {
    Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    unique_ptr<string> ptr;

    move(res).match_err([&ptr](auto &&value) {
        ptr = move(value);
    });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ("Hello", *ptr);
}

TEST(Result, MatchErrRefOk) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));
    string value = "World";

    res.match_err([&value](auto &&res_value) {
        value = *res_value;
    });

    EXPECT_EQ("World", value);
    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(2, *res.get_unchecked());
}

TEST(Result, MatchErrRefErr) {
    const Result<unique_ptr<int>, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    string value = "World";

    res.match_err([&value](auto &&res_value) {
        value = *res_value;
    });

    EXPECT_EQ("Hello", value);
    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", *res.get_err_unchecked());
}

TEST(Result, GetUnchecked) {
    const Result<int, string> res = Ok(7);

    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(7, res.get_unchecked());
}

TEST(Result, GetErrUnchecked) {
    const Result<int, string> res = Err(string{"Hello"});

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", res.get_err_unchecked());
}

TEST(Result, UnwrapUnchecked) {
    int x = 7;
    Result<int &, int> res = Ok(ref(x));

    EXPECT_TRUE(res.is_ok());

    // to show that it returns reference
    static_assert(is_same<
        decltype(move(res).unwrap_unchecked()),
        int &>::value,
        "Result::UnwrapUnchecked does not return reference directly!");

    EXPECT_EQ(7, move(res).unwrap_unchecked());
}

TEST(Result, UnwrapErrUnchecked) {
    Result<string, unique_ptr<string>> res = Err(make_unique<string>("Hello"));
    EXPECT_TRUE(res.is_err());

    // to prove that the pointer can move
    auto err_ptr = move(res).unwrap_err_unchecked();
    EXPECT_EQ("Hello", *err_ptr);
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
        EXPECT_TRUE(res.is_ok());
        EXPECT_EQ(3, res.get_unchecked());
    }

    {
        const auto res = fn(false);
        EXPECT_TRUE(res.is_err());
        EXPECT_EQ("Hello", res.get_err_unchecked());
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
        EXPECT_TRUE(res.is_ok());
        EXPECT_EQ(13, res.get_unchecked());
    }

    {
        const auto res = fn(false);
        EXPECT_TRUE(res.is_err());
        EXPECT_EQ("Hello", res.get_err_unchecked());
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
        EXPECT_TRUE(res.is_ok());
        EXPECT_EQ(VAL, res.get_unchecked());
        EXPECT_EQ(&VAL, &res.get_unchecked());
    }

    {
        const auto res = fn(false);
        EXPECT_TRUE(res.is_err());
        EXPECT_EQ("Hello", res.get_err_unchecked());
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
        EXPECT_TRUE(res.is_ok());
        EXPECT_EQ(17, res.get_unchecked());
        EXPECT_EQ(&VAL, &res.get_unchecked());
    }

    {
        const auto res = fn(false);
        EXPECT_TRUE(res.is_err());
        EXPECT_EQ("Hello", res.get_err_unchecked());
    }
}

int main(int argc, char * argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
