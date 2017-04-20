// This file is part of the dune-xt-common project:
//   https://github.com/dune-community/dune-xt-common
// Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2012, 2014, 2016 - 2017)
//   Rene Milk       (2010 - 2016)
//   Sven Kaulmann   (2011)

#ifndef DUNE_XT_COMMON_FILESYSTEM_HH
#define DUNE_XT_COMMON_FILESYSTEM_HH

#include "debug.hh"
#include "string.hh"
#include "logstreams.hh"
#include <string>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Dune
{
namespace XT {
namespace Common {

//! strip filename from \path if present, return empty string if only filename present
std::string directory_only(std::string _path);

//! return everything after the last slash
std::string filename_only(const std::string& _path);

//! may include filename, will be stripped
void test_create_directory(const std::string _path);

//! pure c++ emulation of system's touch binary
bool touch(const
   
    std::string& _path);

std::unique_ptr<boost::filesystem::ofstream> make_ofstream(const boost::filesystem::path& path,

                                                           const std::ios_base::openmode mode = std::ios_base::out);

std::unique_ptr<boost::filesystem::ifstream> make_ifstream(const boost::filesystem::path& path,
                                                           const std::ios_base::openmode mode = std::ios_base::in);

//! read a file and output all lines containing filter string to a stream
void file_to_stream_filtered(std::ostream& stream, std::string filename, std::string filter);

//! output programs mem usage stats by reading from /proc
void meminfo(LogStream& stream);

} // namespace Common
} // namespace XT
} // namespace Dune

#endif // DUNE_XT_COMMON_FILESYSTEM_HH
