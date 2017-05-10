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
#include "rustfp/map.h"
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
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
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
using rustfp::iter;
using rustfp::map;
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
using std::make_unique;
using std::mismatch;
using std::move;
using std::pair;
using std::plus;
using std::reference_wrapper;
using std::string;
using std::stringstream;
using std::to_string;
using std::unique_ptr;
using std::vector;

// simple tests

class Ops : public ::testing::Test
{
protected:
    void SetUp() override
    {
        int_vec = vector<int>{0, 1, 2, 3, 4, 5};
        str_vec = vector<string>{"Hello", "World", "How", "Are", "You"};
    }

    vector<int> int_vec;
    vector<string> str_vec;
};

class ComplexOps : public ::testing::Test
{
protected:
    void SetUp() override
    {
        int_vec = vector<int>{0, 1, 2, 3, 4, 5};
    }

    vector<int> int_vec;
};

TEST_F(Ops, AllTrue)
{
    const auto result = iter(int_vec)
        | all([](const auto value) { return value < 6; });

    EXPECT_TRUE(result);
}

TEST_F(Ops, AllFalse)
{
    const auto result = iter(int_vec)
        | all([](const auto value) { return value > 0; });

    EXPECT_FALSE(result);
}

TEST_F(Ops, AnyTrue)
{
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 5; });

    EXPECT_TRUE(result);
}

TEST_F(Ops, AnyFalse)
{
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 7; });

    EXPECT_FALSE(result);
}

TEST_F(Ops, ClonedRef)
{
    const auto str_dup_vec = iter(str_vec)
        | cloned()
        | collect<vector<string>>();

    const auto input_it_pairs = mismatch(
        cbegin(str_vec), cend(str_vec), cbegin(str_dup_vec),
        [](const auto &lhs, const auto &rhs)
        {
            // same value but different addresses
            return (lhs == rhs) && (&lhs != &rhs);
        });

    EXPECT_EQ(cend(str_vec), input_it_pairs.first);
    EXPECT_EQ(cend(str_dup_vec), input_it_pairs.second);
}

TEST_F(Ops, ClonedValue)
{
    const auto int_str_vec = iter(int_vec)
        | map([](const auto value) { return to_string(value); })
        | cloned() | cloned() | cloned()
        | collect<vector<string>>();

    const auto input_it_pairs = mismatch(
        cbegin(int_vec), cend(int_vec), cbegin(int_str_vec),
        [](const auto &lhs, const auto &rhs)
        {
            return to_string(lhs) == rhs;
        });

    EXPECT_EQ(cend(int_vec), input_it_pairs.first);
    EXPECT_EQ(cend(int_str_vec), input_it_pairs.second);
}

TEST_F(Ops, CollectVec)
{
    const auto dup_vec = range(0, int_vec.size())
        | collect<vector<int>>();

    const auto input_it_pairs = mismatch(
        cbegin(int_vec), cend(int_vec), cbegin(dup_vec));

    EXPECT_EQ(cend(int_vec), input_it_pairs.first);
    EXPECT_EQ(cend(dup_vec), input_it_pairs.second);
}

TEST_F(Ops, CollectVecRef)
{
    const auto str_ref_vec = iter(str_vec)
        | collect<vector<reference_wrapper<const string>>>();

    const auto input_it_pairs = mismatch(
        cbegin(str_vec), cend(str_vec), cbegin(str_ref_vec),
        [](const auto &lhs, const auto &rhs)
        {
            return &lhs == &rhs.get();
        });

    EXPECT_EQ(cend(str_vec), input_it_pairs.first);
    EXPECT_EQ(cend(str_ref_vec), input_it_pairs.second);
}

