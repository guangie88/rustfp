# `rustfp`
[![Build Status](https://travis-ci.org/guangie88/rustfp.svg?branch=master)](https://travis-ci.org/guangie88/rustfp)
[![codecov](https://codecov.io/gh/guangie88/rustfp/branch/master/graph/badge.svg)](https://codecov.io/gh/guangie88/rustfp)

## Overview
C++14 implementation of Rust Option/Result and Iterator concepts, which encompasses many of the functional programming ideas.

## Examples
```c++
#include "rustfp/collect.h"
#include "rustfp/iter.h"
#include "rustfp/map.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    const std::vector<int> v_int{0, 1, 2};

    // rustfp::iter works differently from C++ iterator.
    // containers with proper begin/cbegin and end/cend
    // has to be wrapped by rustfp::iter before any of
    // the FP operations (e.g. map, filter, fold) can be
    // used.

    const auto v_str = rustfp::iter(v_int)
        | rustfp::map([](const auto &v) {
            // v is of const int & type
            return std::to_string(v);
        })
        | rustfp::collect<std::vector<std::string>>();

    assert(v_str[0] == "0");
    assert(v_str[1] == "1");
    assert(v_str[2] == "2");
}
```
