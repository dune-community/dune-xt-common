#ifndef DUNE_COMMON_COLOR_HH
#define DUNE_COMMON_COLOR_HH

#include <sstream>
#include <assert.h>
#include <iostream>

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <string>


namespace Color {
// foreground colors
static const char* black     = "\033[30m";
static const char* red       = "\033[31m";
static const char* green     = "\033[32m";
static const char* brown     = "\033[33m";
static const char* blue      = "\033[34m";
static const char* purple    = "\033[35m";
static const char* cyan      = "\033[36m";
static const char* lightgray = "\033[37m";
// light foreground colors
static const char* darkgray    = "\033[1;30m";
static const char* lightred    = "\033[1;31m";
static const char* lightgreen  = "\033[1;32m";
static const char* yellow      = "\033[1;33m";
static const char* lightblue   = "\033[1;34m";
static const char* lightpurple = "\033[1;35m";
static const char* lightcyan   = "\033[1;36m";
static const char* white       = "\033[1;37m";

// background colors
static const char* bblack     = "\033[40m";
static const char* bred       = "\033[41m";
static const char* bgreen     = "\033[42m";
static const char* bbrown     = "\033[43m";
static const char* bblue      = "\033[44m";
static const char* bpurple    = "\033[45m";
static const char* bcyan      = "\033[46m";
static const char* blightgray = "\033[47m";
// light background colors
static const char* bdarkgray    = "\033[1;40m";
static const char* blightred    = "\033[1;41m";
static const char* blightgreen  = "\033[1;42m";
static const char* byellow      = "\033[1;43m";
static const char* blightblue   = "\033[1;44m";
static const char* blightpurple = "\033[1;45m";
static const char* blightcyan   = "\033[1;46m";
static const char* bwhite       = "\033[1;47m";
// modifiers
static const char* normal       = "\033[0m";
static const char* bold         = "\033[1m";
static const char* italic       = "\033[2m";
static const char* underline    = "\033[4m";
static const char* blink        = "\033[5m";
static const char* reverse      = "\033[7m";
static const char* enditalic    = "\033[22m";
static const char* endunderline = "\033[24m";
static const char* endblink     = "\033[25m";
static const char* endreverse   = "\033[27m";

// converts int to string by using ostringstream
std::string intToString(int i)
{
  std::ostringstream out;
  out << i;
  return out.str();
}

std::string color(int i)
{
  return "\033[38;5;" + intToString(i) + "m";
}

std::string backcolor(int i)
{
  return "\033[38;5;" + intToString(i) + "m";
}


// demangles typeid
template <class T>
std::string demangledTypeId(T& obj)
{
  int status;
#ifdef __GNUC__
  return abi::__cxa_demangle(typeid(obj).name(), 0, 0, &status);
#else
  return typeid(obj).name();
#endif
}

// create output for demangled typeid
template <class T>
void realTypeId(T& obj, std::string name = "", int maxlevel = 10000)
{
  std::cout << name << (name == "" ? "" : "'s type is ") << highlightTemplate(demangledTypeId(obj), maxlevel)
            << std::endl;
}

// maybe you want to choose your own color
int templateColorChooser(int i)
{
  return i % 256;
}

// highlight templates depending on the "template"-level
std::string highlightTemplate(std::string str, int maxlevel = 10000)
{
  if (maxlevel < 0)
    maxlevel     = 0;
  int startindex = 0;
  int level = 0;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '<') {
      level++;
      std::string dummy = "\033[38;5;" + intToString(templateColorChooser(level)) + "m";
      str.insert(i, dummy);
      i += dummy.size();
      if (level == maxlevel)
        startindex = i + 1;
    } else if (str[i] == '>') {
      level--;
      std::string dummy = "\033[38;5;" + intToString(templateColorChooser(level)) + "m";
      str.insert(++i, dummy);
      if (level + 1 == maxlevel) {
        int size = i - startindex - 1;
        str.erase(startindex, size);
        i = startindex + 1;
      }
      i += dummy.size();
    }
  }
  str += "\033[38;5;0m";
  return str;
}

// highlight a string in a specified color
std::string highlightString(std::string str, int colornr = 0)
{
  return "\033[38;5;" + intToString(colornr % 256) + "m" + str + "\033[38;5;0m";
}

// highlight a string which is searched in another string in a specified color
std::string highlightSearchString(std::string str, std::string searchstr, int colornr = 0)
{
  int index = str.find(searchstr, 0);
  while (index != int(std::string::npos)) {
    std::string dummy  = "\033[38;5;" + intToString(colornr % 256) + "m";
    std::string dummy2 = "\033[38;5;0m";
    str.insert(index, dummy);
    str.insert(index + searchstr.size() + dummy.size(), dummy2);
    index = str.find(searchstr, index + dummy.size() + searchstr.size() + dummy2.size());
  }
  return str;
}

} // end of namespace Color


#endif // end of DUNE_COMMON_COLOR_HH