TEST_F(Ops, CollectMapVecSum)
{
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

TEST_F(Ops, Cycle)
{
    const int copyable_value = 7;

    const auto sum = cycle(cref(copyable_value))
        | take(1000)
        | fold(0, [](const auto acc, const auto value)
        {
            return acc + value.get();
        });

    EXPECT_EQ(7000, sum);
}

TEST_F(Ops, Enumerate)
{
    int sum = 0;

    iter(int_vec)
        | enumerate()
        | for_each([&sum](const auto pair)
        {
            // first and second are the same values
            sum += pair.first + pair.second;
        });

    // * 2 because the index and values are the same values
    // thus adding both together create a * 2 effect
    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 2, sum);
}

TEST_F(Ops, Filter)
{
    int sum = 0;

    iter(int_vec)
        | filter([](const auto value)
        {
            return value % 2 == 1;
        })

        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(9, sum);
}

TEST_F(Ops, FilterMap)
{
    double sum = 0;

    iter(int_vec)
        | filter_map([](const auto value)
        {
            return value % 2 == 1
                ? Some(value + 0.5)
                : None;
        })

        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(10.5, sum);
}

TEST_F(Ops, FindSome)
{
    static constexpr auto FIND_SOME_ACC = 5;

    const auto find_some_opt = iter(int_vec)
        | find([](const auto value) { return value == FIND_SOME_ACC; });

    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(FIND_SOME_ACC, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindNone)
{
    const auto find_none_opt = iter(int_vec)
        | find([](const auto value) { return value == 6; });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, FindMapSome)
{
    const auto find_some_opt = iter(int_vec)
        | find_map([](const auto value)
        {
            return value == 4
                ? Some(value + 0.5)
                : None;
        });
    
    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(4.5, find_some_opt.get_unchecked());
}

TEST_F(Ops, FindMapNone)
{
    static constexpr auto FIND_MAP_NONE_VAL = -1;

    const auto find_none_opt = iter(int_vec)
        | find_map([](const int value)
        {
            return value == FIND_MAP_NONE_VAL
                ? Some(value)
                : None;
        });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(Ops, Fold)
{
    static constexpr int FOLD_ACC = 10;

    const auto fold_sum = iter(int_vec)
        | fold(FOLD_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), FOLD_ACC), fold_sum);
}

TEST_F(Ops, ForEach)
{
    int sum = 0;

    iter(int_vec)
        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

TEST_F(Ops, Map)
{
    double sum = 0.0;

    iter(int_vec)
        | map([](const auto value)
        {
            return value * 0.5;
        })

        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0) * 0.5, sum);
}

TEST_F(Ops, Range)
{
    static constexpr int RANGE_ACC = 5;

    const auto sum = range(0, 6)
        | fold(RANGE_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), RANGE_ACC), sum);
}

TEST_F(Ops, SkipWithin)
{
    const auto sum = iter(int_vec)
        | skip(3)
        | fold(0, plus<int>());

    EXPECT_EQ(12, sum);
}

TEST_F(Ops, SkipPass)
{
    const auto sum = iter(int_vec)
        | skip(100)
        | fold(0, plus<int>());

    EXPECT_EQ(0, sum);
}

TEST_F(Ops, TakeWithin)
{
    const auto sum = iter(int_vec)
        | take(3)
        | fold(0, plus<int>());

    EXPECT_EQ(3, sum);
}

