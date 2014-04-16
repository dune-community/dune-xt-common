// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
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
std::string pathOnly(std::string _path);

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
