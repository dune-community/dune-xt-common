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

TEST(ProfilerTest, Timing)
{
  for (auto i : valueRange(1, 4)) {
    DSC_PROFILER.startTiming("ProfilerTest.Timing");
    busywait(wait_ms);
    DSC_PROFILER.stopTiming("ProfilerTest.Timing");
    EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.Timing"), i * wait_ms);
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

TEST(ProfilerTest, MultiRuns)
{
  const auto dvalueRange = valueRange(1, 3);
  // needs to be dvalueRange.size() + 1 since we're calling nextRun() dvalueRange.size() times
  DSC_PROFILER.reset(dvalueRange.size() + 1);
  for (auto i : dvalueRange) {
    scoped_busywait("ProfilerTest.MultiRuns", i * wait_ms);
    DSC_PROFILER.nextRun();
  }
  for (auto i : dvalueRange) {
    // i-1 cause runs have 0-based index
    EXPECT_GE(DSC_PROFILER.getTimingIdx("ProfilerTest.MultiRuns", i - 1), i * wait_ms);
  }
}

TEST(ProfilerTest, OutputConstness)
{
  DSC_PROFILER.reset(1);
  const auto& prof = DSC_PROFILER;
  Profiler::InfoContainer infos;
  Profiler::InfoContainerMap info_map;
  prof.output(infos);
  prof.outputAveraged(0, 0);
  prof.outputTimings();
  prof.outputMap(info_map);
}

TEST(ProfilerTest, ExpectedFailures)
{
  EXPECT_THROW(DSC_PROFILER.reset(0), Dune::RangeError);
  EXPECT_THROW(DSC_PROFILER.reset(-1), Dune::RangeError);
  EXPECT_THROW(DSC_PROFILER.stopTiming("This_section_was_never_start"), Dune::RangeError);
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
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
