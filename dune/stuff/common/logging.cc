#include <fstream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <map>
#include <assert.h>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/common/mpihelper.hh>

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
  , emptyLogStream_(logflags_)
  , created_(false)
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
    logfile_ << std::endl;
    logfile_.close();
  }
}

void Logging::create(int logflags, const std::string logfile, const std::string datadir, const std::string _logdir)
{
  using namespace boost::filesystem;
  const auto& comm = Dune::MPIHelper::getCollectiveCommunication();
  boost::format log_fn("%s%s");
  if (comm.size() > 1) {
    const std::string rank = (boost::format("%08d") % comm.rank()).str();
    log_fn                 = boost::format("%s_p" + rank + "_%s");
  }
  logflags_   = logflags;
  path logdir = path(datadir) / _logdir;
  filename_ = logdir / (log_fn % logfile % ".log").str();
  testCreateDirectory(filename_.string());
  if ((logflags_ & LOG_FILE) != 0) {
    logfile_.open(filename_);
    assert(logfile_.is_open());
  }
  IdVecCIter it = streamIDs_.begin();
  for (; it != streamIDs_.end(); ++it) {
    flagmap_[*it]   = logflags;
    streammap_[*it] = new FileLogStream(*it, flagmap_[*it], logfile_);
  }
  created_ = true;
} // Create

bool Logging::created() const
{
  return created_;
}


void Logging::setPrefix(std::string prefix)
{
  // / begin dtor
  IdVecCIter it = streamIDs_.end();

  for (; it != streamIDs_.begin(); --it) {
    delete streammap_[*it];
    streammap_[*it] = 0;
  }

  if ((logflags_ & LOG_FILE) != 0) {
    logfile_ << std::endl;
    logfile_.close();
  }

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
  else {
    assert(it->second);
    return *(it->second);
  }
}

void Logging::flush()
{
  BOOST_FOREACH (auto pair, streammap_) {
    assert(pair.second);
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
  streammap_[streamID] = new FileLogStream(streamID, flagmap_[streamID], logfile_);
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
