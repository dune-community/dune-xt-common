#include "test_common.hh"

#include <dune/stuff/common/parameter/validation.hh>
#include <dune/stuff/common/parameter/configcontainer.hh>
#include <dune/stuff/common/random.hh>

#include <array>
#include <ostream>
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>

using namespace Dune::Stuff::Common;
using namespace Dune::Stuff::Common::Parameter;

typedef testing::Types<double, float, // Dune::bigunsignedint,
                       int, unsigned int, unsigned long, long long, char> TestTypes;

template <class T>
struct ConfigTest : public testing::Test
{
  Math::DefaultRNG<T> rng;
  Math::RandomStrings key_gen;
  // std::array is not assignable from list_of it seems
  const boost::array<T, 1000> values;
  const boost::array<std::string, 1000> keys;
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
  }

  void other()
  {
    DSC_CONFIG.printRequests(dev_null);
    DSC_CONFIG.printMismatchedDefaults(dev_null);
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
  return RUN_ALL_TESTS();
}
