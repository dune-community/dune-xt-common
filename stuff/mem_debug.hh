#include <cstdlib>
#include <iostream>
#include <string>

/** super fun file indeed (read: include at your own risk)
  *
  */

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

void operator delete(void* p) throw()
{
  //    std::cout << "operator delete(" << p << ")" << std::endl;

  free(p);
}

void operator delete[](void* p) throw()
{
  //    std::cout << "operator delete[](" << p << ")" << std::endl;

  free(p);
}

#define new (a) new (a, __func__)
/** Copyright (c) 2012, Rene Milk
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
