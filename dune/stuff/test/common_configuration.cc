// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

// This one has to come first (includes the config.h)!
#include "main.hxx"

#include <dune/stuff/common/validation.hh>
#include <dune/stuff/common/configuration.hh>
#include <dune/stuff/common/random.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/type_utils.hh>
#include <dune/stuff/common/float_cmp.hh>
#include <dune/stuff/la/container.hh>
#include <dune/stuff/common/matrix.hh>
#include <dune/stuff/common/tuple.hh>

#include <array>
#include <ostream>
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>


// uncomment this for output
// std::ostream& test_out = std::cout;
std::ostream& test_out = DSC_LOG.devnull();


using namespace Dune::Stuff::Common;
using Dune::Stuff::Exceptions::results_are_not_as_expected;
using namespace Dune::Stuff::Common::FloatCmp;

struct CreateByOperator
{
  static Configuration create()
  {
    Configuration config;
    config["string"]              = "string";
    config["sub1.int"]            = "1";
    config["sub2.size_t"]         = "1";
    config["sub2.subsub1.vector"] = "[0 1]";
    config["sub2.subsub1.matrix"] = "[0 1; 1 2]";
    return config;
  }
};

struct CreateByOperatorAndAssign
{
  static Configuration create()
  {
    Configuration config;
    config["string"]              = "string";
    config["sub1.int"]            = "1";
    config["sub2.size_t"]         = "1";
    config["sub2.subsub1.vector"] = "[0 1]";
    config["sub2.subsub1.matrix"] = "[0 1; 1 2]";
    Configuration config2;
    config2 = config;
    return config2;
  }
};

struct CreateByKeyAndValueAndAddConfiguration
{
  static Configuration create()
  {
    Configuration config("string", "string");
    config.set("sub1.int", "1");
    config.set("sub2.size_t", 1);
    config.add(Configuration("vector", "[0 1]"), "sub2.subsub1");
    config.add(Configuration("matrix", "[0 1; 1 2]"), "sub2.subsub1");
    return config;
  }
};

struct CreateByKeyAndValueAndAddParameterTree
{
  static Configuration create()
  {
    Configuration config("string", "string");
    config.set("sub1.int", "1");
    config.set("sub2.size_t", 1);
    Dune::ParameterTree paramtree;
    paramtree["vector"] = "[0 1]";
    paramtree["matrix"] = "[0 1; 1 2]";
    config.add(paramtree, "sub2.subsub1");
    return config;
  }
};

struct CreateByKeysAndValues
{
  static Configuration create()
  {
    return Configuration({"string", "sub1.int", "sub2.size_t", "sub2.subsub1.vector", "sub2.subsub1.matrix"},
                         {"string", "1", "1", "[0 1]", "[0 1; 1 2]"});
  }
};

struct CreateByParameterTree
{
  static Configuration create()
  {
    Dune::ParameterTree paramtree;
    paramtree["string"]              = "string";
    paramtree["sub1.int"]            = "1";
    paramtree["sub2.size_t"]         = "1";
    paramtree["sub2.subsub1.vector"] = "[0 1]";
    paramtree["sub2.subsub1.matrix"] = "[0 1; 1 2]";
    return Configuration(paramtree);
  }
};


typedef testing::Types<double, float, std::string, int, unsigned int, unsigned long, long long, char> TestTypes;

typedef testing::Types<CreateByOperator, CreateByKeyAndValueAndAddConfiguration, CreateByKeyAndValueAndAddParameterTree,
                       CreateByKeysAndValues, CreateByParameterTree, CreateByOperatorAndAssign> ConfigurationCreators;


