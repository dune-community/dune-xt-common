// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_RANDOM_HH
#define DUNE_STUFF_RANDOM_HH

#include <random>
#include <limits>

#include <boost/assign/list_of.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

//! Helper class to abstract away selecting an integer or real valued distribution
template <typename T, bool = std::is_integral<T>::value>
struct UniformDistributionSelector
{
};

template <typename T>
struct UniformDistributionSelector<T, true>
{
  typedef std::uniform_int_distribution<T> type;
};
template <typename T>
struct UniformDistributionSelector<T, false>
{
  typedef std::uniform_real_distribution<T> type;
};

/** RandomNumberGenerator adapter
 * \template T type of generated numbers
 * \template EngineImp randomization algorithm/engine implementation
 * \template DistributionImp class with an EngineImp accepting call operator that returns the actual RNGs
 * \ref DefaultRNG for default choice of distribution, engine and init args
 **/
template <class T, class DistributionImp, class EngineImp = std::mt19937>
struct RNG
{
  typedef DistributionImp DistributionType;
  typedef EngineImp EngineType;
  EngineType generator;
  DistributionType distribution;
  RNG(EngineType g, DistributionType d)
    : generator(g)
    , distribution(d)
  {
  }

  inline T operator()()
  {
    return distribution(generator);
  }
};

template <class T, class DistributionImp, class EngineImp>
struct RNG<std::complex<T>, DistributionImp, EngineImp>
{
  typedef DistributionImp DistributionType;
  typedef EngineImp EngineType;
  EngineType generator;
  DistributionType distribution;
  RNG(EngineType g, DistributionType d)
    : generator(g)
    , distribution(d)
  {
  }

  inline std::complex<T> operator()()
  {
    return std::complex<T>(distribution(generator), distribution(generator));
  }
};

namespace {
const std::string alphanums("abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "1234567890");
const std::string other_printables("!@#$%^&*()"
                                   "`~-_=+[{]{\\|;:'\",<.>/? ");
}

//! RNG that represents strings of given length
class RandomStrings : public RNG<std::string, std::uniform_int_distribution<int>, std::mt19937>
{
  typedef RNG<std::string, std::uniform_int_distribution<int>, std::mt19937> BaseType;
  const size_t length;

public:
  RandomStrings(size_t l)
    : BaseType(std::mt19937(std::random_device()()),
               std::uniform_int_distribution<int>(0, boost::numeric_cast<int>(alphanums.size() - 1)))
    , length(l)
  {
  }

  inline std::string operator()()
  {
    std::string ret(length, '\0');
    std::generate(std::begin(ret), std::end(ret), [=]() { return alphanums[distribution(generator)]; });
    return ret;
  }
};

//! defaultrng with choice of uniform distribution and stl's default random engine based on T and its numeric_limits
template <class T>
class DefaultRNG : public RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine>
{
  typedef RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine> BaseType;

public:
  DefaultRNG(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max(),
             T seed = std::random_device()())
    : BaseType(std::default_random_engine(seed), typename UniformDistributionSelector<T>::type(min, max))
  {
  }
};

template <class T>
class DefaultRNG<std::complex<T>>
    : public RNG<std::complex<T>, typename UniformDistributionSelector<T>::type, std::default_random_engine>
{
  typedef RNG<std::complex<T>, typename UniformDistributionSelector<T>::type, std::default_random_engine> BaseType;

public:
  DefaultRNG(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max(),
             T seed = std::random_device()())
    : BaseType(std::default_random_engine(seed), typename UniformDistributionSelector<T>::type(min, max))
  {
  }
};

template <>
class DefaultRNG<std::string> : public RandomStrings
{
public:
  DefaultRNG(size_t ilength = 12)
    : RandomStrings(ilength)
  {
  }
};


} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_RANDOM_HH
