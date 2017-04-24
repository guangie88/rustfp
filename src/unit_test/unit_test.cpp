#include "rustfp/all.h"
#include "rustfp/any.h"
#include "rustfp/cloned.h"
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
#include "rustfp/skip.h"
#include "rustfp/take.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

// rustfp
using rustfp::all;
using rustfp::any;
using rustfp::cloned;
using rustfp::collect;
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
using rustfp::None;
using rustfp::Some;

// std
using std::accumulate;
using std::cbegin;
using std::cend;
using std::cout;
using std::mismatch;
using std::plus;
using std::reference_wrapper;
using std::string;
using std::to_string;
using std::vector;

// simple tests

class SimpleTest : public ::testing::Test
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

class ComplexTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};

TEST_F(SimpleTest, AllTrue)
{
    const auto result = iter(int_vec)
        | all([](const auto value) { return value < 6; });

    EXPECT_TRUE(result);
}

TEST_F(SimpleTest, AllFalse)
{
    const auto result = iter(int_vec)
        | all([](const auto value) { return value > 0; });

    EXPECT_FALSE(result);
}

TEST_F(SimpleTest, AnyTrue)
{
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 5; });

    EXPECT_TRUE(result);
}

TEST_F(SimpleTest, AnyFalse)
{
    const auto result = iter(int_vec)
        | any([](const auto value) { return value == 7; });

    EXPECT_FALSE(result);
}

TEST_F(SimpleTest, ClonedRef)
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

TEST_F(SimpleTest, ClonedValue)
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

TEST_F(SimpleTest, CollectVec)
{
    const auto dup_vec = range(0, int_vec.size())
        | collect<vector<int>>();

    const auto input_it_pairs = mismatch(
        cbegin(int_vec), cend(int_vec), cbegin(dup_vec));

    EXPECT_EQ(cend(int_vec), input_it_pairs.first);
    EXPECT_EQ(cend(dup_vec), input_it_pairs.second);
}

TEST_F(SimpleTest, CollectVecRef)
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

TEST_F(SimpleTest, CollectMapVecSum)
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

TEST_F(SimpleTest, Enumerate)
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

TEST_F(SimpleTest, Filter)
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

TEST_F(SimpleTest, FilterMap)
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

TEST_F(SimpleTest, FindSome)
{
    static constexpr auto FIND_SOME_ACC = 5;

    const auto find_some_opt = iter(int_vec)
        | find([](const auto value) { return value == FIND_SOME_ACC; });

    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(FIND_SOME_ACC, find_some_opt.get_unchecked());
}

TEST_F(SimpleTest, FindNone)
{
    const auto find_none_opt = iter(int_vec)
        | find([](const auto value) { return value == 6; });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(SimpleTest, FindMapSome)
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

TEST_F(SimpleTest, FindMapNone)
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

TEST_F(SimpleTest, Fold)
{
    static constexpr int FOLD_ACC = 10;

    const auto fold_sum = iter(int_vec)
        | fold(FOLD_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), FOLD_ACC), fold_sum);
}

TEST_F(SimpleTest, ForEach)
{
    int sum = 0;

    iter(int_vec)
        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

TEST_F(SimpleTest, Map)
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

TEST_F(SimpleTest, Range)
{
    static constexpr int RANGE_ACC = 5;

    const auto sum = range(0, 6)
        | fold(RANGE_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), RANGE_ACC), sum);
}

TEST_F(SimpleTest, SkipWithin)
{
    const auto sum = iter(int_vec)
        | skip(3)
        | fold(0, plus<int>());

    EXPECT_EQ(12, sum);
}

TEST_F(SimpleTest, SkipPass)
{
    const auto sum = iter(int_vec)
        | skip(100)
        | fold(0, plus<int>());

    EXPECT_EQ(0, sum);
}

TEST_F(SimpleTest, TakeWithin)
{
    const auto sum = iter(int_vec)
        | take(3)
        | fold(0, plus<int>());

    EXPECT_EQ(3, sum);
}

TEST_F(SimpleTest, TakeExceed)
{
    const auto sum = iter(int_vec)
        | take(100)
        | fold(0, plus<int>());

    EXPECT_EQ(accumulate(cbegin(int_vec), cend(int_vec), 0), sum);
}

// complex tests

TEST_F(ComplexTest, FilterMapFold)
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

TEST_F(ComplexTest, FilterMapFind)
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

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
