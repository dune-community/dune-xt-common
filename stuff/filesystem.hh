#ifndef DUNE_STUFF_FILESYSTEM_HH
#define DUNE_STUFF_FILESYSTEM_HH

#include "debug.hh"
#include <string>
#include <fstream>

namespace Stuff {

//! strip filename from \path if present, return empty string if only filename present
std::string pathOnly(std::string path)
{
  if (!path.empty()) {
    char buf[1024]; // not _exactly_ sure this is max path length, but it's suggested in wx source

    // Local copy
    strcpy(buf, path.c_str());

    int l = path.length();
    int i = l - 1;

    // Search backward for a backward or forward slash
    while (i > -1) {
      if ((path[i] == '/') || (path[i] == '\\')) {
        // Don't return an empty string
        if (i == 0)
          i++;
        buf[i] = 0;
        return std::string(buf);
      }
      i--;
    }
  }
  return std::string();
}

//! return everything after the last slash
std::string filenameOnly(const std::string& path)
{
  size_t last_slash_position = path.find_last_of('/');
  return path.substr(last_slash_position + 1);
}

//! may include filename, will be stripped
bool testCreateDirectory(std::string path)
{
  std::string pathonly = pathOnly(path);
  if (pathonly.empty())
    return true; // no dir to create

  // maybe test if dir exists??
  bool ok = (mkdir(pathonly.c_str(), 0755) == 0);
  if (!ok) {
    perror(pathonly.c_str());
    return errno == EEXIST;
  }
  return true;
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
