#ifndef DUNE_STUFF_COMMON_FLOAT_CMP_HH
#define DUNE_STUFF_COMMON_FLOAT_CMP_HH

#include <dune/common/float_cmp.hh>
#include <dune/common/dynvector.hh>

#if HAVE_EIGEN
#include <dune/stuff/la/container/eigen.hh>
#endif

namespace Dune {
namespace Stuff {
namespace Common {
namespace FloatCmp {


template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::DynamicVector<T>& first, const Dune::DynamicVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  bool result = true;
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (!Dune::FloatCmp::eq<T, style>(first[ii], second[ii], epsilon))
      result = false;
  return result;
} // ... eq(...)

#if HAVE_EIGEN
template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::Stuff::LA::EigenDenseVector<T>& first, const Dune::Stuff::LA::EigenDenseVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  bool result = true;
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (!Dune::FloatCmp::eq<T, style>(first.backend()[ii], second.backend()[ii], epsilon))
      result = false;
  return result;
} // ... eq(...)

template <class T, Dune::FloatCmp::CmpStyle style = Dune::FloatCmp::defaultCmpStyle>
bool eq(const Dune::DynamicVector<T>& first, const Dune::Stuff::LA::EigenDenseVector<T>& second,
        typename Dune::FloatCmp::EpsilonType<T>::Type epsilon = Dune::FloatCmp::DefaultEpsilon<T, style>::value())
{
  assert(first.size() == second.size());
  bool result = true;
  for (size_t ii = 0; ii < first.size(); ++ii)
    if (!Dune::FloatCmp::eq<T, style>(first[ii], second.backend()[ii], epsilon))
      result = false;
  return result;
} // ... eq(...)
#endif // HAVE_EIGEN

} // namespace FloatCmp
} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_FLOAT_CMP_HH
