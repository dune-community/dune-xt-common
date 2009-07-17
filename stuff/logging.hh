/**
 *  \file logging.hh
 *  \brief  logging
 **/
#ifndef LOGGING_HH_INCLUDED
#define LOGGING_HH_INCLUDED

#include <fstream>
#include <ostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <assert.h>
#include "parametercontainer.hh"
#include "misc.hh"


/** \brief handles all logging
**/
class Logging
{
public:
  enum LogFlags
  {
    LOG_NONE    = 1,
    LOG_ERR     = 2,
    LOG_INFO    = 4,
    LOG_DEBUG   = 8,
    LOG_CONSOLE = 16,
    LOG_FILE    = 32
  };

  //! only for logging to a single file which should then be executabla by matlab
  class MatlabLogStream
  {
  public:
    MatlabLogStream(LogFlags loglevel, int& logflags, std::ofstream& logFile)
      : matlabLogFile_(logFile)
      , loglevel_(loglevel)
      , logflags_(logflags)
      , suspended_logflags_(logflags)
    {
    }

    ~MatlabLogStream()
    {
    }

    template <typename T>
    MatlabLogStream& operator<<(T in)
    {
      if (logflags_ & loglevel_)
        buffer_ << in;
      return *this;
    }

    MatlabLogStream& operator<<(MatlabLogStream& (*pf)(MatlabLogStream&))
    {
      if (logflags_ & loglevel_)
        buffer_ << pf;
      return *this;
    }

    MatlabLogStream& operator<<(std::ostream& (*pf)(std::ostream&))
    {
      if (logflags_ & loglevel_) {
        if (pf == (std::ostream & (*)(std::ostream&))std::endl) { // flush buffer into stream
          buffer_ << "\n";
          Flush();
        } else {
          buffer_ << pf;
        }
      }
      return *this;
    }

    void Flush()
    {
      matlabLogFile_ << buffer_.str();
      matlabLogFile_.flush();
      buffer_.flush();
      buffer_.str(""); // clear the buffer
    }

    void Suspend()
    {
      // don't accidentally invalidate flags if already suspended
      if (!is_suspended_) {
        suspended_logflags_ = logflags_;
        logflags_           = 1;
      }
      is_suspended_ = true;
    }

    void Resume()
    {
      if (is_suspended_)
        logflags_   = suspended_logflags_;
      is_suspended_ = false;
    }

  private:
    std::stringstream buffer_;
    std::ofstream& matlabLogFile_;
    LogFlags loglevel_;
    int& logflags_;
    int suspended_logflags_;
    bool is_suspended_;
  };

  class LogStream //: virtual public std::ostream
  {
  protected:
    LogFlags loglevel_;
    int& logflags_;
    int suspended_logflags_;
    std::stringstream buffer_;
    std::ofstream& logfile_;
    std::ofstream& logfileWoTime_;
    bool is_suspended_;

  public:
    LogStream(LogFlags loglevel, int& logflags, std::ofstream& file, std::ofstream& fileWoTime)
      : loglevel_(loglevel)
      , logflags_(logflags)
      , suspended_logflags_(logflags)
      , logfile_(file)
      , logfileWoTime_(fileWoTime)
      , is_suspended_(false)
    {
    }
    ~LogStream()
    {
    }

    template <typename T>
    LogStream& operator<<(T in)
    {
      SetColor();
      if (logflags_ & loglevel_)
        buffer_ << in;
      UnsetColor();
      return *this;
    }

    void Suspend()
    {
      // don't accidentally invalidate flags if already suspended
      if (!is_suspended_) {
        suspended_logflags_ = logflags_;
        logflags_           = 1;
      }
      is_suspended_ = true;
    }

    void Resume()
    {
      if (is_suspended_)
        logflags_   = suspended_logflags_;
      is_suspended_ = false;
    }

    void Flush()
    {
      if (logflags_ & loglevel_) { // flush buffer into stream
        if ((logflags_ & LOG_CONSOLE) != 0) {
          std::cout << buffer_.str(); // << std::endl;
          std::cout.flush();
        }
        if ((logflags_ & LOG_FILE) != 0) {
          logfile_ << "\n" << TimeString() << buffer_.str() << std::endl;
          logfileWoTime_ << buffer_.str(); // << std::endl;
          logfile_.flush();
          logfileWoTime_.flush();
        }
        buffer_.flush();
        buffer_.str(""); // clear the buffer
      }
    }

    void SetColor()
    {
      //                    if ( logflags_ & LOG_INFO ) {
      //                        buffer_ << "\033[21;31m";
      //                    }
    }

