#include "color.hh"

using namespace Color;

int main()
{
  std::cout << underline << "a simple 'hello world': " << endunderline << "hello world" << std::endl;
  std::cout << underline << "a colored 'hello world': " << endunderline << highlightString("hello world", 1)
            << std::endl;
  std::cout << underline << "a colored 'hello world': " << endunderline << bgreen << "hello world" << normal
            << std::endl;
  std::cout << underline << "a blinking 'hello world': " << endunderline << blink << "hello world" << endblink
            << std::endl;
  std::cout << underline << "an italic 'hello world': " << endunderline << italic << "hello world" << enditalic
            << std::endl;
  std::cout << underline << "an underlined 'hello world': " << endunderline << underline << "hello world"
            << endunderline << std::endl;
  std::cout << underline << "a reverse 'hello world': " << endunderline << reverse << "hello world" << endreverse
            << std::endl;
  std::cout << underline << "a 'hello world' with highlighted substrings ('o'): " << endunderline
            << highlightSearchString("hello world", "o", 3) << std::endl;
  std::cout << underline << "a highlighted 'hello world'-template: " << endunderline
            << highlightTemplate("Hello< World, Hello< World, Hello< World< Hello, World > > > >") << std::endl;
  std::cout << underline << "a highlighted 'hello world'-template only showing two levels: " << endunderline
            << highlightTemplate("Hello< World, Hello< World, Hello< World< Hello, World > > > >", 2) << std::endl;
  std::cout << underline << "colored 'hello world' for all available colors( 0 - 255): " << endunderline << std::endl;
  for (int i = 0; i < 256; ++i)
    std::cout << highlightString("hello world", i) << std::endl;
} // main
