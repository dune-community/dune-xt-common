// This file is part of the dune-stuff project:
//   https://github.com/wwu-numerik/dune-stuff
// Copyright holders: Rene Milk, Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

/**
   *  \file   string.hh
   *  \brief  contains methods to read/write scalar types and container types from/to strings and methods to
   *  manipulate strings
   **/
#ifndef DUNE_STUFF_COMMON_STRING_HH
#define DUNE_STUFF_COMMON_STRING_HH

#include <cstring>
#include <ctime>
#include <map>
#include <assert.h>
#include <algorithm>
#include <ostream>
#include <iomanip>
#include <vector>
#include <string>
#include <ctime>
#include <iostream>

#include <dune/common/array.hh>
#include <dune/common/deprecated.hh>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <dune/stuff/common/disable_warnings.hh>
#include <dune/common/fmatrix.hh>
#include <dune/common/densematrix.hh>
#include <dune/common/fvector.hh>
#include <dune/common/dynmatrix.hh>
#include <dune/stuff/common/reenable_warnings.hh>
#include <dune/common/dynvector.hh>
#include <dune/common/densevector.hh>

#include <dune/stuff/common/exceptions.hh>
#include <dune/stuff/common/fvector.hh>
#include <dune/stuff/common/fmatrix.hh>
#include <dune/stuff/common/debug.hh>
#include <dune/stuff/la/container/interfaces.hh>
#include <dune/stuff/la/container/common.hh>
#include <dune/stuff/la/container/eigen.hh>
#include <dune/stuff/la/container/istl.hh>

namespace Dune {
namespace Stuff {
namespace Common {

/* forward to use tokenize in the get_matrix_from_string and get_vector_from_string methods below (only
tokenize<std::string>
is used, which does not depend on the get_*_from_string methods, so no cyclic dependency here). */
template <class T = std::string>
inline std::vector<T>
tokenize(const std::string& msg, const std::string& separators,
         const boost::algorithm::token_compress_mode_type mode = boost::algorithm::token_compress_off);


namespace internal {


/* forward to use Choose< S >::fromString in convert_from_string_safely method below. As Choose< S >::fromString does
not depend on convert_from_string_safely for scalar types, there should be no endless recursion */
template <class ReturnType>
class Choose;

/* we want to make fromString< S > work for every scalar or container type S, but we cannot simply use a template,
because the container types are also templated, so we need this class to choose the appropriate fromString */
template <class T>
class ChooseBase
{
  template <class M>
  struct MatrixConstructor
  {
    static M create(const size_t rows, const size_t cols)
    {
      return M(rows, cols);
    }
  };

  template <class K, int r, int c>
  struct MatrixConstructor<Dune::FieldMatrix<K, r, c>>
  {
    static Dune::FieldMatrix<K, r, c> create(const size_t /*rows*/, const size_t /*cols*/)
    {
      return Dune::FieldMatrix<K, r, c>();
    }
  };

  template <class M>
  struct MatrixSetter
  {
    template <class V>
    static void set_entry(M& mat, const size_t rr, const size_t cc, const V& val)
    {
      mat[rr][cc] = val;
    }
  };

  template <class S>
  struct MatrixSetter<Stuff::LA::CommonDenseMatrix<S>>
  {
    template <class V>
    static void set_entry(Stuff::LA::CommonDenseMatrix<S>& mat, const size_t rr, const size_t cc, const V& val)
    {
      mat.set_entry(rr, cc, val);
    }
  };

#if HAVE_EIGEN
  template <class S>
  struct MatrixSetter<Stuff::LA::EigenDenseMatrix<S>>
  {
    template <class V>
    static void set_entry(Stuff::LA::EigenDenseMatrix<S>& mat, const size_t rr, const size_t cc, const V& val)
    {
      mat.set_entry(rr, cc, val);
    }
  };
#endif // HAVE_EIGEN

public:
  template <class M>
  struct MatrixGetter
  {
    template <class V>
    static V get_entry(const M& mat, const size_t rr, const size_t cc)
    {
      return mat[rr][cc];
    }
  };