    void UnsetColor()
    {
      //                    buffer_ << "\033[0m";
    }

    // template < class Class >
    LogStream& operator<<(LogStream& (*pf)(LogStream&))
    {
      if (logflags_ & loglevel_)
        buffer_ << pf;
      return *this;
    }

    LogStream& operator<<(std::ostream& (*pf)(std::ostream&))
    {
      SetColor();
      if (logflags_ & loglevel_) {
        if (pf == (std::ostream & (*)(std::ostream&))std::endl) { // flush buffer into stream
          buffer_ << "\n";
          Flush();
        } else
          buffer_ << pf;
      }
      UnsetColor();
      return *this;
    }

    template <class Class, typename Pointer>
    void Log(Pointer pf, Class& c)
    {
      if (logflags_ & loglevel_) {
        (c.*pf)(buffer_);
      }
    }
  };

protected:
  Logging()
  {
    streamIDs_.push_back(LOG_ERR);
    streamIDs_.push_back(LOG_DEBUG);
    streamIDs_.push_back(LOG_INFO);
  }

  ~Logging()
  {
    IdVecCIter it = streamIDs_.end();
    for (; it != streamIDs_.begin(); --it) {
      Stuff::safe_delete(streammap_[*it]);
    }

    if ((logflags_ & LOG_FILE) != 0) {
      logfile_ << '\n' << TimeString() << ": LOG END" << std::endl;
      logfileWoTime_ << std::endl;
      logfile_.close();
      logfileWoTime_.close();
    }

    // delete the MatlabLogStream
    matlabLogStreamPtr->Flush();
    matlabLogFile_.close();
    Stuff::safe_delete(matlabLogStreamPtr);
  }

public:
  /** \brief setup loglevel, logfilename
      \param logflags any OR'd combination of flags
      \param logfile filename for log, can contain paths, but creation will fail if dir is non-existant
  **/
  void Create(unsigned int logflags = LOG_FILE | LOG_CONSOLE | LOG_ERR, std::string logfile = "dune_stokes")
  {
    logflags_ = logflags;

    // if we get a logdir from parameters append path seperator, othwersie leave empty
    // enables us to use logdir unconditionally further down
    std::string logdir = Parameters().getParam("fem.io.logdir", std::string());
    if (!logdir.empty())
      logdir += "/";

    filename_ = logdir + logfile + "_time.log";
    Stuff::testCreateDirectory(filename_); // could assert this if i figure out why errno is != EEXIST
    filenameWoTime_ = logdir + logfile + ".log";
    if ((logflags_ & LOG_FILE) != 0) {
      logfile_.open(filename_.c_str());
      assert(logfile_.is_open());
      logfileWoTime_.open(filenameWoTime_.c_str());
      assert(logfileWoTime_.is_open());
    }
    IdVecCIter it = streamIDs_.begin();
    for (; it != streamIDs_.end(); ++it) {
      flagmap_[*it]   = logflags;
      streammap_[*it] = new LogStream(*it, flagmap_[*it], logfile_, logfileWoTime_);
    }
    // create the MatlabLogStream
    std::string matlabLogFileName = logdir + logfile + "_matlab.m";
    Stuff::testCreateDirectory(matlabLogFileName); // could assert this if i figure out why errno is != EEXIST
    matlabLogFile_.open(matlabLogFileName.c_str());
    assert(matlabLogFile_.is_open());
    matlabLogStreamPtr = new MatlabLogStream(LOG_FILE, logflags_, matlabLogFile_);
  }

  void SetPrefix(std::string prefix)
  {
    /// begin dtor
    IdVecCIter it = streamIDs_.end();
    for (; it != streamIDs_.begin(); --it) {
      Stuff::safe_delete(streammap_[*it]);
    }

    if ((logflags_ & LOG_FILE) != 0) {
      logfile_ << '\n' << TimeString() << ": LOG END" << std::endl;
      logfileWoTime_ << std::endl;
      logfile_.close();
      logfileWoTime_.close();
    }

    // delete the MatlabLogStream
    matlabLogStreamPtr->Flush();
    matlabLogFile_.close();
    Stuff::safe_delete(matlabLogStreamPtr);
    /// end dtor

    Create(logflags_, prefix);
  }

  void SetStreamFlags(LogFlags stream, int flags)
  {
    assert(stream & (LOG_ERR | LOG_INFO | LOG_DEBUG));
    // this might result in logging to diff targtes, so we flush the current targets
    flagmap_[stream] = flags;
  }

