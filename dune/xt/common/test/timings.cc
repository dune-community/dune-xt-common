// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: Dual licensed as  BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include <dune/xt/common/test/main.hxx>

#include <dune/xt/common/timings.hh>
#include <dune/xt/common/math.hh>
#include <dune/xt/common/ranges.hh>

using namespace Dune::XT::Common;
const size_t wait_ms = 142;

void scoped_busywait(const std::string& name, size_t ms)
{
  DUNE_UNUSED ScopedTiming scopedTiming(name);
  busywait(ms);
}

static double confidence_margin()
{
  return 0.90f;
}

TEST(ProfilerTest, Timing)
{
  for (auto i : value_range(1, 4)) {
    DXTC_TIMINGS.start("ProfilerTest.Timing");
    busywait(wait_ms);
    DXTC_TIMINGS.stop("ProfilerTest.Timing");
    const auto timing = DXTC_TIMINGS.walltime("ProfilerTest.Timing");
    EXPECT_GE(timing, i * wait_ms * confidence_margin());
    EXPECT_EQ(timing, DXTC_TIMINGS.delta("ProfilerTest.Timing")[0]);
  }
}

TEST(ProfilerTest, ScopedTiming)
{
  const auto dvalue_range = value_range(1, 4);
  for (auto i DUNE_UNUSED : dvalue_range) {
    scoped_busywait("ProfilerTest.ScopedTiming", wait_ms);
  }
  EXPECT_GE(DXTC_TIMINGS.walltime("ProfilerTest.ScopedTiming"), long(dvalue_range.size() * wait_ms));
}

TEST(ProfilerTest, OutputConstness)
{
  DXTC_TIMINGS.reset();
  const auto& prof = DXTC_TIMINGS;
  prof.output_per_rank("timings");
  prof.output_all_measures(dev_null);
  prof.output_simple(dev_null);
}

TEST(ProfilerTest, ExpectedFailures)
{
  EXPECT_THROW(DXTC_TIMINGS.stop("This_section_was_never_started"), Dune::RangeError);
}

TEST(ProfilerTest, NestedTiming)
{
  auto& prof = DXTC_TIMINGS;
  prof.reset();
  prof.start("NestedTiming.Outer");
  busywait(100);
  prof.start("NestedTiming.Inner");
  busywait(100);
  auto inner = prof.walltime("NestedTiming.Inner");
  auto outer = prof.walltime("NestedTiming.Outer");
  EXPECT_GT(outer, inner);
}

TEST(ProfilerTest, Example)
{
  timings().reset();
  using namespace Dune::XT::Common;
  timings().start("example");
  for (auto i : value_range(5)) {
    ScopedTiming scoped_timing("example.inner");
    busywait(i * 200);
  }
  timings().stop("example");
  busywait(1000);
  auto file = make_ofstream("example.csv");
  timings().output_all_measures(*file);
}
