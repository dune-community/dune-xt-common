```
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2016)
```

# Contributing

We very much encourage and welcome contributions!

# Coding style

Every project has its onw style, so here it is: we use clang-format to handle all whitespace related formatting and provide a corresponding `.clang-format` style specification file (more detailes on the workflow below).
This means that everything related to whitespace (brackets, comments, braces, macros, ...) is determined by clang-format, no flexibilty here.
I repeat: no flexibility at all regarding whitespace.
Apart from that we have some simple rules that you are asked to follow and any contributions in violation may be rejected.
In particular, these are:

* We use `CamelCase` for class and struct names, enum names, typedefs, and the like. We use `stl_standard` for class methods, members and variables. Does not seem to be consistent, but we never claimed we would be and it is highly readable. The only exception are typedefs and helper structs which arise in a context similar to the stl, where we use lowercase `typedef ... type;` and `struct is_vector {`.
* Class members end with a trailing underscore. Static class members may drop the trailing underscore (in particular dimensions, such as `static const size_t dim = 2;`).
* We append `Type` to in-class typedefs.
* Prefer `using typename BaseClass::FooType;` over `typedef typename BaseType::FooType FooType;`, reduces errors due to typos.
* Be careful to handle integers corretly, bad integer conversion is bad! Whenever possible, use `size_t` (even for `template<class Foo, size_t bar>`) and convert to the correct type using boost:
```c++
#include <boost/numeric/conversion/cast.hpp>

const auto unsigned_number = boost::numeric_cast<size_t>(some_size_of_unknown_int_type);
// ...
Dune::function_that_requires_an_int_because_ints_are_cool(boost::numeric_cast<int>(unsigned_number);