TEST_F(Ops, TakeExceed)
{
    const auto sum = iter(int_vec)
        | take(100)
        | fold(0, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

// complex tests

TEST_F(ComplexOps, FilterMapFold)
{
    const auto eleven_div_str = range(1, 100)
        | filter([](const auto value) { return value % 11 == 0; })
        | map([](const auto value) { return to_string(value); })
        | fold(string{}, [](const auto acc, const auto &rhs)
        {
            return acc + rhs + " ";
        });

    EXPECT_EQ("11 22 33 44 55 66 77 88 99 ", eleven_div_str);
}

TEST_F(ComplexOps, FilterMapFind)
{
    // .5 is a easily representable value in mantissa
    // so that the floating float comparison can directly compare the values
    static constexpr auto FILTER_MAP_FIND_VAL = 34.5;

    const auto find_opt = range(1, 100)
        | filter([](const auto value) { return value % 17 == 0; })
        | map([](const auto value) { return value + 0.5; })
        | find([](const auto value)
        {
            return value == FILTER_MAP_FIND_VAL;
        });

    EXPECT_TRUE(find_opt.is_some());
    EXPECT_EQ(FILTER_MAP_FIND_VAL, find_opt.get_unchecked());
}

TEST_F(ComplexOps, ZipRefMapFold)
{
    const auto zipped_int_vec = iter(int_vec)
        | zip(iter(int_vec) | skip(1))
        | collect<vector<pair<const int &, const int &>>>();

    const auto folded_str = iter(zipped_int_vec)
        | map([](const auto &index_value_pair)
        {
            stringstream ss;
            ss << '(' << index_value_pair.first << ',' << index_value_pair.second << ')';
            return ss.str();
        })

        | fold(string{}, [](const auto acc, const auto value)
        {
            stringstream ss;
            ss << acc << value << ' ';
            return ss.str();
        });

    EXPECT_EQ("(0,1) (1,2) (2,3) (3,4) (4,5) ", folded_str);
}

// option

TEST(Option, Assignment)
{
    auto opt = Some(1);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(1, opt.get_unchecked());

    opt = None;
    EXPECT_TRUE(opt.is_none());

    opt = Some(7);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorNone)
{
    Option<int> opt(None);

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, CtorLRef)
{
    auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_some());

    EXPECT_EQ(7, opt.get_unchecked());
    EXPECT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorConstLRef)
{
    const auto opt_rhs = Some(7);
    const auto opt(opt_rhs);

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_some());

    EXPECT_EQ(7, opt.get_unchecked());
    EXPECT_EQ(7, opt_rhs.get_unchecked());
}

TEST(Option, CtorMoveSimple)
{
    Option<int> opt_rhs(Some(7));
    const Option<int> opt(move(opt_rhs));

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_none());

    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, CtorMoveComplex)
{
    auto opt_rhs = Some(make_unique<int>(7));
    const Option<unique_ptr<int>> opt(move(opt_rhs));

    EXPECT_TRUE(opt.is_some());
    EXPECT_TRUE(opt_rhs.is_none());

    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MapSome)
{
    const auto opt = Some(make_unique<int>(777))
        .map([](auto &&value)
        {
            return make_unique<string>(to_string(*value));
        })
        .map([](auto &&value)
        {
            return "*" + *value + "*";
        });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("*777*", opt.get_unchecked());
}

TEST(Option, MapNone)
{
    auto opt = Option<int>(None);

    move(opt)
        .map([](auto &&value)
        {
            return "Hello";
        });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, AndThenSomeToSome)
{
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value)
        {
            return Some(make_unique<string>(to_string(*value)));
        });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("777", *opt.get_unchecked());
}

TEST(Option, AndThenSomeToNone)
{
    const auto opt = Some(make_unique<int>(777))
        .and_then([](auto &&value) -> Option<string>
        {
            return None;
        });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, AndThenNone)
{
    const auto opt = Option<unique_ptr<int>>(None)
        .and_then([](auto &&value)
        {
            return Some(make_unique<string>(to_string(*value)));
        });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToNone)
{
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return None; });

    EXPECT_TRUE(opt.is_none());
}

TEST(Option, OrElseNoneToSome)
{
    const auto opt = Option<unique_ptr<int>>(None)
        .or_else([] { return Some(make_unique<int>(7)); });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, OkOrElseSome)
{
    const auto res = Some(7)
        .ok_or_else([] { return string("Hello"); });

    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(7, res.get_unchecked());
}

TEST(Option, OkOrElseNone)
{
    const auto res = Option<int>(None)
        .ok_or_else([] { return string("Hello"); });

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", res.get_err_unchecked());
}

