#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_HH

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/float_cmp.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/fvector.hh>

#if HAVE_EIGEN
#include <dune/stuff/la/container/eigen.hh>
#endif

#include <type_traits>

namespace Dune {
namespace Stuff {
namespace Common {
namespace FloatCmp {

template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
ne(const T& first, const T& second,
   typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  return Dune::FloatCmp::ne<T, style>(first, second, epsilon);
}

template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
typename std::enable_if<std::is_arithmetic<T>::value, bool>::type
eq(const T& first, const T& second,
   typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  return Dune::FloatCmp::eq<T, style>(first, second, epsilon);
}

template <class T, template <class> class FirstVectorType, template <class> class SecondVectorType = FirstVectorType,
          Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool ne(const FirstVectorType<T>& first, const SecondVectorType<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  return !eq(first, second, epsilon);
}

template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::DynamicVector<T>& first, const Dune::DynamicVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (Dune::FloatCmp::ne<T, style>(first[ii], second[ii], epsilon))
      return false;
  return true;
} // ... eq(...)

template <class T, int size, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::FieldVector<T, size>& first, const Dune::FieldVector<T, size>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  for (size_t ii = 0; ii < size; ++ii)
    if (Dune::FloatCmp::ne<T, style>(first[ii], second[ii], epsilon))
      return false;
  return true;
} // ... eq(...)

#if HAVE_EIGEN
template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::Stuff::LA::EigenDenseVector<T>& first, const Dune::Stuff::LA::EigenDenseVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (!Dune::FloatCmp::eq<T, style>(first.backend()[ii], second.backend()[ii], epsilon))
      return false;
  return true;
} // ... eq(...)

template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::DynamicVector<T>& first, const Dune::Stuff::LA::EigenDenseVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (!Dune::FloatCmp::eq<T, style>(first[ii], second.backend()[ii], epsilon))
      return false;
  return true;
} // ... eq(...)
#endif // HAVE_EIGEN

} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FLOAT_CMP_HH
