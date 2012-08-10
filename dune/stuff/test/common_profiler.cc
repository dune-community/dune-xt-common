#include "test_common.hh"

#include <dune/stuff/common/profiler.hh>
#include <dune/stuff/common/math.hh>

using namespace Dune::Stuff::Common;
const int wait_ms = 142;

TEST(ProfilerTest, Timing)
{
  for (auto i : Math::range(1, 4)) {
    DSC_PROF.startTiming("ProfilerTest.Timing");
    busywait(wait_ms);
    DSC_PROF.stopTiming("ProfilerTest.Timing");
    EXPECT_GE(DSC_PROF.getTiming("ProfilerTest.Timing"), i * wait_ms);
  }
}

TEST(ProfilerTest, ScopedTiming)
{
  const auto range = Math::range(1, 4);
  for (auto i : range) {
    Profiler::ScopedTiming scopedTiming("ProfilerTest.ScopedTiming");
    busywait(wait_ms);
  }
  EXPECT_GE(DSC_PROF.getTiming("ProfilerTest.ScopedTiming"), range.size() * wait_ms);
}

TEST(ProfilerTest, MultiRuns)
{
  const auto range = Math::range(1, 3);
  DSC_PROF.reset(range.size());
  for (auto i : range) {
    {
      Profiler::ScopedTiming scopedTiming("ProfilerTest.MultiRuns");
      busywait(i * wait_ms);
    }
    DSC_PROF.nextRun();
  }
  for (auto i : range) {
    EXPECT_GE(DSC_PROF.getTiming("ProfilerTest.MultiRuns", i - 1), i * wait_ms);
  }
}


int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  Dune::MPIHelper::instance(argc, argv);
  return RUN_ALL_TESTS();
}
