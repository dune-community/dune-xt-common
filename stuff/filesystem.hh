#ifndef DUNE_STUFF_FILESYSTEM_HH
#define DUNE_STUFF_FILESYSTEM_HH

#include "debug.hh"
#include "misc.hh"
#include <string>
#include <fstream>

#include <dune/fem/io/file/iointerface.hh>
#include <boost/filesystem.hpp>

namespace Stuff {

//! strip filename from \path if present, return empty string if only filename present
std::string pathOnly(std::string path)
{
  return boost::filesystem::path(path).parent_path().string();
}

//! return everything after the last slash
std::string filenameOnly(const std::string& path)
{
  return boost::filesystem::path(path).filename().string();
}

//! may include filename, will be stripped
void testCreateDirectory(const std::string path)
{
  std::string pathonly = pathOnly(path);
  boost::filesystem::create_directories(pathonly);
}

//! read a file and output all lines containing filter string to a stream
template <class Stream>
void fileToStreamFiltered(Stream& stream, std::string filename, std::string filter)
{
  std::ifstream file(filename.c_str(), std::ifstream::in);
  ASSERT_EXCEPTION(file.good(), filename.c_str());
  while (file.good()) {
    std::string line;
    std::getline(file, line);
    if (line.find(filter) != std::string::npos)
      stream << line << "\n";
  }
  file.close();
}

//! output programs mem usage stats by reading from /proc
template <class Stream>
void meminfo(Stream& stream)
{
  stream << "Memory info: \n";
  stream.Resume();
  pid_t pid = getpid();
  std::stringstream filename;
  filename << "/proc/" << pid << "/status";

  fileToStreamFiltered(stream, filename.str(), "Vm");
  fileToStreamFiltered(stream, "/proc/meminfo", "Mem");
  stream << "------------ \n\n" << std::endl;
}

} // end namespace Stuff

#endif // DUNE_STUFF_FILESYSTEM_HH
