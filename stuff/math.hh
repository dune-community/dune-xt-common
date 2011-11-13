#ifndef DUNE_STUFF_MATH_HH
#define DUNE_STUFF_MATH_HH

#include <vector>
#include <limits>
#include <algorithm>
#include <cstring>
#include <iostream>
#include "static_assert.hh"
#include <boost/static_assert.hpp>
#include <boost/fusion/include/void.hpp>
#include <boost/format.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/mean.hpp>

#include <dune/common/deprecated.hh>

namespace Stuff {

/** \todo DOCME **/
template <class SomeRangeType, class OtherRangeType>
static double colonProduct(const SomeRangeType& arg1, const OtherRangeType& arg2)
{
  dune_static_assert(SomeRangeType::cols == SomeRangeType::rows && OtherRangeType::cols == OtherRangeType::rows
                         && int(OtherRangeType::cols) == int(SomeRangeType::rows),
                     "RangeTypes_dont_fit");

  double ret = 0.0;
  // iterators
  typedef typename SomeRangeType::ConstRowIterator ConstRowIteratorType;
  typedef typename SomeRangeType::row_type::ConstIterator ConstIteratorType;
  ConstRowIteratorType arg1RowItEnd = arg1.end();
  ConstRowIteratorType arg2RowItEnd = arg2.end();
  ConstRowIteratorType arg2RowIt = arg2.begin();
  for (ConstRowIteratorType arg1RowIt = arg1.begin(); arg1RowIt != arg1RowItEnd, arg2RowIt != arg2RowItEnd;
       ++arg1RowIt, ++arg2RowIt) {
    ConstIteratorType row1ItEnd = arg1RowIt->end();
    ConstIteratorType row2ItEnd = arg2RowIt->end();
    ConstIteratorType row2It = arg2RowIt->begin();
    for (ConstIteratorType row1It = arg1RowIt->begin(); row1It != row1ItEnd, row2It != row2ItEnd; ++row1It, ++row2It) {
      ret += *row1It * *row2It;
    }
  }
  return ret;
}

/**
 *  \brief  dyadic product
 *
 *          Implements \f$\left(arg_{1} \otimes arg_{2}\right)_{i,j}:={arg_{1}}_{i} {arg_{2}}_{j}\f$
 *			RangeType1 should be fieldmatrix, RangeType2 fieldvector
 **/
template <class RangeType1, class RangeType2>
static RangeType1 dyadicProduct(const RangeType2& arg1, const RangeType2& arg2)
{
  RangeType1 ret(0.0);
  typedef typename RangeType1::RowIterator MatrixRowIteratorType;
  typedef typename RangeType2::ConstIterator ConstVectorIteratorType;
  typedef typename RangeType2::Iterator VectorIteratorType;
  MatrixRowIteratorType rItEnd   = ret.end();
  ConstVectorIteratorType arg1It = arg1.begin();
  for (MatrixRowIteratorType rIt = ret.begin(); rIt != rItEnd; ++rIt) {
    ConstVectorIteratorType arg2It = arg2.begin();
    VectorIteratorType vItEnd = rIt->end();
    for (VectorIteratorType vIt = rIt->begin(); vIt != vItEnd; ++vIt) {
      *vIt = *arg1It * *arg2It;
      ++arg2It;
    }
    ++arg1It;
  }
  return ret;
}

/** \brief a vector wrapper for continiously updating min,max,avg of some element type vector
  \todo find use? it's only used in minimal as testcase for itself...
  **/
template <class ElementType>
class MinMaxAvg
{
protected:
  typedef MinMaxAvg<ElementType> ThisType;
  typedef std::vector<ElementType> ElementsVec;
  typedef typename ElementsVec::const_iterator ElementsVecConstIterator;

public:
  MinMaxAvg()
  {
  }

  template <class stl_container_type>
  MinMaxAvg(const stl_container_type& elements)
  {
    dune_static_assert((boost::is_same<ElementType, typename stl_container_type::value_type>::value),
                       "cannot assign mismatching types");
    acc_ = std::for_each(elements.begin(), elements.end(), acc_);
  }

  ElementType min() const
  {
    return boost::accumulators::min(acc_);
  }
  ElementType max() const
  {
    return boost::accumulators::max(acc_);
  }
  ElementType average() const
  {
    return boost::accumulators::mean(acc_);
  }

  void DUNE_DEPRECATED push(const ElementType& el)
  {
    acc_(el);
  }

  void operator()(const ElementType& el)
  {
    acc_(el);
  }

  template <class Stream>
  void output(Stream& stream)
  {
    stream << boost::format("min: %e\tmax: %e\tavg: %e\n") % min() % max() % average();
  }

protected:
  typedef boost::accumulators::stats<boost::accumulators::tag::max, boost::accumulators::tag::min,
                                     boost::accumulators::tag::mean> StatsType;
  boost::accumulators::accumulator_set<ElementType, StatsType> acc_;

  MinMaxAvg(const ThisType& other);
};

//! bound \param var in [\param min,\param max]
template <typename T>
T clamp(const T var, const T min, const T max)
{
  return ((var < min) ? min : (var > max) ? max : var);
}

//! docme
class MovingAverage
{
  double avg_;
  size_t steps_;

public:
  MovingAverage()
    : avg_(0.0)
    , steps_(0)
  {
  }
  MovingAverage& operator+=(double val)
  {
    avg_ += (val - avg_) / ++steps_;
    return *this;
  }
  operator double()
  {
    return avg_;
  }
};

//! no-branch sign function
long sign(long x)
{
  return long(x != 0) | (long(x >= 0) - 1);
}

} // end namespace Stuff

#endif // DUNE_STUFF_MATH_HH
