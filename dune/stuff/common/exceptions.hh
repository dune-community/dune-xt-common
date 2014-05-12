// This file is part of the dune-stuff project:
//   https://users.dune-project.org/projects/dune-stuff/
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_COMMON_EXCEPTIONS_HH
#define DUNE_STUFF_COMMON_EXCEPTIONS_HH

#include <dune/common/exceptions.hh>

#include <dune/stuff/common/color.hh>

/**
 *  \brief Macro to throw a colorfull exception.
 *
 *  Example:
\code
#include <dune/stuff/common/exceptions.hh>

if (a.size() != b.size())
  DUNE_THROW_COLORFULLY(Exceptions::shapes_do_not_match,
                       "size of a (" << a.size() << ") does not match the size of b (" << b.size() << ")!");
\endcode
 *  This macro is essentially copied from dune-common with added color functionality.
 *  \param  E Exception class, derived from Dune::Exception.
 *  \param  m Message in ostream notation.
 *  \see DUNE_THROW, Dune::Exception
 */
#define DUNE_THROW_COLORFULLY(E, m)                                                                                    \
  do {                                                                                                                 \
    E th__ex;                                                                                                          \
    std::ostringstream th__msg;                                                                                        \
    th__msg << m;                                                                                                      \
    std::ostringstream th__out;                                                                                        \
    th__out << Dune::Stuff::Common::Colors::red << #E << "\033[0m\n";                                                  \
    th__out << Dune::Stuff::Common::Colors::red << "[\033[0m";                                                         \
    th__out << __func__;                                                                                               \
    th__out << Dune::Stuff::Common::Colors::red << "|\033[0m";                                                         \
    th__out << __FILE__ << Dune::Stuff::Common::Colors::red << ":" << __LINE__ << "]\033[0m";                          \
    if (!th__msg.str().empty())                                                                                        \
      th__out << "\n" << Dune::Stuff::Common::Colors::red << "=>\033[0m " << th__msg.str();                            \
    th__ex.message(th__out.str());                                                                                     \
    throw th__ex;                                                                                                      \
  } while (0)
// DUNE_THROW_COLORFULLY

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


} // namespace Exceptions
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_EXCEPTIONS_HH
