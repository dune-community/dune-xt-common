// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "filesystem.hh"

namespace Dune {
namespace Stuff {
namespace Common {

//! strip filename from \path if present, return empty string if only filename present
std::string directoryOnly(std::string _path)
{
  return boost::filesystem::path(_path).parent_path().string();
}

//! return everything after the last slash
std::string filenameOnly(const std::string& _path)
{
#if BOOST_FILESYSTEM_VERSION > 2
  return boost::filesystem::path(_path).filename().string();
#else // if BOOST_FILESYSTEM_VERSION > 2
  return boost::filesystem::path(_path).filename();
#endif // if BOOST_FILESYSTEM_VERSION > 2
} // filenameOnly

//! may include filename, will be stripped
void testCreateDirectory(const std::string _path)
{
  std::string pathonly = directoryOnly(_path);
  if (!pathonly.empty())
    boost::filesystem::create_directories(pathonly);
}

//! pure c++ emulation of system's touch binary
bool touch(const std::string& _path)
{
  return std::ofstream(_path.c_str()).is_open();
}

boost::filesystem::ofstream* make_ofstream(const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
  testCreateDirectory(path.string());
  return new boost::filesystem::ofstream(path, mode);
}

boost::filesystem::ifstream* make_ifstream(const boost::filesystem::path& path, const std::ios_base::openmode mode)
{
  testCreateDirectory(path.string());
  return new boost::filesystem::ifstream(path, mode);
}

//! read a file and output all lines containing filter string to a stream
void fileToStreamFiltered(std::ostream& stream, std::string filename, std::string filter)
{
  std::ifstream file(filename.c_str(), std::ifstream::in);
  std::string line;
  while (file.good()) {
    std::getline(file, line);
    if (line.find(filter) != std::string::npos)
      stream << line << "\n";
  }
  file.close();
} // fileToStreamFiltered

//! output programs mem usage stats by reading from /proc
void meminfo(Dune::Stuff::Common::LogStream& stream)
{
  stream << "Memory info: \n";
  stream.resume();
  pid_t pid = getpid();
  std::stringstream filename;
  filename << "/proc/" << pid << "/status";

  fileToStreamFiltered(stream, filename.str(), "Vm");
  fileToStreamFiltered(stream, "/proc/meminfo", "Mem");
  stream << "------------ \n\n" << std::endl;
} // meminfo

} // namespace Common
} // namespace Stuff
} // namespace Dune
