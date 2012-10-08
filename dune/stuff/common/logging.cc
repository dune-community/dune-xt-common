#include <fstream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include <assert.h>

// dune-common
#include <dune/common/exceptions.hh>

#include "misc.hh"
#include "filesystem.hh"
#include "logstreams.hh"
#include "string.hh"

#include <boost/range/adaptors.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

Logging::Logging()
  : logflags_(LOG_NONE)
  , matlabLogStreamPtr(0)
  , emptyLogStream_(logflags_)
{
  streamIDs_.push_back(LOG_ERROR);
  streamIDs_.push_back(LOG_DEBUG);
  streamIDs_.push_back(LOG_INFO);
}

Logging::~Logging()
{

  // destroy in reverse creation order, 2012 style
  BOOST_REVERSE_FOREACH(auto id, streamIDs_)
  {
    // logstream dtor is mandated to flush itself
    delete streammap_[id];
    streammap_[id] = 0;
  }

  if ((logflags_ & LOG_FILE) != 0) {
    logfile_ << '\n' << stringFromTime() << ": LOG END" << std::endl;
    logfileWoTime_ << std::endl;
    logfile_.close();
    logfileWoTime_.close();
  }

  // delete the MatlabLogStream
  if (matlabLogStreamPtr) {
    matlabLogStreamPtr->flush();
    matlabLogFile_.close();
    delete matlabLogStreamPtr;
    matlabLogStreamPtr = 0;
  }
}

void Logging::create(int logflags, const std::string logfile, const std::string datadir, const std::string _logdir)
{
  using namespace boost::filesystem;
  logflags_   = logflags;
  path logdir = path(datadir) / _logdir;
  filename_ = logdir / (logfile + "_time.log");
  testCreateDirectory(filename_.string());
  filenameWoTime_ = logdir / (logfile + ".log");
  if ((logflags_ & LOG_FILE) != 0) {
    logfile_.open(filename_);
    assert(logfile_.is_open());
    logfileWoTime_.open(filenameWoTime_);
    assert(logfileWoTime_.is_open());
  }
  IdVecCIter it = streamIDs_.begin();
  for (; it != streamIDs_.end(); ++it) {
    flagmap_[*it]   = logflags;
    streammap_[*it] = new FileLogStream(*it, flagmap_[*it], logfile_, logfileWoTime_);
  }
  // create the MatlabLogStream
  path matlabLogFileName = logdir / (logfile + "_matlab.m");
  testCreateDirectory(matlabLogFileName.string());
  matlabLogFile_.open(matlabLogFileName);
  assert(matlabLogFile_.is_open());
  matlabLogStreamPtr = new MatlabLogStream(LOG_FILE, logflags_, matlabLogFile_);
} // Create

void Logging::setPrefix(std::string prefix)
{
  // / begin dtor
  IdVecCIter it = streamIDs_.end();

  for (; it != streamIDs_.begin(); --it) {
    delete streammap_[*it];
    streammap_[*it] = 0;
  }

  if ((logflags_ & LOG_FILE) != 0) {
    logfile_ << '\n' << stringFromTime() << ": LOG END" << std::endl;
    logfileWoTime_ << std::endl;
    logfile_.close();
    logfileWoTime_.close();
  }

  // delete the MatlabLogStream
  matlabLogStreamPtr->flush();
  matlabLogFile_.close();
  delete matlabLogStreamPtr;
  matlabLogStreamPtr = 0;
  // / end dtor

  create(logflags_, prefix);
} // SetPrefix

void Logging::setStreamFlags(int streamID, int flags)
{
  assert(flagmap_.find(streamID) != flagmap_.end());
  // this might result in logging to diff targtes, so we flush the current targets
  flush();
  flagmap_[streamID] = flags;
}

int Logging::getStreamFlags(int streamID) const
{
  const auto it = flagmap_.find(streamID);
  if (it == flagmap_.end())
    DUNE_THROW(InvalidStateException, "cannot get flags for unkown Stream id");
  return it->second;
}

LogStream& Logging::getStream(int streamId)
{
  const auto it = streammap_.find(streamId);
  if (it == streammap_.end())
    return emptyLogStream_;
  //      DUNE_THROW(InvalidStateException, "cannot get unkown Stream");
  else
    return *(it->second);
}

void Logging::flush()
{
  BOOST_FOREACH (auto pair, streammap_) {
    pair.second->flush();
  }
} // Flush

int Logging::addStream(int flags)
{
  static int streamID_int = LOG_NEXT;
  streamID_int <<= 1;
  int streamID = streamID_int;
  streamIDs_.push_back(streamID);
  flagmap_[streamID]   = (flags | streamID);
  streammap_[streamID] = new FileLogStream(streamID, flagmap_[streamID], logfile_, logfileWoTime_);
  return streamID_int;
} // AddStream

void Logging::resume(LogStream::PriorityType prio)
{
  BOOST_FOREACH (auto pair, streammap_) {
    pair.second->resume(prio);
  }
} // Resume

void Logging::suspend(LogStream::PriorityType prio)
{
  BOOST_FOREACH (auto pair, streammap_) {
    pair.second->suspend(prio);
  }
} // Suspend

} // namespace Common
} // namespace Stuff
} // namespace Dune
