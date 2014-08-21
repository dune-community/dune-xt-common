// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Stefan Girke

#ifndef DUNE_COMMON_COLOR_HH
#define DUNE_COMMON_COLOR_HH

#include <sstream>
#include <assert.h>
#include <iostream>

#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <string>

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

#define DS_CONST_CHAR const char *

  // foreground colors
  static DS_CONST_CHAR black;
  static DS_CONST_CHAR red;
  static DS_CONST_CHAR green;
  static DS_CONST_CHAR brown;
  static DS_CONST_CHAR blue;
  static DS_CONST_CHAR purple;
  static DS_CONST_CHAR cyan;
  static DS_CONST_CHAR lightgray;
  // light foreground colors
  static DS_CONST_CHAR darkgray;
  static DS_CONST_CHAR lightred;
  static DS_CONST_CHAR lightgreen;
  static DS_CONST_CHAR yellow;
  static DS_CONST_CHAR lightblue;
  static DS_CONST_CHAR lightpurple;
  static DS_CONST_CHAR lightcyan;
  static DS_CONST_CHAR white;

  // background colors
  static DS_CONST_CHAR bblack;
  static DS_CONST_CHAR bred;
  static DS_CONST_CHAR bgreen;
  static DS_CONST_CHAR bbrown;
  static DS_CONST_CHAR bblue;
  static DS_CONST_CHAR bpurple;
  static DS_CONST_CHAR bcyan;
  static DS_CONST_CHAR blightgray;
  // light background colors
  static DS_CONST_CHAR bdarkgray;
  static DS_CONST_CHAR blightred;
  static DS_CONST_CHAR blightgreen;
  static DS_CONST_CHAR byellow;
  static DS_CONST_CHAR blightblue;
  static DS_CONST_CHAR blightpurple;
  static DS_CONST_CHAR blightcyan;
  static DS_CONST_CHAR bwhite;
};
// modifiers
struct StreamModifiers
{
  static DS_CONST_CHAR normal;
  static DS_CONST_CHAR bold;
  static DS_CONST_CHAR italic;
  static DS_CONST_CHAR underline;
  static DS_CONST_CHAR blink;
  static DS_CONST_CHAR reverse;
  static DS_CONST_CHAR enditalic;
  static DS_CONST_CHAR endunderline;
  static DS_CONST_CHAR endblink;
  static DS_CONST_CHAR endreverse;
#undef DS_CONST_CHAR
};

/**
 * @brief Chooses a color from a 256 color map for a foreground color.
 *
 * @param i The color number between 0 and 255.
 * @returns A string describing a color code.
 */
inline std::string color(int i)
{
  return "\033[38;5;" + std::to_string(i) + "m";
}

/**
 * @brief Chooses a color from a 256 color map for a background color.
 *
 * @param i The color number between 0 and 255.
 * @returns A string describing a color code.
 */
inline std::string backcolor(int i)
{
  return "\033[38;5;" + std::to_string(i) + "m";
}

inline // maybe you want to choose your own color
    int
    templateColorChooser(int i)
{
  return i % 256;
}


bool terminal_supports_color();


/**
 * @brief Highlights templates depending on the "template"-level.
 *
 * @param str The string containing the template string
 * @param maxlevel The maximal template-level the string is reduced to.
 * @returns A colored template string.
 */
std::string highlightTemplate(std::string str, int maxlevel = 10000);

/**
 * @brief A simple function highlighting a whole string in a specified foreground color.
 *
 * @param str The string you want to highlight.
 * @param colornr A color number from a 256 color map between 0 and 255.
 * @returns The highlighted string.
 */
inline std::string highlightString(std::string str, int colornr = 0)
{
  return "\033[38;5;" + std::to_string(colornr % 256) + "m" + str + "\033[0m"; //"\033[38;5;0m";
}

std::string colorString(const std::string str, const std::string clr = Colors::brown);

std::string colorStringRed(const std::string str);

/**
 * @brief Highlights a substring of another string in a specified color.
 *
 * @param str The string where you want to highlight substrings.
 * @param substr The sub string you want to highlight in str.
 * @param colornr A color number from a 256 color map between 0 and 255.
 * @returns The highlighted string.
 */
std::string highlightSearchString(std::string str, std::string substr, int colornr = 0);

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // end of DUNE_COMMON_COLOR_HH
