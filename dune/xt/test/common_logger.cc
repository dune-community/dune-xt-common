// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2012, 2014 - 2015)
//   Rene Milk       (2012 - 2013, 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"

// dune-stuff
#include <dune/stuff/common/logging.hh>
#include <dune/stuff/common/logstreams.hh>

namespace DSC = Dune::Stuff::Common;

void balh(std::ostream& out)
{
  static int c = 0;
  out << "balh " << c << "\n";
  c++;
}

void do_something_that_takes_long(std::ostream& out)
{
  out << "  there should appear five dots, but not too fast:" << std::flush;
  for (size_t i = 0; i < 5; ++i) {
    busywait(666);
    out << "." << std::flush;
  }
  out << std::endl;
} // void do_something_that_takes_long()

TEST(LoggerTest, all)
{
  DSC::Logger().create(DSC::LOG_CONSOLE | DSC::LOG_ERROR);
  DSC::Logger().error() << "This should be in output\n";
  DSC::Logger().info() << "This should NOT be in output\n";
  DSC::Logger().debug() << "dito\n";
  DSC::Logger().flush();
  for (int i : {DSC::LOG_INFO, DSC::LOG_DEBUG, DSC::LOG_ERROR}) {
    const int id = DSC::Logger().addStream(DSC::LOG_CONSOLE | i);
    DSC::Logger().getStream(id) << "Create a new stream with id: " << id << std::endl;
  }
  DSC_LOG_ERROR.suspend();
  DSC_LOG_ERROR << "not in output\n";
  balh(DSC_LOG_ERROR);
  DSC_LOG_ERROR.resume();
  DSC_LOG_ERROR << "in output\n";
  balh(DSC_LOG_ERROR);

  // this should do nothing whatsoever
  balh(DSC::dev_null);
  DSC::Logger().flush();

  // this is the desired result:
  DSC::LogStream& err = DSC::Logger().error();
  std::cout << "begin std::cout test" << std::endl;
  do_something_that_takes_long(std::cout);
  std::cout << "end   std::cout test" << std::endl;
  std::cout << "begin Logger().error() test" << std::endl;
  do_something_that_takes_long(err);
  std::cout << "end   Logger().error() test" << std::endl;
}

TEST(LoggerTest, file)
{
  DSC::Logger().create(DSC::LOG_INFO | DSC::LOG_CONSOLE | DSC::LOG_FILE, "test_common_logger", "", "");
  DSC::Logger().info() << "This output should be in 'test_common_logger.log'" << std::endl;
}
