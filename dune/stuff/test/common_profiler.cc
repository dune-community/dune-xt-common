// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "test_common.hh"

#include <dune/stuff/common/profiler.hh>
#include <dune/stuff/common/math.hh>
#include <dune/stuff/common/ranges.hh>

using namespace Dune::Stuff::Common;
const int wait_ms = 142;

void scoped_busywait(const std::string& name, int ms)
{
  Profiler::ScopedTiming scopedTiming(name);
  busywait(ms);
}

static double confidence_margin()
{
  return 0.90f;
}

TEST(ProfilerTest, Timing)
{
  for (auto i : valueRange(1, 4)) {
    DSC_PROFILER.startTiming("ProfilerTest.Timing");
    busywait(wait_ms);
    DSC_PROFILER.stopTiming("ProfilerTest.Timing");
    EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.Timing"), i * wait_ms * confidence_margin());
  }
}

TEST(ProfilerTest, ScopedTiming)
{
  const auto dvalueRange = valueRange(1, 4);
  for (auto DUNE_UNUSED(i) : dvalueRange) {
    scoped_busywait("ProfilerTest.ScopedTiming", wait_ms);
  }
  EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.ScopedTiming"), long(dvalueRange.size() * wait_ms));
}

TEST(ProfilerTest, OutputConstness)
{
  DSC_PROFILER.reset(1);
  const auto& prof = DSC_PROFILER;
  prof.outputAveraged(0, 0);
  prof.outputTimings("timings");
  prof.outputTimings(DSC::dev_null);
}

TEST(ProfilerTest, ExpectedFailures)
{
  EXPECT_THROW(DSC_PROFILER.reset(0), Dune::RangeError);
  EXPECT_THROW(DSC_PROFILER.reset(-1), Dune::RangeError);
  EXPECT_THROW(DSC_PROFILER.stopTiming("This_section_was_never_started"), Dune::RangeError);
}

TEST(ProfilerTest, NestedTiming)
{
  auto& prof = DSC_PROFILER;
  prof.reset(1);
  prof.startTiming("NestedTiming.Outer");
  busywait(100);
  prof.startTiming("NestedTiming.Inner");
  busywait(100);
  auto inner = prof.getTiming("NestedTiming.Inner");
  auto outer = prof.getTiming("NestedTiming.Outer");
  EXPECT_GT(outer, inner);
}

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}
