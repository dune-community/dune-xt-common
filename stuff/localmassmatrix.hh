#ifndef STUFF_LOCALMASSMATRIX_HH
#define STUFF_LOCALMASSMATRIX_HH

#include <dune/common/version.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>

namespace Stuff {
//! create yet another child class just in case fem actually implements something in LocalMassMatrix in addition to its
//! base
#if DUNE_VERSION_NEWER_REV(DUNE_FEM, 1, 2, 0)
template <class DiscreteFunctionSpaceImp, class VolumeQuadratureImp>
struct LocalMassMatrix : public Dune::LocalMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>
{
  LocalMassMatrix(const DiscreteFunctionSpaceImp& spc, const int volQuadOrd = -1)
    : Dune::LocalMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>(spc, volQuadOrd)
  {
  }
};
#else
template <class DiscreteFunctionSpaceImp, class VolumeQuadratureImp>
struct LocalMassMatrix : public Dune::LocalDGMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>
{
  LocalMassMatrix(const DiscreteFunctionSpaceImp& spc, const int volQuadOrd = -1)
    : Dune::LocalDGMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>(spc, volQuadOrd)
  {
  }
};
#endif

} // namespace Stuff {

#endif // STUFF_LOCALMASSMATRIX_HH