TEST(Option, OrElseSome)
{
    const auto opt = Some(7)
        .or_else([] { return None; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, MatchXValueSome)
{
    const auto value = Some(7).match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_EQ("7", value);
}

TEST(Option, MatchMoveSome)
{
    auto opt = Some(make_unique<int>(8));

    const auto value = move(opt).match(
        [](const auto &value) { return to_string(*value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_none());
    EXPECT_EQ("8", value);
}

TEST(Option, MatchXValueNone)
{
    const auto value = Option<int>(None).match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_EQ("NONE", value);
}

TEST(Option, MatchRefSome)
{
    const auto opt = Some(7);

    const auto value = opt.match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("7", value);
}

TEST(Option, MatchRefNone)
{
    const auto opt = Option<int>(None);
    
    const auto value = opt.match(
        [](const auto value) { return to_string(value); },
        [] { return "NONE"; });

    EXPECT_TRUE(opt.is_none());
    EXPECT_EQ("NONE", value);
}

TEST(Option, MatchSomeXValueSomeAssignBack)
{
    auto ptr = make_unique<int>(7);

    Some(move(ptr))
        .match_some([&ptr](auto &&value)
        {
            // assign back so that there is something to test
            ptr = move(value);
        });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(7, *ptr);
}

TEST(Option, MatchSomeXValueSomeNoEffect)
{
    auto ptr = make_unique<int>(7);

    Some(move(ptr))
        .match_some([&ptr](auto &&value)
        {
            // do nothing
        });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchSomeXValueNone)
{
    auto ptr = make_unique<int>(7);

    Option<int>(None)
        .match_some([&ptr](auto &&value)
        {
            // not happening
            ptr = nullptr;
        });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(7, *ptr);
}

TEST(Option, MatchSomeRefDoSomething)
{
    const auto opt = Some(make_unique<int>(7));
    bool flag = false;

    opt.match_some([&flag](auto &&value)
    {
        flag = true;
    });

    EXPECT_TRUE(flag);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, *opt.get_unchecked());
}

TEST(Option, MatchSomeRefNoEffect)
{
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_some([&flag](auto &&value)
    {
        flag = true;
    });

    EXPECT_FALSE(flag);
    EXPECT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneXValueNoneDoSomething)
{
    unique_ptr<int> ptr;

    Option<int>(None)
        .match_none([&ptr]
        {
            // assign back so that there is something to test
            ptr = make_unique<int>(8);
        });

    EXPECT_TRUE(static_cast<bool>(ptr));
    EXPECT_EQ(8, *ptr);
}

TEST(Option, MatchNoneXValueSomeNoEffect)
{
    unique_ptr<int> ptr;

    Some(777)
        .match_none([&ptr]
        {
            // assign back so that there is something to test
            ptr = make_unique<int>(777);
        });

    EXPECT_FALSE(static_cast<bool>(ptr));
}

TEST(Option, MatchNoneRefDoSomething)
{
    const auto opt = Option<int>(None);
    bool flag = false;

    opt.match_none([&flag]
    {
        flag = true;
    });

    EXPECT_TRUE(flag);
    EXPECT_TRUE(opt.is_none());
}

TEST(Option, MatchNoneRefNoEffect)
{
    const auto opt = Some(7);
    bool flag = false;

    opt.match_none([&flag]
    {
        flag = true;
    });

    EXPECT_FALSE(flag);
    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

TEST(Option, OptIfTrue)
{
    const auto opt = opt_if(true,
        [] { return 7; });

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(7, opt.get_unchecked());
}

// result

TEST(Result, ResIfElseTrue)
{
    const auto res = res_if_else(true,
        [] { return 1; },
        [] { return 3.14; });

    EXPECT_TRUE(res.is_ok());
}

TEST(Result, ResIfElseFalse)
{
    const auto res = res_if_else(false,
        [] { return 1; },
        [] { return 3.14; });

    EXPECT_TRUE(res.is_err());
}

TEST(Result, GetUnchecked)
{
    const Result<int, string> res = Ok(7);

    EXPECT_TRUE(res.is_ok());
    EXPECT_EQ(7, res.get_unchecked());
}

TEST(Result, GetErrUnchecked)
{
    const Result<int, string> res = Err(string{"Hello"});

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ("Hello", res.get_err_unchecked());
}

TEST(Result, OkSome)
{
    Result<int, string> res = Ok(8);
    const auto opt = move(res).ok();

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ(8, opt.get_unchecked());
}

TEST(Result, OkNone)
{
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).ok();

    EXPECT_TRUE(opt.is_none());
}

TEST(Result, ErrSome)
{
    Result<int, string> res = Err(string{"World"});
    const auto opt = move(res).err();

    EXPECT_TRUE(opt.is_some());
    EXPECT_EQ("World", opt.get_unchecked());
}

TEST(Result, ErrNone)
{
    Result<int, string> res = Ok(9);
    const auto opt = move(res).err();

    EXPECT_TRUE(opt.is_none());
}

TEST(Result, MapOk)
{
    Result<int, string> res = Ok(1);
    const auto mapped_res = move(res).map([](const auto value) { return value + 0.5; });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(1.5, mapped_res.get_unchecked());
}

TEST(Result, MapErr)
{
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map([](const auto value) { return to_string(value); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrErr)
{
    Result<int, string> res = Err(string{"Error"});
    const auto mapped_res = move(res).map_err([](const auto value) { return value + " value"; });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error value", mapped_res.get_err_unchecked());
}

TEST(Result, MapErrOk)
{
    Result<int, int> res = Ok(-1);
    const auto mapped_res = move(res).map_err([](const auto value) { return to_string(value); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(-1, mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToOk)
{
    Result<int, string> res = Ok(1);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<string, string> { return Ok(to_string(value)); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ("1", mapped_res.get_unchecked());
}

TEST(Result, AndThenOkToErr)
{
    Result<int, string> res = Ok(2);

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> { return Err(to_string(value)); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("2", mapped_res.get_err_unchecked());
}

TEST(Result, AndThenErr)
{
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .and_then([](const auto value) -> Result<int, string> { return Ok(value); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseErrToOk)
{
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, int> { return Ok(7); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(7, mapped_res.get_unchecked());
}

TEST(Result, OrElseErrToErr)
{
    Result<int, string> res = Err(string{"Error"});

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, string> { return Err(string{"Still error"}); });

    EXPECT_TRUE(mapped_res.is_err());
    EXPECT_EQ("Still error", mapped_res.get_err_unchecked());
}

TEST(Result, OrElseOk)
{
    Result<int, string> res = Ok(5);

    const auto mapped_res = move(res)
        .or_else([](const auto value) -> Result<int, double> { return Ok(6); });

    EXPECT_TRUE(mapped_res.is_ok());
    EXPECT_EQ(5, mapped_res.get_unchecked());
}

TEST(Result, MatchOk)
{
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(2));

    const auto match_res = move(res).match(
        [](unique_ptr<int> value) { return true; },
        [](unique_ptr<string> value) { return false; });

    EXPECT_TRUE(match_res);
}

TEST(Result, MatchErr)
{
    Result<int, string> res = Err(string{"Hello"});

    const auto match_res = move(res).match(
        [](const auto value) { return 0.5; },
        [](const auto value) { return 0.25; });

    EXPECT_EQ(0.25, match_res);
}

TEST(Result, MatchOkRef)
{
    Result<unique_ptr<int>, unique_ptr<string>> res = Ok(make_unique<int>(777));

    const auto match_res = res.match(
        [](const auto &value) { return to_string(*value); },
        [](const auto &value) { return *value; });

    EXPECT_EQ("777", match_res);
}

TEST(Result, MatchErrRef)
{
    Result<string, string> res = Err(string{"World"});

    const auto match_res = res.match(
        [](const auto &value) { return cref(value); },
        [](const auto &value) { return cref(value); });

    EXPECT_TRUE(res.is_err());
    EXPECT_EQ(&res.get_err_unchecked(), &match_res.get());
    EXPECT_EQ("World", match_res.get());
}

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
