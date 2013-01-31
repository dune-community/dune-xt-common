#ifndef DUNE_COMMON_COLOR_HH
#define DUNE_COMMON_COLOR_HH

#include <sstream>
#include <assert.h>
#include <iostream>

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <string>

#include <dune/stuff/common/string.hh>

namespace Dune {
namespace Stuff {
namespace Common {

/**
 * @brief namespace to define color constants that can be
 * used to print colored text in an output stream.
 *
 * * \todo this could go into libdune-stuff
 * @warning Some color codes might be unsupported by your terminal.
 */
struct Colors
{

#define DS_CONST_CHAR const constexpr char *

  // foreground colors
  static DS_CONST_CHAR black     = "\033[30m";
  static DS_CONST_CHAR red       = "\033[31m";
  static DS_CONST_CHAR green     = "\033[32m";
  static DS_CONST_CHAR brown     = "\033[33m";
  static DS_CONST_CHAR blue      = "\033[34m";
  static DS_CONST_CHAR purple    = "\033[35m";
  static DS_CONST_CHAR cyan      = "\033[36m";
  static DS_CONST_CHAR lightgray = "\033[37m";
  // light foreground colors
  static DS_CONST_CHAR darkgray    = "\033[1;30m";
  static DS_CONST_CHAR lightred    = "\033[1;31m";
  static DS_CONST_CHAR lightgreen  = "\033[1;32m";
  static DS_CONST_CHAR yellow      = "\033[1;33m";
  static DS_CONST_CHAR lightblue   = "\033[1;34m";
  static DS_CONST_CHAR lightpurple = "\033[1;35m";
  static DS_CONST_CHAR lightcyan   = "\033[1;36m";
  static DS_CONST_CHAR white       = "\033[1;37m";

  // background colors
  static DS_CONST_CHAR bblack     = "\033[40m";
  static DS_CONST_CHAR bred       = "\033[41m";
  static DS_CONST_CHAR bgreen     = "\033[42m";
  static DS_CONST_CHAR bbrown     = "\033[43m";
  static DS_CONST_CHAR bblue      = "\033[44m";
  static DS_CONST_CHAR bpurple    = "\033[45m";
  static DS_CONST_CHAR bcyan      = "\033[46m";
  static DS_CONST_CHAR blightgray = "\033[47m";
  // light background colors
  static DS_CONST_CHAR bdarkgray    = "\033[1;40m";
  static DS_CONST_CHAR blightred    = "\033[1;41m";
  static DS_CONST_CHAR blightgreen  = "\033[1;42m";
  static DS_CONST_CHAR byellow      = "\033[1;43m";
  static DS_CONST_CHAR blightblue   = "\033[1;44m";
  static DS_CONST_CHAR blightpurple = "\033[1;45m";
  static DS_CONST_CHAR blightcyan   = "\033[1;46m";
  static DS_CONST_CHAR bwhite       = "\033[1;47m";
};
// modifiers
struct StreamModifiers
{
  static DS_CONST_CHAR normal       = "\033[0m";
  static DS_CONST_CHAR bold         = "\033[1m";
  static DS_CONST_CHAR italic       = "\033[2m";
  static DS_CONST_CHAR underline    = "\033[4m";
  static DS_CONST_CHAR blink        = "\033[5m";
  static DS_CONST_CHAR reverse      = "\033[7m";
  static DS_CONST_CHAR enditalic    = "\033[22m";
  static DS_CONST_CHAR endunderline = "\033[24m";
  static DS_CONST_CHAR endblink     = "\033[25m";
  static DS_CONST_CHAR endreverse   = "\033[27m";
#undef DS_CONST_CHAR
};
/**
 * @brief Chooses a color from a 256 color map for a foreground color.
 *
 * @param i The color number between 0 and 255.
 * @returns A string describing a color code.
 */
std::string color(int i)
{
  return "\033[38;5;" + toString(i) + "m";
}

/**
 * @brief Chooses a color from a 256 color map for a background color.
 *
 * @param i The color number between 0 and 255.
 * @returns A string describing a color code.
 */
std::string backcolor(int i)
{
  return "\033[38;5;" + toString(i) + "m";
}

// maybe you want to choose your own color
int templateColorChooser(int i)
{
  return i % 256;
}

/**
 * @brief Highlights templates depending on the "template"-level.
 *
 * @param str The string containing the template string
 * @param maxlevel The maximal template-level the string is reduced to.
 * @returns A colored template string.
 */
std::string highlightTemplate(std::string str, int maxlevel = 10000)
{
  if (maxlevel < 0)
    maxlevel     = 0;
  int startindex = 0;
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
        int size = i - startindex - 1;
        str.erase(startindex, size);
        i = startindex + 1;
      }
      i += dummy.size();
    }
  }
  str += "\033[38;5;0m";
  return str;
} // highlightTemplate

/**
 * @brief A simple function highlighting a whole string in a specified foreground color.
 *
 * @param str The string you want to highlight.
 * @param colornr A color number from a 256 color map between 0 and 255.
 * @returns The highlighted string.
 */
std::string highlightString(std::string str, int colornr = 0)
{
  return "\033[38;5;" + toString(colornr % 256) + "m" + str + "\033[0m"; //"\033[38;5;0m";
}

std::string colorString(const std::string _string, const std::string _color = Colors::brown)
{
  return _color + _string + "\033[0m";
}

std::string colorStringRed(const std::string _string)
{
  return colorString(_string, Colors::red);
}

/**
 * @brief Highlights a substring of another string in a specified color.
 *
 * @param str The string where you want to highlight substrings.
 * @param substr The sub string you want to highlight in str.
 * @param colornr A color number from a 256 color map between 0 and 255.
 * @returns The highlighted string.
 */
std::string highlightSearchString(std::string str, std::string substr, int colornr = 0)
{
  int index = str.find(substr, 0);

  while (index != int(std::string::npos)) {
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

#endif // end of DUNE_COMMON_COLOR_HH

/** Copyright (c) 2012, Stefan Girke
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
