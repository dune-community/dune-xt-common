#include "test_common.hh"

#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>
#include <dune/stuff/common/random.hh>
#include <dune/stuff/common/math.hh>

#include <array>
#include <ostream>
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>

using namespace Dune::Stuff::Common;

typedef testing::Types<double, float, std::string, int, unsigned int, unsigned long, long long, char> TestTypes;


template <class T>
struct ConfigTest : public testing::Test
{
  static const int count = 2;
  DefaultRNG<T> rng;
  RandomStrings key_gen;
  // std::array is not assignable from list_of it seems
  const boost::array<T, count> values;
  const boost::array<std::string, count> keys;
  ConfigTest()
    : key_gen(512)
    , values(boost::assign::list_of<T>().repeat_fun(values.size() - 1, rng))
    , keys(boost::assign::list_of<std::string>().repeat_fun(values.size() - 1, key_gen))
  {
  }

  void get()
  {
    std::set<std::string> uniq_keys;
    for (T val : values) {
      auto key = key_gen();
      EXPECT_EQ(val, DSC_CONFIG_GET(key, val));
      uniq_keys.insert(key);
    }
    EXPECT_EQ(0u, DSC_CONFIG.getMismatchedDefaultsMap().size());
    EXPECT_EQ(values.size(), uniq_keys.size());
  }

  void set()
  {
    for (T val : values) {
      auto key = key_gen();
      DSC_CONFIG.set(key, val);
      // get with default diff from expected
      auto re = DSC_CONFIG.get(key, val + Epsilon<T>::value);
      EXPECT_EQ(re, val);
    }
  }

  void other()
  {
    DSC_CONFIG.printRequests(dev_null);
    DSC_CONFIG.printMismatchedDefaults(dev_null);
    auto key = this->key_gen();
    DSC_CONFIG.set(key, 0);
    EXPECT_THROW(DSC_CONFIG.get(key, T(0), ValidateNone<T>()), DSC::InvalidParameter);
  }
};

TYPED_TEST_CASE(ConfigTest, TestTypes);
TYPED_TEST(ConfigTest, Get)
{
  this->get();
}
TYPED_TEST(ConfigTest, Set)
{
  this->set();
}
TYPED_TEST(ConfigTest, Other)
{
  this->other();
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);
  return RUN_ALL_TESTS();
}
