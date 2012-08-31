#ifndef DUNE_STUFF_FILESYSTEM_HH
#define DUNE_STUFF_FILESYSTEM_HH

#include "debug.hh"
#include "string.hh"
#include "logstreams.hh"
#include <string>
#include <fstream>

#include <dune/fem/io/file/iointerface.hh>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

//! strip filename from \path if present, return empty string if only filename present
std::string pathOnly(std::string _path)
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
  std::string pathonly = pathOnly(_path);
  if (!pathonly.empty())
    boost::filesystem::create_directories(pathonly);
}

bool touch(const std::string& _path)
{
  return std::ofstream(_path.c_str()).is_open();
}

boost::filesystem::ofstream* make_ofstream(const boost::filesystem::path& path,
                                           const std::ios_base::openmode mode = std::ios_base::out)
{
  testCreateDirectory(path.string());
  return new boost::filesystem::ofstream(path, mode);
}

boost::filesystem::ifstream* make_ifstream(const boost::filesystem::path& path,
                                           const std::ios_base::openmode mode = std::ios_base::in)
{
  testCreateDirectory(path.string());
  return new boost::filesystem::ifstream(path, mode);
}

//! read a file and output all lines containing filter string to a stream
void fileToStreamFiltered(std::ostream& stream, std::string filename, std::string filter)
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

#endif // DUNE_STUFF_FILESYSTEM_HH

/** Copyright (c) 2012, Rene Milk
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
