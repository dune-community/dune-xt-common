#include "test_common.hh"

#include <dune/stuff/common/profiler.hh>
#include <dune/stuff/common/math.hh>

using namespace Dune::Stuff::Common;
const int wait_ms = 142;

void scoped_busywait(const std::string& name, int ms)
{
  Profiler::ScopedTiming scopedTiming(name);
  busywait(ms);
}

TEST(ProfilerTest, Timing)
{
  for (auto i : Math::range(1, 4)) {
    DSC_PROFILER.startTiming("ProfilerTest.Timing");
    busywait(wait_ms);
    DSC_PROFILER.stopTiming("ProfilerTest.Timing");
    EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.Timing"), i * wait_ms);
  }
}

TEST(ProfilerTest, ScopedTiming)
{
  const auto range = Math::range(1, 4);
  for (auto DUNE_UNUSED(i) : range) {
    scoped_busywait("ProfilerTest.ScopedTiming", wait_ms);
  }
  EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.ScopedTiming"), long(range.size() * wait_ms));
}

TEST(ProfilerTest, MultiRuns)
{
  const auto range = Math::range(1, 3);
  DSC_PROFILER.reset(range.size());
  for (auto i : range) {
    scoped_busywait("ProfilerTest.MultiRuns", i * wait_ms);
    DSC_PROFILER.nextRun();
  }
  for (auto i : range) {
    EXPECT_GE(DSC_PROFILER.getTiming("ProfilerTest.MultiRuns", i - 1), i * wait_ms);
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

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
