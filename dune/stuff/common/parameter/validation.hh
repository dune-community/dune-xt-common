#ifndef DUNE_STUFF_VALIDATION_HH
#define DUNE_STUFF_VALIDATION_HH

#include <ostream>
#include <string>
#include <boost/format.hpp>

#include <dune/stuff/common/misc.hh>
#include <dune/stuff/common/string.hh>
#include <dune/stuff/common/type_utils.hh>

namespace Dune {
namespace Stuff {
namespace Common {


/** \brief Base class for all Validators
 *  the idea is from dune-fem, only our class is an actual interface
 **/
template <class T, class Derived>
class ValidatorInterface
{
public:
  bool operator()(const T& value) const
  {
    return asImp()(value);
  }

  std::string msg() const
  {
    return asImp().msg();
  }
  void print(std::ostream& out) const
  {
    out << asImp().msg();
  }

protected:
  inline const Derived& asImp() const
  {
    return static_cast<const Derived&>(*this);
  }

  inline Derived& asImp()
  {
    return static_cast<Derived&>(*this);
  }
};

//! a class usable as a default validator
template <class T>
class ValidateAny : public ValidatorInterface<T, ValidateAny<T>>
{
  typedef ValidateAny<T> ThisType;
  typedef ValidatorInterface<T, ThisType> BaseType;

public:
  inline ValidateAny()
  {
  }
  inline ValidateAny(const ThisType&)
  {
  }

  inline bool operator()(const T&) const
  {
    return true;
  }

  std::string msg() const
  {
    return "ValidateAny: all values should be valid... \n\n";
  }
};


//! validates arg iff in given list
template <class T, class ListImp = std::vector<T>>
class ValidateInList : public ValidatorInterface<T, ValidateInList<T, ListImp>>
{
  typedef ValidateInList<T, ListImp> ThisType;
  typedef ValidatorInterface<T, ThisType> BaseType;
  typedef ListImp ListType;
  ListType valid_list_;

public:
  ValidateInList(const ListType& valid_list)
    : valid_list_(valid_list)
  {
  }

  inline bool operator()(const T& val) const
  {
    return std::find(valid_list_.begin(), valid_list_.end(), val) != valid_list_.end();
  }

  std::string msg() const
  {
    return "ValidateInList: checked Parameter was not in valid list\n\n";
  }
};

//! validate arg iff less than value, obviously
template <class T>
class ValidateLess : public ValidatorInterface<T, ValidateLess<T>>
{
public:
  ValidateLess(const T& baseval)
    : baseval_(baseval)
  {
  }
  inline bool operator()(const T& val) const
  {
    return baseval_ < val;
  }

  std::string msg() const
  {
    return (boost::format("given value was invalid: not less than %s") % toString(baseval_)).str();
  }

private:
  const T baseval_;
};

//! validate arg iff greater than value, obviously
template <class T>
class ValidateGreater : public ValidatorInterface<T, ValidateGreater<T>>
{
public:
  ValidateGreater(const T& baseval)
    : baseval_(baseval)
  {
  }
  inline bool operator()(const T& val) const
  {
    return baseval_ > val;
  }

  std::string msg() const
  {
    return (boost::format("given value was invalid: not greater than %s") % toString(baseval_)).str();
  }

private:
  const T baseval_;
};

//! validate iff not Validator(arg)
template <class T, class Validator>
class ValidateInverse : public ValidatorInterface<T, ValidateInverse<T, Validator>>
{
public:
  ValidateInverse(const Validator validator = Validator())
    : validator_(validator)
  {
  }
  ValidateInverse(const T arg)
    : validator_(Validator(arg))
  {
  }
  inline bool operator()(const T& val) const
  {
    return !validator_(val);
  }

  std::string msg() const
  {
    return "Inverse condition failed: " + validator_.msg();
  }

private:
  const Validator validator_;
};

#define INVERSE_VALIDATE(V_NEW_NAME, V_BASE_NAME)                                                                      \
  template <class T>                                                                                                   \
  struct V_NEW_NAME : public ValidateInverse<T, V_BASE_NAME<T>>                                                        \
  {                                                                                                                    \
    template <typename... Types>                                                                                       \
    V_NEW_NAME(Types... args)                                                                                          \
      : ValidateInverse<T, V_BASE_NAME<T>>(args...)                                                                    \
    {                                                                                                                  \
    }                                                                                                                  \
  }

INVERSE_VALIDATE(ValidateNone, ValidateAny);
INVERSE_VALIDATE(ValidateGreaterOrEqual, ValidateLess);

//! validate arg iff arg \in [min,max]
template <class T>
class ValidateInterval : public ValidatorInterface<T, ValidateInterval<T>>
{
public:
  ValidateInterval(const T& min, const T& max)
    : min_(min)
    , max_(max)
  {
  }

  inline bool operator()(const T& val) const
  {
    const bool lowerOk = ValidateGreaterOrEqual<T>(val)(min_);
    const bool upperOk = ValidateGreaterOrEqual<T>(max_)(val);
    return lowerOk && upperOk;
  }

  std::string msg() const
  {
    return "given value was invalid: value not in given interval";
  }

private:
  const T min_;
  const T max_;
};

//! example partial specialisation
template <typename T>
struct Typename<ValidateAny<T>>
{
  static std::string value()
  {
    return "Dune::Stuff::Common::Parameter::ValidateAny<T>";
  }
};

} // end namesapce Common
} // end namesapce Stuff
} // end namesapce Dune

template <class T, class Validator>
std::ostream operator<<(std::ostream& out, const Dune::Stuff::Common::ValidatorInterface<T, Validator>& validator)
{
  return out << validator.msg();
}

#endif // DUNE_STUFF_VALIDATION_HH