  int GetStreamFlags(LogFlags stream)
  {
    assert(stream & (LOG_ERR | LOG_INFO | LOG_DEBUG));
    return flagmap_[stream];
  }

  /** \name Log funcs for member-function pointers
  * \{
  */
  template <typename Pointer, class Class>
  void LogDebug(Pointer pf, Class& c)
  {
    if ((logflags_ & LOG_DEBUG))
      Log(pf, c, LOG_DEBUG);
  }

  template <class Class, typename Pointer>
  void LogInfo(Pointer pf, Class& c)
  {
    if ((logflags_ & LOG_INFO))
      Log(pf, c, LOG_INFO);
  }

  template <typename Pointer, class Class>
  void LogErr(Pointer pf, Class& c)
  {
    if ((logflags_ & LOG_ERR))
      Log(pf, c, LOG_ERR);
  }

  template <typename Pointer, class Class>
  void Log(void (Class::*pf)(std::ostream&) const, Class& c, LogFlags stream)
  {
    assert(stream & (LOG_ERR | LOG_INFO | LOG_DEBUG));
    if ((flagmap_[stream] & LOG_CONSOLE) != 0)
      (c.*pf)(std::cout);
    if ((flagmap_[stream] & LOG_FILE) != 0)
      (c.*pf)(logfile_);
  }

  template <class Class, typename Pointer>
  void Log(Pointer pf, Class& c, LogFlags stream)
  {
    assert(stream & (LOG_ERR | LOG_INFO | LOG_DEBUG));
    if ((flagmap_[stream] & LOG_CONSOLE) != 0)
      (c.*pf)(std::cout);
    if ((flagmap_[stream] & LOG_FILE) != 0) {
      (c.*pf)(logfile_);
      (c.*pf)(logfileWoTime_);
    }
  }

  /** \}
  */

  /** \name Log funcs for basic types/classes
  * \{
  */
  template <class Class>
  void LogDebug(Class c)
  {
    if ((logflags_ & LOG_DEBUG))
      Log(c, LOG_DEBUG);
  }

  template <class Class>
  void LogInfo(Class c)
  {
    if ((logflags_ & LOG_INFO))
      Log(c, LOG_INFO);
  }

  template <class Class>
  void LogErr(Class c)
  {
    if ((logflags_ & LOG_ERR))
      Log(c, LOG_ERR);
  }

  template <class Class>
  void Log(Class c, LogFlags stream)
  {
    assert(stream & (LOG_ERR | LOG_INFO | LOG_DEBUG));
    if ((flagmap_[stream] & LOG_CONSOLE) != 0)
      std::cout << c;
    if ((flagmap_[stream] & LOG_FILE) != 0) {
      logfile_ << c;
      logfileWoTime_ << c;
    }
  }
  /** \}
  */

  LogStream& GetStream(int stream)
  {
    assert(streammap_[(LogFlags)stream]);
    return *streammap_[(LogFlags)stream];
  }
  LogStream& Err()
  {
    return GetStream(LOG_ERR);
  }
  LogStream& Info()
  {
    return GetStream(LOG_INFO);
  }
  LogStream& Dbg()
  {
    return GetStream(LOG_DEBUG);
  }
  MatlabLogStream& Matlab()
  {
    return *matlabLogStreamPtr;
  }

  static std::string TimeString()
  {
    const time_t cur_time = time(NULL);
    return ctime(&cur_time);
  }

  int AddStream(int flags)
  {
    //            assert( streamIDs_.find( streamID ) == streamIDs_.end() );
    static int streamID_int = 16;
    streamID_int <<= 2;
    LogFlags streamID = (LogFlags)streamID_int;
    streamIDs_.push_back(streamID);
    flagmap_[streamID]   = flags | streamID;
    streammap_[streamID] = new LogStream(streamID, flagmap_[streamID], logfile_, logfileWoTime_);
    return streamID_int;
  }

private:
  std::string filename_;
  std::string filenameWoTime_;
  std::ofstream logfile_;
  std::ofstream logfileWoTime_;
  std::ofstream matlabLogFile_;
  typedef std::map<LogFlags, int> FlagMap;
  FlagMap flagmap_;
  typedef std::map<LogFlags, LogStream*> StreamMap;
  StreamMap streammap_;
  typedef std::vector<LogFlags> IdVec;
  typedef std::vector<LogFlags>::const_iterator IdVecCIter;
  IdVec streamIDs_;
  int logflags_;
  MatlabLogStream* matlabLogStreamPtr;

  friend Logging& Logger();
};

/// global Logging instance
Logging& Logger()
{
  static Logging log;
  return log;
}

#endif
