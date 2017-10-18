# `rustfp`
[![Build Status](https://travis-ci.org/guangie88/rustfp.svg?branch=master)](https://travis-ci.org/guangie88/rustfp)
[![Build status](https://ci.appveyor.com/api/projects/status/gx7vmcs5vlar60m6/branch/master?svg=true)](https://ci.appveyor.com/project/guangie88/rustfp/branch/master)
[![codecov](https://codecov.io/gh/guangie88/rustfp/branch/master/graph/badge.svg)](https://codecov.io/gh/guangie88/rustfp)

## Overview
C++14 implementation of Rust Option/Result (monads) and Iterator concepts, useful for expressing items and result manipulation in a more functional style.

- [https://doc.rust-lang.org/std/option/](https://doc.rust-lang.org/std/option/)
- [https://doc.rust-lang.org/std/result/](https://doc.rust-lang.org/std/result/)
- [https://doc.rust-lang.org/std/iter/](https://doc.rust-lang.org/std/iter/)

Currently tested to be compiling on `g++-5` and above, `MSVC2015` and above. `clang` should work as well, as long as the version supports C++14.

## Repository Checkout
For Git version 1.6.5 or later:
```bash
git clone --recursive https://github.com/guangie88/rustfp.git
```

For already cloned repository or older Git version:
```bash
git clone https://github.com/guangie88/rustfp.git
git submodule update --init --recursive
```

## Compilation and Installation
The solution uses CMake (at least version 3.6) for compilation and installation. To enable unit tests to be compiled for `rustfp`, add `-DRUSTFP_INCLUDE_UNIT_TESTS=ON` during the CMake configuration step.

### Windows Generator Names
- MSVC
  - 32-bit
    - `MSVC2015`: `Visual Studio 14`
    - `MSVC2017`: `Visual Studio 15`
  - 64-bit
    - `MSVC2015`: `Visual Studio 14 Win64`
    - `MSVC2017`: `Visual Studio 15 Win64`
    
### Windows Compilation

#### Debug and Release Compilation Example
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 14 Win64" -DCMAKE_INSTALL_PREFIX:PATH=install
cmake --build . --config debug
cmake --build . --config release --target install
```

### Linux Generator Names
For Linux, simply run `export CC=<path-to-desired-CC-compiler>; export CXX=<path-to-desired-CXX-compiler>` before running the CMake commands. As such, there should be no need to set `-G <generator-name>`.

#### Example
```bash
export CC=/usr/bin/gcc-5; export CXX=/usr/bin/g++-5
```

### Linux Compilation
Note that `gcc` requires the build type to be specified at the CMake configuration step.

#### Debug Compilation Example
```bash
mkdir build-debug
cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

#### Release Compilation Example
```bash
mkdir build-release
cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=install
cmake --build . --target install
```

### Installation Extraction
For the header files that are required to be included in other solutions, simply extract out the following directories in `build/install/include` into your other desired `include` directory:
- `nonstd`
- `mpark`
- `rustfp`

## Usage Examples
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
