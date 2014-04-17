// This file is part of the dune-stuff project:
//   http://users.dune-project.org/projects/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//
// Contributors: Stefan Girke

#include "test_common.hh"
#include <dune/stuff/common/color.hh>

using namespace Dune::Stuff::Common;

TEST(Color, All)
{
  std::cout << "Testing some color codes for this terminal." << std::endl;
  std::cout << "WARNING: This test will succeed although some of the color codes" << std::endl;
  std::cout << "are not supported by your terminal!" << std::endl;
  std::cout << "(Sometimes 'blink', 'reverse' or 'italic' are not supported.)" << std::endl;
  std::cout << StreamModifiers::underline << "a simple 'hello world': " << StreamModifiers::endunderline
            << "hello world" << std::endl;
  std::cout << StreamModifiers::underline << "a colored 'hello world': " << StreamModifiers::endunderline
            << highlightString("hello world", 1) << std::endl;
  std::cout << StreamModifiers::underline << "a colored 'hello world': " << StreamModifiers::endunderline
            << Colors::bgreen << "hello world" << StreamModifiers::normal << std::endl;
  std::cout << StreamModifiers::underline << "a blinking 'hello world': " << StreamModifiers::endunderline
            << StreamModifiers::blink << "hello world" << StreamModifiers::endblink << std::endl;
  std::cout << StreamModifiers::underline << "an italic 'hello world': " << StreamModifiers::endunderline
            << StreamModifiers::italic << "hello world" << StreamModifiers::enditalic << std::endl;
  std::cout << StreamModifiers::underline << "an underlined 'hello world': " << StreamModifiers::endunderline
            << StreamModifiers::underline << "hello world" << StreamModifiers::endunderline << std::endl;
  std::cout << StreamModifiers::underline << "a reverse 'hello world': " << StreamModifiers::endunderline
            << StreamModifiers::reverse << "hello world" << StreamModifiers::endreverse << std::endl;
  std::cout << StreamModifiers::underline
            << "a 'hello world' with highlighted substrings ('o'): " << StreamModifiers::endunderline
            << highlightSearchString("hello world", "o", 3) << std::endl;
  std::cout << StreamModifiers::underline << "a highlighted 'hello world'-template: " << StreamModifiers::endunderline
            << highlightTemplate("Hello< World, Hello< World, Hello< World< Hello, World > > > >") << std::endl;
  std::cout << StreamModifiers::underline
            << "a highlighted 'hello world'-template only showing two levels: " << StreamModifiers::endunderline
            << highlightTemplate("Hello< World, Hello< World, Hello< World< Hello, World > > > >", 2) << std::endl;
  std::cout << StreamModifiers::underline
            << "colored 'hello world' for all available colors( 0 - 255): " << StreamModifiers::endunderline
            << std::endl;
  for (int i = 0; i < 256; ++i)
    std::cout << highlightString("hello world - ", i);
  std::cout << std::endl;
} // main

int main(int argc, char** argv)
{
  test_init(argc, argv);
  return RUN_ALL_TESTS();
}