template <class T>
struct ConfigTest : public testing::Test
{
  static const int count = 2;
  DefaultRNG<T> rng;
  RandomStrings key_gen;
  // std::array is not assignable from list_of it seems. Don't make the following two arrays const (triggers boost/intel
  // compiler bug, last tested with icpc version 14.0.3).
  boost::array<T, count> values;
  boost::array<std::string, count> keys;
  ConfigTest()
    : key_gen(8)
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
    const auto mismatches = DSC_CONFIG.get_mismatched_defaults_map();
    EXPECT_TRUE(mismatches.empty());
    if (!mismatches.empty()) {
      DSC_CONFIG.print_mismatched_defaults(std::cerr);
    }
    EXPECT_EQ(values.size(), uniq_keys.size());
  }

  void set()
  {
    for (T val : values) {
      auto key = key_gen();
      DSC_CONFIG.set(key, val);
      // get with default diff from expected
      auto re = DSC_CONFIG.get(key, T(val + Dune::Stuff::Common::Epsilon<T>::value));
      EXPECT_EQ(re, val);
    }
  }

  void other()
  {
    DSC_CONFIG.print_requests(dev_null);
    DSC_CONFIG.print_mismatched_defaults(dev_null);
    auto key = this->key_gen();
    DSC_CONFIG.set(key, T());
    EXPECT_THROW(DSC_CONFIG.get(key, T(), ValidateNone<T>()), Dune::Stuff::Exceptions::configuration_error);
  }

  void issue_42()
  {
    using namespace DSC;
    using namespace std;
    Configuration empty;
    Configuration to_add(vector<string>{"subsection.key"}, {0l});
    empty.add(to_add, "", true);
    EXPECT_TRUE(empty.has_sub("subsection"));
  }
}; // struct ConfigTest

struct StaticCheck
{
  typedef boost::mpl::vector<Int<1>, Int<2>> Ints;

  template <class MatrixType>
  static void check_matrix_static_size(const Configuration& config)
  {
    typedef DSC::MatrixAbstraction<MatrixType> MT;
    const auto r = MT::rows(MatrixType());
    const auto c = MT::cols(MatrixType());

    const auto check = [&r, &c](const MatrixType& mat) {
      for (size_t cc = 0; cc < c; ++cc) {
        for (size_t rr = 0; rr < r; ++rr) {
          EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, rr, cc), double(rr + cc)));
        }
      }
    };

    check(config.get("matrix", MatrixType(), r, c));
    check(config.get("matrix", MatrixType()));
    check(config.get<MatrixType>("matrix", r, c));
    check(config.get<MatrixType>("matrix"));
  }

  template <class U, class V>
  static void run(const DSC::Configuration& config)
  {
    const auto rows = U::value;
    const auto cols = V::value;
    check_matrix_static_size<Dune::Stuff::Common::FieldMatrix<double, rows, cols>>(config);
    check_matrix_static_size<Dune::FieldMatrix<double, rows, cols>>(config);
  }
};

template <class ConfigurationCreator>
struct ConfigurationTest : public ::testing::Test
{
  template <class VectorType>
  static void check_vector(const Configuration& config)
  {
    VectorType vec = config.get("vector", VectorType(), 1);
    EXPECT_EQ(1, vec.size());
    EXPECT_FLOAT_EQ(0.0, vec[0]);
    vec = config.get("vector", VectorType(), 2);
    EXPECT_EQ(2, vec.size());
    for (auto ii : {0.0, 1.0})
      EXPECT_FLOAT_EQ(ii, vec[ii]);
    vec = config.get<VectorType>("vector", 1);
    EXPECT_EQ(1, vec.size());
    EXPECT_FLOAT_EQ(0.0, vec[0]);
    vec = config.get<VectorType>("vector", 2);
    EXPECT_EQ(2, vec.size());
    for (auto ii : {0.0, 1.0})
      EXPECT_FLOAT_EQ(ii, vec[ii]);
    vec = config.get<VectorType>("vector");
    EXPECT_EQ(2, vec.size());
    for (auto ii : {0.0, 1.0})
      EXPECT_FLOAT_EQ(ii, vec[ii]);
  } // ... check_vector< ... >(...)

  template <class K, int d>
  static void check_field_vector(const Configuration& config)
  {
    typedef FieldVector<K, d> VectorType;
    VectorType vec = config.get("vector", VectorType(), d);
    EXPECT_EQ(d, vec.size());
    for (size_t ii = 0; ii < d; ++ii)
      EXPECT_TRUE(FloatCmp::eq(vec[ii], double(ii)));

    vec = config.get<VectorType>("vector", d);
    EXPECT_EQ(vec.size(), d);

    for (size_t ii = 0; ii < d; ++ii)
      EXPECT_TRUE(FloatCmp::eq(vec[ii], double(ii)));

    vec = config.get<VectorType>("vector");
    EXPECT_EQ(vec.size(), d);

    for (size_t ii = 0; ii < d; ++ii)
      EXPECT_TRUE(FloatCmp::eq(vec[ii], double(ii)));
  } // ... check_field_vector< ... >(...)

