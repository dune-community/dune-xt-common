#include "color.hh"

namespace Dune {
namespace Stuff {
namespace Common {

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

} // namespace Common
} // namespace Stuff
} // namespace Dune
