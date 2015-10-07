// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_EXCEPTIONS_HH
#define DUNE_STUFF_COMMON_EXCEPTIONS_HH

#include <dune/common/exceptions.hh>
#include <dune/common/parallel/mpihelper.hh>
#include <dune/common/deprecated.hh>

#ifdef DUNE_THROW
#undef DUNE_THROW
#endif

/**
 *  \brief Macro to throw a colorfull exception.
 *
 *  Example:
\code
#include <dune/stuff/common/exceptions.hh>

if (a.size() != b.size())
  DUNE_THROW(Exceptions::shapes_do_not_match,
             "size of a (" << a.size() << ") does not match the size of b (" << b.size() << ")!");
\endcode
 *  This macro is essentially copied from dune-common with added color functionality and rank information.
 *  \param  E Exception class, derived from Dune::Exception.
 *  \param  m Message in ostream notation.
 *  \see Dune::Exception
 */
#define DUNE_THROW(E, m)                                                                                               \
  do {                                                                                                                 \
    const std::string th__red   = "\033[31m";                                                                          \
    const std::string th__brown = "\033[33m";                                                                          \
    const std::string th__clear = "\033[0m";                                                                           \
    E th__ex;                                                                                                          \
    std::ostringstream th__msg;                                                                                        \
    th__msg << m;                                                                                                      \
    std::ostringstream th__out;                                                                                        \
    th__out << th__red << #E << th__clear;                                                                             \
    if (Dune::MPIHelper::getCollectiveCommunication().size() > 1)                                                      \
      th__out << " (on rank " << Dune::MPIHelper::getCollectiveCommunication().rank() << ")";                          \
    th__out << "\n";                                                                                                   \
    th__out << th__brown << "[" << th__clear;                                                                          \
    th__out << th__red << __func__ << th__clear;                                                                       \
    th__out << th__brown << "|" << th__clear;                                                                          \
    th__out << __FILE__ << th__brown << ":" << th__clear << th__red << __LINE__ << th__clear << th__brown << "]"       \
            << th__clear;                                                                                              \
    if (!th__msg.str().empty())                                                                                        \
      th__out << "\n" << th__brown << "=>" << th__clear << " " << th__msg.str();                                       \
    th__ex.message(th__out.str());                                                                                     \
    throw th__ex;                                                                                                      \
  } while (0)
// DUNE_THROW

namespace Dune {
namespace Stuff {
namespace Exceptions {

class CRTP_check_failed : public Dune::Exception
{
};

class shapes_do_not_match : public Dune::Exception
{
};

class index_out_of_range : public Dune::Exception
{
};

class you_are_using_this_wrong : public Dune::Exception
{
};

class wrong_input_given : public you_are_using_this_wrong
{
};

class requirements_not_met : public you_are_using_this_wrong
{
};

class configuration_error : public Dune::Exception
{
};

class conversion_error : public Dune::Exception
{
};

class results_are_not_as_expected : public Dune::Exception
{
};

class internal_error : public Dune::Exception
{
};

class external_error : public Dune::Exception
{
};

class linear_solver_failed : public Dune::Exception
{
};

class you_have_to_implement_this : public Dune::NotImplemented
{
};

} // namespace Exceptions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_EXCEPTIONS_HH
