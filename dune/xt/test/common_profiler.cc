// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// The copyright lies with the authors of this file (see below).
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2014 - 2016)
//   Rene Milk       (2012 - 2015)
//   Tobias Leibner  (2014)

#include "main.hxx"

#include <dune/xt/common/profiler.hh>
#include <dune/xt/common/math.hh>
#include <dune/xt/common/ranges.hh>

using namespace Dune::XT::Common;
const size_t wait_ms = 142;

void scoped_busywait(const std::string& name, size_t ms)
{
  ScopedTiming DUNE_UNUSED(scopedTiming)(name);
  busywait(ms);
}

static double confidence_margin()
{
  return 0.90f;
}

TEST(ProfilerTest, Timing)
{
  for (auto i : value_range(1, 4)) {
    DXTC_PROFILER.start_timing("ProfilerTest.Timing");
    busywait(wait_ms);
    DXTC_PROFILER.stop_timing("ProfilerTest.Timing");
    EXPECT_GE(DXTC_PROFILER.get_timing("ProfilerTest.Timing"), i * wait_ms * confidence_margin());
  }
}

TEST(ProfilerTest, ScopedTiming)
{
  const auto dvalue_range = value_range(1, 4);
  for (auto DUNE_UNUSED(i) : dvalue_range) {
    scoped_busywait("ProfilerTest.ScopedTiming", wait_ms);
  }
  EXPECT_GE(DXTC_PROFILER.get_timing("ProfilerTest.ScopedTiming"), long(dvalue_range.size() * wait_ms));
}

TEST(ProfilerTest, OutputConstness)
{
  DXTC_PROFILER.reset(1);
  const auto& prof = DXTC_PROFILER;
  prof.output_averaged(0, 0);
  prof.output_timings("timings");
  prof.output_timings(dev_null);
}

TEST(ProfilerTest, ExpectedFailures)
{
  EXPECT_THROW(DXTC_PROFILER.reset(0), Dune::RangeError);
  EXPECT_THROW(DXTC_PROFILER.stop_timing("This_section_was_never_started"), Dune::RangeError);
}

TEST(ProfilerTest, NestedTiming)
{
  auto& prof = DXTC_PROFILER;
  prof.reset(1);
  prof.start_timing("NestedTiming.Outer");
  busywait(100);
  prof.start_timing("NestedTiming.Inner");
  busywait(100);
  auto inner = prof.get_timing("NestedTiming.Inner");
  auto outer = prof.get_timing("NestedTiming.Outer");
  EXPECT_GT(outer, inner);
}
