#include "logging.hh"

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

#include <dune/stuff/common/memory.hh>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

Logging::Logging()
  : streamIDs_({LOG_ERROR, LOG_DEBUG, LOG_INFO})
  , logflags_(LOG_NONE)
  , emptyLogStream_(logflags_)
{
  for (const auto id : streamIDs_)
    streammap_[id] = make_unique<EmptyLogStream>(logflags_);
}

void Logging::deinit()
{
  streammap_.clear();
  if ((logflags_ & LOG_FILE) != 0) {
    logfile_ << std::endl;
    logfile_.close();
  }
}

Logging::~Logging()
{
  deinit();
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

  for (const auto id : streamIDs_) {
    flagmap_[id]   = logflags;
    streammap_[id] = make_unique<FileLogStream>(id, flagmap_[id], logfile_);
  }
} // create

void Logging::setPrefix(std::string prefix)
{
  deinit();
  create(logflags_, prefix);
} // setPrefix

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
    DUNE_THROW(InvalidStateException,
               "cannot get flags for unkown Stream id, maybe you forgot to call \"Logging::create\"?");
  return it->second;
}

LogStream& Logging::getStream(int streamId)
{
  const auto it = streammap_.find(streamId);
  if (it == streammap_.end())
    DUNE_THROW(InvalidStateException, "cannot get unknown logStream, maybe you forgot to call \"Logging::create\"?");
  else {
    assert(it->second);
    return *(it->second);
  }
}

void Logging::flush()
{
  for (auto& pair : streammap_) {
    assert(pair.second);
    pair.second->flush();
  }
} // flush

int Logging::addStream(int flags)
{
  static int streamID_int = LOG_NEXT;
  streamID_int <<= 1;
  int streamID = streamID_int;
  streamIDs_.push_back(streamID);
  flagmap_[streamID]   = (flags | streamID);
  streammap_[streamID] = make_unique<FileLogStream>(streamID, flagmap_[streamID], logfile_);
  return streamID_int;
} // addStream

void Logging::resume(LogStream::PriorityType prio)
{
  for (auto& pair : streammap_) {
    pair.second->resume(prio);
  }
} // resume

void Logging::suspend(LogStream::PriorityType prio)
{
  for (auto& pair : streammap_) {
    pair.second->suspend(prio);
  }
} // suspend

} // namespace Common
} // namespace Stuff
} // namespace Dune