  template <class MatrixType>
  static void check_matrix(const Configuration& config)
  {
    MatrixType mat = config.get("matrix", MatrixType(), 1, 1);
    typedef DSC::MatrixAbstraction<MatrixType> MT;
    EXPECT_FALSE(MT::rows(mat) != 1 || MT::cols(mat) != 1);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    mat = config.get("matrix", MatrixType(), 1, 2);
    EXPECT_FALSE(MT::rows(mat) != 1 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    mat = config.get("matrix", MatrixType(), 2, 1);
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 1);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    mat = config.get("matrix", MatrixType(), 2, 2);
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 1), 2.0));
    mat = config.get("matrix", MatrixType());
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 1), 2.0));

    mat = config.get<MatrixType>("matrix", 1, 1);
    EXPECT_FALSE(MT::rows(mat) != 1 || MT::cols(mat) != 1);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    mat = config.get<MatrixType>("matrix", 1, 2);
    EXPECT_FALSE(MT::rows(mat) != 1 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    mat = config.get<MatrixType>("matrix", 2, 1);
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 1);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    mat = config.get<MatrixType>("matrix", 2, 2);
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 1), 2.0));
    mat = config.get<MatrixType>("matrix");
    EXPECT_FALSE(MT::rows(mat) != 2 || MT::cols(mat) != 2);
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 0), 0.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 0, 1), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 0), 1.0));
    EXPECT_TRUE(FloatCmp::eq(MT::get_entry(mat, 1, 1), 2.0));
  } // ... check_matrix< ... >(...)

  static void behaves_correctly()
  {
    const Configuration config = ConfigurationCreator::create();
    config.report(); // <- this works as well but will produce output
    config.report(test_out);
    config.report(test_out, "'prefix '");
    test_out << config << std::endl;
    std::string DUNE_UNUSED(report_str) = config.report_string();
    std::string str = config.get("string", std::string("foo"));
    EXPECT_EQ(str, "string");
    str = config.get("foo", std::string("string"));
    EXPECT_EQ(str, "string");
    str = config.get<std::string>("string");
    EXPECT_EQ(str, "string");

    EXPECT_TRUE(config.has_sub("sub1"));
    Configuration sub1_config = config.sub("sub1");
    int nt = sub1_config.get("int", int(0));
    EXPECT_EQ(nt, 1);
    nt = sub1_config.get("intt", int(1));
    EXPECT_EQ(nt, 1);
    nt = sub1_config.get<int>("int");
    EXPECT_EQ(nt, 1);
    size_t st = config.get("sub2.size_t", size_t(0));
    EXPECT_EQ(st, 1);
    st = config.get("sub2.size_tt", size_t(1));
    EXPECT_EQ(st, 1);
    st = config.get<size_t>("sub2.size_t");
    EXPECT_EQ(st, 1);

    const auto subsub1 = config.sub("sub2.subsub1");
    check_vector<std::vector<double>>(subsub1);
    check_field_vector<double, 1>(subsub1);
    check_field_vector<double, 2>(subsub1);
    check_vector<Dune::DynamicVector<double>>(subsub1);
    check_vector<Dune::Stuff::LA::CommonDenseVector<double>>(subsub1);
#if HAVE_DUNE_ISTL
    check_vector<Dune::Stuff::LA::IstlDenseVector<double>>(subsub1);
#endif
#if HAVE_EIGEN
    check_vector<Dune::Stuff::LA::EigenDenseVector<double>>(subsub1);
    check_vector<Dune::Stuff::LA::EigenMappedDenseVector<double>>(subsub1);
    check_matrix<Dune::Stuff::LA::EigenDenseMatrix<double>>(subsub1);
#endif // HAVE_EIGEN
    check_matrix<Dune::DynamicMatrix<double>>(subsub1);
    check_matrix<Dune::Stuff::LA::CommonDenseMatrix<double>>(subsub1);

    DSC::TupleProduct::Combine<StaticCheck::Ints, StaticCheck::Ints, StaticCheck>::Generate<>::Run(subsub1);

  } // ... behaves_correctly(...)
}; // struct ConfigurationTest


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
  this->issue_42();
}

TYPED_TEST_CASE(ConfigurationTest, ConfigurationCreators);
TYPED_TEST(ConfigurationTest, behaves_correctly)
{
  this->behaves_correctly();
}
