#ifdef HAVE_CMAKE_CONFIG
#include "cmake_config.h"
#elif defined(HAVE_CONFIG_H)
#include <config.h>
#endif // ifdef HAVE_CMAKE_CONFIG

#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/math.hh>
#include <dune/common/tuples.hh>
#include <dune/common/exceptions.hh>
#include <dune/common/bigunsignedint.hh>
#include <limits>
#include <random>
#include <iostream>

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>

template <typename, bool>
struct Distribution
{
};
template <typename T>
struct Distribution<T, true>
{
  typedef std::uniform_int_distribution<T> type;
};
template <typename T>
struct Distribution<T, false>
{
  typedef std::uniform_real_distribution<T> type;
};

#define MY_ASSERT(cond)                                                                                                \
  if (not cond) {                                                                                                      \
    DUNE_THROW(Dune::Exception, #cond " failed");                                                                      \
  }

namespace DSC = Dune::Stuff::Common;
using namespace DSC::Parameter;

template <class T>
struct TypeTest
{
  void run() const
  {
    const int samples = 100000;
    std::cout << "Testing Validators for type " << DSC::Typename<T>::value() << "\n\t" << samples
              << " random numbers ..." << std::endl;
    {
      std::default_random_engine generator;
      const T lower = std::numeric_limits<T>::min();
      const T upper = std::numeric_limits<T>::max();
      typename Distribution<T, boost::is_integral<T>::value>::type distribution(lower, upper);
      for (int i = samples; i > 0; --i) {
        const T arg = distribution(generator);
        test(lower, upper, arg);
      }
    }
    std::cout << "\tfixed interval" << std::endl;
    {
      const T lower = T(0);
      const T upper = T(2);
      const T arg = T(1);
      test(lower, upper, arg);
    }
    std::cout << "\tdone." << std::endl;
  }

  void test(const T lower, const T upper, const T arg) const
  {
    const T eps         = DSC::Math::Epsilon<T>::value;
    const T clamped_arg = DSC::Math::clamp(arg, T(lower + eps), T(upper - eps));
    MY_ASSERT(ValidateAny<T>()(arg));
    MY_ASSERT(ValidateLess<T>(clamped_arg)(upper));
    MY_ASSERT(ValidateGreaterOrEqual<T>(arg)(lower));
    MY_ASSERT(ValidateGreater<T>(clamped_arg)(lower));
    MY_ASSERT(ValidateInterval<T>(lower, upper)(arg));
  }
};

int main(int, char* [])
{
  try {
    TypeTest<double>().run();
    TypeTest<float>().run();
    // is_intergral is false for Dune::bigunsignedint
    //    TypeTest<Dune::bigunsignedint<32> >().run();
    TypeTest<int>().run();
    TypeTest<unsigned int>().run();
    TypeTest<unsigned long>().run();
    TypeTest<long long>().run();
    TypeTest<char>().run();
    return 0;
  } catch (Dune::Exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