  template <class S>
  struct MatrixGetter<Stuff::LA::CommonDenseMatrix<S>>
  {
    template <class V>
    static V get_entry(const Stuff::LA::CommonDenseMatrix<S>& mat, const size_t rr, const size_t cc)
    {
      return mat.get_entry(rr, cc);
    }
  };

#if HAVE_EIGEN
  template <class S>
  struct MatrixGetter<Stuff::LA::EigenDenseMatrix<S>>
  {
    template <class V>
    static V get_entry(const Stuff::LA::EigenDenseMatrix<S>& mat, const size_t rr, const size_t cc)
    {
      return mat.get_entry(rr, cc);
    }
  };
#endif // HAVE_EIGEN

private:
  static std::string trim_copy_safely(const std::string& str_in)
  {
    const std::string str_out = boost::algorithm::trim_copy(str_in);
    if (str_out.find(";") != std::string::npos)
      DUNE_THROW(Exceptions::configuration_error,
                 "There was an error while parsing the string below. "
                     << "The value contained a ';': '"
                     << str_out
                     << "'!\n"
                     << "This usually happens if you try to get a matrix expression with a vector type "
                     << "or if you are missing the white space after the ';' in a matrix expression!\n");
    return str_out;
  } // ... trim_copy_safely(...)

  template <class S>
  static S convert_from_string_safely(const std::string& str_in)
  {
    try {
      return Choose<S>::fromString(str_in);
    } catch (boost::bad_lexical_cast& e) {
      DUNE_THROW(Exceptions::external_error,
                 "Error in boost while converting the string '" << str_in << "' to type '" << Typename<T>::value()
                                                                << "':\n"
                                                                << e.what());
    } catch (std::exception& e) {
      DUNE_THROW(Exceptions::external_error,
                 "Error in the stl while converting the string '" << str_in << "' to type '" << Typename<T>::value()
                                                                  << "':\n"
                                                                  << e.what());
    }
  } // ... convert_from_string_safely(...)

protected:
  template <class MatrixType, class S>
  static MatrixType get_matrix_from_string(std::string matrix_str, const size_t rows, const size_t cols)
  {
    // check if this is a matrix
    if (matrix_str.substr(0, 1) == "[" && matrix_str.substr(matrix_str.size() - 1, 1) == "]") {
      matrix_str = matrix_str.substr(1, matrix_str.size() - 2);
      // we treat this as a matrix and split along ';' to obtain the rows
      const auto row_tokens = tokenize<std::string>(matrix_str, ";", boost::algorithm::token_compress_on);
      if (rows > 0 && row_tokens.size() < rows)
        DUNE_THROW(Exceptions::configuration_error,
                   "Matrix (see below) has only " << row_tokens.size() << " rows but " << rows
                                                  << " rows were requested!"
                                                  << "\n"
                                                  << "'["
                                                  << matrix_str
                                                  << "]'");
      const size_t actual_rows = (rows > 0) ? std::min(row_tokens.size(), rows) : row_tokens.size();
      // compute the number of columns the matrix will have
      size_t min_cols = std::numeric_limits<size_t>::max();
      for (size_t rr = 0; rr < actual_rows; ++rr) {
        const auto row_token = boost::algorithm::trim_copy(row_tokens[rr]);
        // we treat this as a vector, so we split along ' '
        const auto column_tokens = tokenize<std::string>(row_token, " ", boost::algorithm::token_compress_on);
        min_cols                 = std::min(min_cols, column_tokens.size());
      }
      if (cols > 0 && min_cols < cols)
        DUNE_THROW(Exceptions::configuration_error,
                   "Matrix (see below) has only " << min_cols << " columns but " << cols << " columns were requested!"
                                                  << "\n"
                                                  << "'["
                                                  << matrix_str
                                                  << "]'");
      const size_t actual_cols = (cols > 0) ? std::min(min_cols, cols) : min_cols;
      MatrixType ret           = MatrixConstructor<MatrixType>::create(actual_rows, actual_cols);
      // now we do the same again and build the actual matrix
      for (size_t rr = 0; rr < actual_rows; ++rr) {
        const std::string row_token = boost::algorithm::trim_copy(row_tokens[rr]);
        const auto column_tokens = tokenize<std::string>(row_token, " ", boost::algorithm::token_compress_on);
        for (size_t cc = 0; cc < actual_cols; ++cc)
          MatrixSetter<MatrixType>::set_entry(
              ret, rr, cc, convert_from_string_safely<S>(trim_copy_safely(column_tokens[cc])));
      }
      return ret;
    } else {
      // we treat this as a scalar
      const S val              = convert_from_string_safely<S>(trim_copy_safely(matrix_str));
      const size_t actual_rows = (rows == 0 ? 1 : rows);
      const size_t actual_cols = (cols == 0 ? 1 : cols);
      MatrixType ret = MatrixConstructor<MatrixType>::create(actual_rows, actual_cols);
      for (size_t rr = 0; rr < actual_rows; ++rr)
        for (size_t cc = 0; cc < actual_cols; ++cc)
          MatrixSetter<MatrixType>::set_entry(ret, rr, cc, val);
      return ret;
    }
  } // ... get_matrix_from_string(...)

