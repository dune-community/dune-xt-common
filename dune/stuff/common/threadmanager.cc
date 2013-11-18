#include <config.h>

#include "threadmanager.hh"


#ifdef HAVE_DUNE_FEM

#include <dune/fem/misc/threadmanager.hh>

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

#else // ifdef HAVE_DUNE_FEM

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

#endif // else ifdef HAVE_DUNE_FEM
