// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_FILESYSTEM_HH
#define DUNE_STUFF_FILESYSTEM_HH

#include "debug.hh"
#include "string.hh"
#include "logstreams.hh"
#include <string>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Dune {
namespace Stuff {
namespace Common {

//! strip filename from \path if present, return empty string if only filename present
std::string directoryOnly(std::string _path);

//! return everything after the last slash
std::string filenameOnly(const std::string& _path);

//! may include filename, will be stripped
void testCreateDirectory(const std::string _path);

//! pure c++ emulation of system's touch binary
bool touch(const std::string& _path);

boost::filesystem::ofstream* make_ofstream(const boost::filesystem::path& path,
                                           const std::ios_base::openmode mode = std::ios_base::out);

boost::filesystem::ifstream* make_ifstream(const boost::filesystem::path& path,
                                           const std::ios_base::openmode mode = std::ios_base::in);

//! read a file and output all lines containing filter string to a stream
void fileToStreamFiltered(std::ostream& stream, std::string filename, std::string filter);

//! output programs mem usage stats by reading from /proc
void meminfo(Dune::Stuff::Common::LogStream& stream);

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_FILESYSTEM_HH
