#include "color.hh"

namespace Dune {
namespace Stuff {
namespace Common {

// foreground colors
const char* Colors::black     = "\033[30m";
const char* Colors::red       = "\033[31m";
const char* Colors::green     = "\033[32m";
const char* Colors::brown     = "\033[33m";
const char* Colors::blue      = "\033[34m";
const char* Colors::purple    = "\033[35m";
const char* Colors::cyan      = "\033[36m";
const char* Colors::lightgray = "\033[37m";
// light foreground colors
const char* Colors::darkgray    = "\033[1;30m";
const char* Colors::lightred    = "\033[1;31m";
const char* Colors::lightgreen  = "\033[1;32m";
const char* Colors::yellow      = "\033[1;33m";
const char* Colors::lightblue   = "\033[1;34m";
const char* Colors::lightpurple = "\033[1;35m";
const char* Colors::lightcyan   = "\033[1;36m";
const char* Colors::white       = "\033[1;37m";

// background colors
const char* Colors::bblack     = "\033[40m";
const char* Colors::bred       = "\033[41m";
const char* Colors::bgreen     = "\033[42m";
const char* Colors::bbrown     = "\033[43m";
const char* Colors::bblue      = "\033[44m";
const char* Colors::bpurple    = "\033[45m";
const char* Colors::bcyan      = "\033[46m";
const char* Colors::blightgray = "\033[47m";
// light background colors
const char* Colors::bdarkgray    = "\033[1;40m";
const char* Colors::blightred    = "\033[1;41m";
const char* Colors::blightgreen  = "\033[1;42m";
const char* Colors::byellow      = "\033[1;43m";
const char* Colors::blightblue   = "\033[1;44m";
const char* Colors::blightpurple = "\033[1;45m";
const char* Colors::blightcyan   = "\033[1;46m";
const char* Colors::bwhite       = "\033[1;47m";
// modifiers
const char* StreamModifiers::normal       = "\033[0m";
const char* StreamModifiers::bold         = "\033[1m";
const char* StreamModifiers::italic       = "\033[2m";
const char* StreamModifiers::underline    = "\033[4m";
const char* StreamModifiers::blink        = "\033[5m";
const char* StreamModifiers::reverse      = "\033[7m";
const char* StreamModifiers::enditalic    = "\033[22m";
const char* StreamModifiers::endunderline = "\033[24m";
const char* StreamModifiers::endblink     = "\033[25m";
const char* StreamModifiers::endreverse   = "\033[27m";


std::string highlightTemplate(std::string str, int maxlevel /*= 10000*/)
{
  if (maxlevel < 0)
    maxlevel        = 0;
  size_t startindex = 0;
  int level = 0;
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == '<') {
      level++;
      std::string dummy = "\033[38;5;" + toString(templateColorChooser(level)) + "m";
      str.insert(i, dummy);
      i += dummy.size();
      if (level == maxlevel)
        startindex = i + 1;
    } else if (str[i] == '>') {
      level--;
      std::string dummy = "\033[38;5;" + toString(templateColorChooser(level)) + "m";
      str.insert(++i, dummy);
      if (level + 1 == maxlevel) {
        auto size = i - startindex - 1;
        str.erase(startindex, size);
        i = startindex + 1;
      }
      i += dummy.size();
    }
  }
  str += "\033[38;5;0m";
  return str;
} // highlightTemplate

std::string highlightSearchString(std::string str, std::string substr, int colornr /*= 0*/)
{
  long index = long(str.find(substr, 0));

  while (index != long(std::string::npos)) {
    std::string dummy  = "\033[38;5;" + toString(colornr % 256) + "m";
    std::string dummy2 = "\033[38;5;0m";
    str.insert(index, dummy);
    str.insert(index + substr.size() + dummy.size(), dummy2);
    index = str.find(substr, index + dummy.size() + substr.size() + dummy2.size());
  }
  return str;
} // highlightSearchString

std::string colorString(const std::string _string, const std::string _color /*= Colors::brown*/)
{
  return _color + _string + "\033[0m";
}

std::string colorStringRed(const std::string _string)
{
  return colorString(_string, Colors::red);
}


} // namespace Common
} // namespace Stuff
} // namespace Dune