  template <class VectorType, class S>
  static VectorType get_vector_from_string(std::string vector_str, const size_t size)
  {
    // check if this is a vector
    if (vector_str.substr(0, 1) == "[" && vector_str.substr(vector_str.size() - 1, 1) == "]") {
      vector_str = vector_str.substr(1, vector_str.size() - 2);
      // we treat this as a vector and split along ' '
      const auto tokens = tokenize<std::string>(vector_str, " ", boost::algorithm::token_compress_on);
      if (size > 0 && tokens.size() < size)
        DUNE_THROW(Exceptions::configuration_error,
                   "Vector (see below) has only " << tokens.size() << " elements but " << size
                                                  << " elements were requested!"
                                                  << "\n"
                                                  << "'["
                                                  << vector_str
                                                  << "]'");
      const size_t actual_size = (size > 0) ? std::min(tokens.size(), size) : tokens.size();
      VectorType ret(actual_size);
      for (size_t ii = 0; ii < actual_size; ++ii)
        ret[ii] = convert_from_string_safely<S>(trim_copy_safely(tokens[ii]));
      return ret;
    } else {
      // we treat this as a scalar
      const S val              = convert_from_string_safely<S>(trim_copy_safely(vector_str));
      const size_t actual_size = (size == 0 ? 1 : size);
      VectorType ret(actual_size);
      for (size_t ii = 0; ii < actual_size; ++ii)
        ret[ii] = val;
      return ret;
    }
  } // ... get_vector_from_string(...)
}; // class ChooseBase


//! simple and dumb std::string to anything conversion
template <class ReturnType>
class Choose : ChooseBase<ReturnType>
{
public:
  static inline ReturnType fromString(const std::string s, const size_t UNUSED_UNLESS_DEBUG(rows) = 0,
                                      const size_t UNUSED_UNLESS_DEBUG(cols) = 0)
  {
    assert(rows == 0);
    assert(cols == 0);
    try {
      return boost::lexical_cast<ReturnType, std::string>(s);
    } catch (boost::bad_lexical_cast& e) {
      DUNE_THROW(Exceptions::external_error,
                 "Error in boost while converting the string '" << s << "' to type '" << Typename<ReturnType>::value()
                                                                << "':\n"
                                                                << e.what());
    } catch (std::exception& e) {
      DUNE_THROW(Exceptions::external_error,
                 "Error in the stl while converting the string '" << s << "' to type '" << Typename<ReturnType>::value()
                                                                  << "':\n"
                                                                  << e.what());
    }
  }
}; // ... Choose < ReturnType >

/**
 * \brief Convert std::string to char.
 * \return String converted to char, if string.size() == 1. Else try to convert string to int and return char(int).
 */
template <>
class Choose<char> : ChooseBase<char>
{
public:
  static inline char fromString(const std::string s, const size_t UNUSED_UNLESS_DEBUG(rows) = 0,
                                const size_t UNUSED_UNLESS_DEBUG(cols) = 0)
  {
    assert(rows == 0);
    assert(cols == 0);
    if (s.size() == 1)
      return s[0];
    else {
      try {
        return char(stoi(s));
      } catch (std::invalid_argument& e) {
        DUNE_THROW(Stuff::Exceptions::wrong_input_given,
                   "Error in the stl while converting the string '" << s << "' to type '" << Typename<char>::value()
                                                                    << "':\n"
                                                                    << e.what());
      }
    }
  } // ... fromString(...)
}; // class Choose < char >

template <>
class Choose<const char*> : ChooseBase<const char*>
{
public:
  static inline const char* fromString(const std::string s, const size_t UNUSED_UNLESS_DEBUG(rows) = 0,
                                       const size_t UNUSED_UNLESS_DEBUG(cols) = 0)
  {
    assert(rows == 0);
    assert(cols == 0);
    return s.c_str();
  } // ... fromString(...)
}; // class Choose < const char * >

//! get numerical types from string, using std::sto*
#define DSC_FRSTR(tn, tns)                                                                                             \
  template <>                                                                                                          \
  class Choose<tn> : ChooseBase<tn>                                                                                    \
  {                                                                                                                    \
  public:                                                                                                              \
    static inline tn fromString(const std::string s, const size_t UNUSED_UNLESS_DEBUG(rows) = 0,                       \
                                const size_t UNUSED_UNLESS_DEBUG(cols) = 0)                                            \
    {                                                                                                                  \
      assert(rows == 0);                                                                                               \
      assert(cols == 0);                                                                                               \
      return std::sto##tns(s);                                                                                         \
    }                                                                                                                  \
  };

DSC_FRSTR(int, i)
DSC_FRSTR(long, l)
DSC_FRSTR(long long, ll)
DSC_FRSTR(unsigned long, ul)
DSC_FRSTR(unsigned long long, ull)
DSC_FRSTR(float, f)
DSC_FRSTR(double, d)
DSC_FRSTR(long double, ld)

#undef DSC_FRSTR

//! fromString for all matrix types that are only templated by their value type
#define DSC_MATRIXFRSTR(MatrixType)                                                                                    \
  template <class S>                                                                                                   \
  class Choose<MatrixType<S>> : ChooseBase<MatrixType<S>>                                                              \
  {                                                                                                                    \
    typedef ChooseBase<MatrixType<S>> BaseType;                                                                        \
                                                                                                                       \
  public:                                                                                                              \
    static inline MatrixType<S> fromString(const std::string s, const size_t rows = 0, const size_t cols = 0)          \
    {                                                                                                                  \
      return BaseType::template get_matrix_from_string<MatrixType<S>, S>(s, rows, cols);                               \
    }                                                                                                                  \
  };

DSC_MATRIXFRSTR(DynamicMatrix)
DSC_MATRIXFRSTR(LA::CommonDenseMatrix)
#if HAVE_EIGEN
DSC_MATRIXFRSTR(LA::EigenDenseMatrix)
#endif

#undef DSC_MATRIXFRSTR

//! fromString for matrix types where the number of rows and columns is also templated
#define DSC_FIELDMATRIXFRSTR(FieldMatrixType)                                                                          \
  template <class S, int ROWS, int COLS>                                                                               \
  class Choose<FieldMatrixType<S, ROWS, COLS>> : ChooseBase<FieldMatrixType<S, ROWS, COLS>>                            \
  {                                                                                                                    \
    typedef ChooseBase<FieldMatrixType<S, ROWS, COLS>> BaseType;                                                       \
                                                                                                                       \
  public:                                                                                                              \
    static inline FieldMatrixType<S, ROWS, COLS> fromString(const std::string s, const size_t rows = 0,                \
                                                            const size_t cols = 0)                                     \
    {                                                                                                                  \
      if ((rows > 0 && rows != ROWS) || (cols > 0 && cols != COLS))                                                    \
        DUNE_THROW(Exceptions::configuration_error,                                                                    \
                   "You requested a '" << getTypename(FieldMatrixType<S, ROWS, COLS>()) << "' with a size of " << rows \
                                       << "x"                                                                          \
                                       << cols                                                                         \
                                       << " but this type of matrix can not have any size other than "                 \
                                       << ROWS                                                                         \
                                       << "x"                                                                          \
                                       << COLS                                                                         \
                                       << "!");                                                                        \
      return BaseType::template get_matrix_from_string<FieldMatrixType<S, ROWS, COLS>, S>(s, ROWS, COLS);              \
    }                                                                                                                  \
  };

DSC_FIELDMATRIXFRSTR(Dune::Stuff::Common::FieldMatrix)
DSC_FIELDMATRIXFRSTR(Dune::FieldMatrix)

#undef DSC_FIELDMATRIXFRSTR

//! fromString for all vector types that are only templated by their value type
#define DSC_VECTORFRSTR(VectorType)                                                                                    \
  template <class S>                                                                                                   \
  class Choose<VectorType<S>> : ChooseBase<VectorType<S>>                                                              \
  {                                                                                                                    \
    typedef ChooseBase<VectorType<S>> BaseType;                                                                        \
                                                                                                                       \
  public:                                                                                                              \
    static inline VectorType<S> fromString(const std::string s, const size_t size = 0,                                 \
                                           const size_t UNUSED_UNLESS_DEBUG(cols) = 0)                                 \
    {                                                                                                                  \
      assert(cols == 0);                                                                                               \
      return BaseType::template get_vector_from_string<VectorType<S>, S>(s, size);                                     \
    }                                                                                                                  \
  };

DSC_VECTORFRSTR(std::vector)
DSC_VECTORFRSTR(LA::CommonDenseVector)
DSC_VECTORFRSTR(DynamicVector)
#if HAVE_EIGEN
DSC_VECTORFRSTR(LA::EigenDenseVector)
DSC_VECTORFRSTR(LA::EigenMappedDenseVector)
#endif
#if HAVE_DUNE_ISTL
DSC_VECTORFRSTR(LA::IstlDenseVector)
#endif

#undef DSC_VECTORFRSTR

//! fromString for vector types where the size is also templated
#define DSC_FIELDVECTORFRSTR(FieldVectorType)                                                                          \
  template <class S, int SIZE>                                                                                         \
  class Choose<FieldVectorType<S, SIZE>> : ChooseBase<FieldVectorType<S, SIZE>>                                        \
  {                                                                                                                    \
    typedef ChooseBase<FieldVectorType<S, SIZE>> BaseType;                                                             \
                                                                                                                       \
  public:                                                                                                              \
    static inline FieldVectorType<S, SIZE> fromString(const std::string s, const size_t size = 0,                      \
                                                      const size_t UNUSED_UNLESS_DEBUG(cols) = 0)                      \
    {                                                                                                                  \
      assert(cols == 0);                                                                                               \
      if (size > 0 && size != SIZE)                                                                                    \
        DUNE_THROW(Exceptions::configuration_error,                                                                    \
                   "You requested a '" /*<< Typename< VectorType >::value() <<*/ "' with a 'size' of "                 \
                       << size                                                                                         \
                       << " but this type of vector can not have any size other than "                                 \
                       << SIZE                                                                                         \
                       << "!");                                                                                        \
      return BaseType::template get_vector_from_string<FieldVectorType<S, SIZE>, S>(s, SIZE);                          \
    }                                                                                                                  \
  };

DSC_FIELDVECTORFRSTR(Dune::FieldVector)
DSC_FIELDVECTORFRSTR(Dune::Stuff::Common::FieldVector)

#undef DSC_FIELDVECTORFRSTR


} // namespace internal


/** \brief Read an object of type ReturnType from a string
 *  \param s string to read from
 *  \param size Determines the size of the returning container (size if ReturnType is a vector type, rows if ReturnType
 *  is a matrix type, ignored if ReturnType is a scalar type, 0 means automatic).
 *  \param cols Determines the number of columns of the returning matrix if ReturnType is a matrix type (0 means
 *  automatic, ignored if ReturnType is a vector or scalar type).
 */
template <class ReturnType>
ReturnType fromString(const std::string s, const size_t size = 0, const size_t cols = 0)
{
  return internal::Choose<ReturnType>::fromString(s, size, cols);
} // class fromString(...)


/* for toString, the template arguments can be inferred from the function argument, so we do not need a special class to
choose the right toString method but can just use templates */
//! simple and dumb anything (numerical values) to std::string conversion
template <class InType>
inline std::string toString(const InType& s)
{
  return std::to_string(s);
} // toString

//! const char* to string conversion
inline std::string toString(const char* s)
{
  return std::string(s);
} // toString

//! char to string conversion
inline std::string toString(char s)
{
  return std::string(1, s);
} // toString

//! simply return s
inline std::string toString(const std::string s)
{
  return std::string(s);
} // toString

//! toString for all vector types that are only templated by their value type
#define DSC_VECTORTOSTR(VectorType)                                                                                    \
  template <class S>                                                                                                   \
  inline std::string toString(const VectorType<S> vector)                                                              \
  {                                                                                                                    \
    std::string str = "[";                                                                                             \
    for (size_t ii = 0; ii < vector.size(); ++ii) {                                                                    \
      if (ii > 0)                                                                                                      \
        str += " ";                                                                                                    \
      str += toString(vector[ii]);                                                                                     \
    }                                                                                                                  \
    str += "]";                                                                                                        \
    return str;                                                                                                        \
  }

DSC_VECTORTOSTR(std::vector)
DSC_VECTORTOSTR(LA::CommonDenseVector)
DSC_VECTORTOSTR(DynamicVector)
#if HAVE_EIGEN
DSC_VECTORTOSTR(LA::EigenDenseVector)
DSC_VECTORTOSTR(LA::EigenMappedDenseVector)
#endif
#if HAVE_DUNE_ISTL
DSC_VECTORTOSTR(LA::IstlDenseVector)
#endif

#undef DSC_VECTORTOSTR

//! toString for vector types where the size is also templated
#define DSC_FIELDVECTORTOSTR(FieldVectorType)                                                                          \
  template <class S, int SIZE>                                                                                         \
  inline std::string toString(const FieldVectorType<S, SIZE> vector)                                                   \
  {                                                                                                                    \
    std::string str = "[";                                                                                             \
    for (size_t ii = 0; ii < SIZE; ++ii) {                                                                             \
      if (ii > 0)                                                                                                      \
        str += " ";                                                                                                    \
      str += toString(vector[ii]);                                                                                     \
    }                                                                                                                  \
    str += "]";                                                                                                        \
    return str;                                                                                                        \
  }

DSC_FIELDVECTORTOSTR(Dune::Stuff::Common::FieldVector)
DSC_FIELDVECTORTOSTR(Dune::FieldVector)

#undef DSC_FIELDVECTORTOSTR

//! toString for all matrix types that are only templated by their value type
#define DSC_MATRIXTOSTR(MatrixType)                                                                                    \
  template <class S>                                                                                                   \
  inline std::string toString(const MatrixType<S> matrix)                                                              \
  {                                                                                                                    \
    std::string str = "[";                                                                                             \
    for (size_t rr = 0; rr < matrix.rows(); ++rr) {                                                                    \
      if (rr > 0)                                                                                                      \
        str += "; ";                                                                                                   \
      for (size_t cc = 0; cc < matrix.cols(); ++cc) {                                                                  \
        if (cc > 0)                                                                                                    \
          str += " ";                                                                                                  \
        str +=                                                                                                         \
            toString(internal::ChooseBase<MatrixType<S>>::template MatrixGetter<MatrixType<S>>::template get_entry<S>( \
                matrix, rr, cc));                                                                                      \
      }                                                                                                                \
    }                                                                                                                  \
    str += "]";                                                                                                        \
    return str;                                                                                                        \
  }

DSC_MATRIXTOSTR(LA::CommonDenseMatrix)
DSC_MATRIXTOSTR(DynamicMatrix)
#if HAVE_EIGEN
DSC_MATRIXTOSTR(LA::EigenDenseMatrix)
#endif

#undef DSC_MATRIXTOSTR

//! toString for matrix types where the number of rows and columns is also templated
#define DSC_FIELDMATRIXTOSTR(FieldMatrixType)                                                                          \
  template <class S, int ROWS, int COLS>                                                                               \
  inline std::string toString(const FieldMatrixType<S, ROWS, COLS> matrix)                                             \
  {                                                                                                                    \
    std::string str = "[";                                                                                             \
    for (size_t rr = 0; rr < ROWS; ++rr) {                                                                             \
      if (rr > 0)                                                                                                      \
        str += "; ";                                                                                                   \
      for (size_t cc = 0; cc < COLS; ++cc) {                                                                           \
        if (cc > 0)                                                                                                    \
          str += " ";                                                                                                  \
        str += toString(                                                                                               \
            internal::ChooseBase<FieldMatrixType<S, ROWS, COLS>>::                                                     \
                template MatrixGetter<FieldMatrixType<S, ROWS, COLS>>::template get_entry<S>(matrix, rr, cc));         \
      }                                                                                                                \
    }                                                                                                                  \
    str += "]";                                                                                                        \
    return str;                                                                                                        \
  }

DSC_FIELDMATRIXTOSTR(Dune::Stuff::Common::FieldMatrix)
DSC_FIELDMATRIXTOSTR(Dune::FieldMatrix)

#undef DSC_FIELDMATRIXTOSTR


/**
  \brief      Returns a string of lengths s' whitespace (or c chars).
  \param[in]  s
              std::string, defines the length of the return string
  \param[in]  whitespace
              char, optional argument, defines entries of return string
  \return     std::string
              Returns a string of lengths s' whitespace (or c chars).
  **/
template <class T>
std::string whitespaceify(const T& t, const char whitespace = ' ')
{
  const std::string s = toString(t);
  std::string ret = "";
  for (unsigned int i = 0; i < s.size(); ++i) {
    ret += whitespace;
  }
  return ret;
} // std::string whitespaceify(const std::string s, const char whitespace = ' ')

/** \brief convenience wrapper around boost::algorithm::split to split one string into a vector of strings
 * \param msg the spring to be split
 * \param separators a list of separators, duh
 * \param mode token_compress_off --> potentially empty strings in return,
              token_compress_on --> empty tokens are discarded
 * \return all tokens in a vector, if msg contains no separators, this'll contain msg as its only element
 **/
template <class T /*= std::string */>
inline std::vector<T>
tokenize(const std::string& msg, const std::string& separators,
         const boost::algorithm::token_compress_mode_type mode /*= boost::algorithm::token_compress_off*/)
{
  std::vector<std::string> strings;
  boost::algorithm::split(strings, msg, boost::algorithm::is_any_of(separators), mode);
  std::vector<T> ret(strings.size());
  size_t i = 0;
  // special case for empty strings to avoid non-default init
  std::generate(
      std::begin(ret), std::end(ret), [&]() { return strings[i++].empty() ? T() : fromString<T>(strings[i - 1]); });
  return ret;
}

template <>
inline std::vector<std::string> tokenize(const std::string& msg, const std::string& separators,
                                         const boost::algorithm::token_compress_mode_type mode)
{
  std::vector<std::string> strings;
  boost::algorithm::split(strings, msg, boost::algorithm::is_any_of(separators), mode);
  return strings;
}

//! returns string with local time in current locale's format
inline std::string stringFromTime(time_t cur_time = time(NULL))
{
  return ctime(&cur_time);
}

//! helper struct for lexical cast
template <typename ElemT>
struct HexToString
{
  // see http://stackoverflow.com/a/2079728
  ElemT value;
  operator ElemT() const
  {
    return value;
  }
  friend std::istream& operator>>(std::istream& in, HexToString& out)
  {
    in >> std::hex >> out.value;
    return in;
  }
};

// some legacy stuff I'd like to keep for a while
namespace String {

inline char** vectorToMainArgs(const std::vector<std::string> args)
{
  char** argv = new char*[args.size()];
  for (unsigned int ii = 0; ii < args.size(); ++ii) {
    argv[ii] = new char[args[ii].length() + 1];
    strcpy(argv[ii], args[ii].c_str());
  }
  return argv;
} // char** vectorToMainArgs(const std::vector< std::string > args)

} // namespace String

} // namespace Common
} // namespace Stuff
} // namespace Dune

#endif // DUNE_STUFF_COMMON_STRING_HH
