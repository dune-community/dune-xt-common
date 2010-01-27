#include <cstdlib>
#include <iostream>
#include <string>

//#define new(a, rest...) mynew( a, atoi(__LINE__), __FILE__, rest )

// void * mynew( size_t sz, int line, char* file) throw (std::bad_alloc)
void* mynew(size_t sz) throw(std::bad_alloc)
{
  //    std::cerr << "node::new" << std::endl;
  return malloc(sz);
}

//#define new(...) mynew(...)

// void* mynew[](std::size_t) throw (std::bad_alloc);
// void* mynew(std::size_t, const std::nothrow_t&) throw();
// void* mynew[](std::size_t, const std::nothrow_t&) throw();
// void operator delete(void*, const std::nothrow_t&) throw();
// void operator delete[](void*, const std::nothrow_t&) throw();
//
// void operator delete( void *pvMem )
//{
//    free( pvMem );
//}


void* operator new(unsigned long size, const char* file)
{
  std::cout << file << " operator new(" << size << "):\t";

  void* p = malloc(size);
  if (!p)
    throw "operator new() error";

  std::cout << static_cast<void*>(p) << std::endl;

  return p;
}

void* operator new[](unsigned long size, const char* file)
{
  std::cout << file << " operator new(" << size << "):\t";

  void* p = malloc(size);
  if (!p)
    throw "operator new() error";

  //    std::cout << static_cast<void*>(p) << std::endl;

  return p;
}

void operator delete(void* p)
{
  //    std::cout << "operator delete(" << p << ")" << std::endl;

  free(p);
}

void operator delete[](void* p)
{
  //    std::cout << "operator delete[](" << p << ")" << std::endl;

  free(p);
}

#define new (a) new (a, __func__)
