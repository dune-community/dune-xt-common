#include <config.h>
#include "dune/xt/common/parallel/mpi_comm_wrapper.hh"


namespace Dune {
namespace XT {
namespace Common {

MPI_Comm_Wrapper::MPI_Comm_Wrapper(WrappedComm comm)
  : comm_(comm)
{
}

MPI_Comm_Wrapper& MPI_Comm_Wrapper::operator=(const WrappedComm comm)
{
  this->comm_ = comm;
  return *this;
}

MPI_Comm_Wrapper::WrappedComm MPI_Comm_Wrapper::get() const
{
  return comm_;
}


} // namespace Common
} // namespace XT
} // namespace Dune