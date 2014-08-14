// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#include "config.h"

#include "threadmanager.hh"

#if HAVE_DUNE_FEM

#include <dune/fem/misc/threads/threadmanager.hh>

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  return Dune::Fem::ThreadManager::maxThreads();
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  return Dune::Fem::ThreadManager::currentThreads();
}

unsigned int Dune::Stuff::ThreadManager::thread()
{
  return Dune::Fem::ThreadManager::thread();
}

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int count)
{
  Dune::Fem::ThreadManager::setMaxNumberThreads(count);
}

#else // if HAVE_DUNE_FEM

unsigned int Dune::Stuff::ThreadManager::max_threads()
{
  return 1;
}

unsigned int Dune::Stuff::ThreadManager::current_threads()
{
  return 1;
}

unsigned int Dune::Stuff::ThreadManager::thread()
{
  return 1;
}

void Dune::Stuff::ThreadManager::set_max_threads(const unsigned int /*count*/)
{
}

#endif // HAVE_DUNE_FEM
