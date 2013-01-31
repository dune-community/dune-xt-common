#ifndef DUNE_STUFF_RANDOM_HH
#define DUNE_STUFF_RANDOM_HH

#include <random>
#include <boost/assign/list_of.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

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

template <class T, class DistributionImp, class EngineImp>
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

  T operator()()
  {
    return distribution(generator);
  }
};


namespace {
const std::string alphanums("abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "1234567890");
const std::string other_printables("!@#$%^&*()"
                                   "`~-_=+[{]{\\|;:'\",<.>/? ");
}

class RandomStrings : public RNG<std::string, std::uniform_int_distribution<int>, std::mt19937>
{
  typedef RNG<std::string, std::uniform_int_distribution<int>, std::mt19937> BaseType;
  const size_t length;

public:
  RandomStrings(size_t l)
    : BaseType(std::mt19937(std::random_device()()), std::uniform_int_distribution<int>(0, alphanums.size() - 1))
    , length(l)
  {
  }

  std::string operator()()
  {
    std::string ret(length, '\0');
    std::generate(std::begin(ret), std::end(ret), [=]() { return alphanums[distribution(generator)]; });
    return ret;
  }
};

template <class T>
class DefaultRNG : public RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine>
{
  typedef RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine> BaseType;

public:
  DefaultRNG(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
    : BaseType(std::default_random_engine(std::random_device()()),
               typename UniformDistributionSelector<T>::type(min, max))
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

/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
