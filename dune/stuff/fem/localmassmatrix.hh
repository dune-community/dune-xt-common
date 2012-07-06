#ifndef STUFF_LOCALMASSMATRIX_HH
#define STUFF_LOCALMASSMATRIX_HH

#include <dune/common/version.hh>
#include <dune/fem/operator/1order/localmassmatrix.hh>

namespace Dune {

namespace Stuff {

namespace Fem {

// ! create yet another child class just in case fem actually implements something in LocalMassMatrix in addition to its
// base
#if DUNE_VERSION_NEWER_REV(DUNE_FEM, 1, 3, 0)
template <class DiscreteFunctionSpaceImp, class VolumeQuadratureImp>
struct LocalMassMatrix : public Dune::LocalMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>
{
  LocalMassMatrix(const DiscreteFunctionSpaceImp& spc, const int volQuadOrd = -1)
    : Dune::LocalMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>(spc, volQuadOrd)
  {
  }
};
#else // if DUNE_VERSION_NEWER_REV(DUNE_FEM, 1, 3, 0)
template <class DiscreteFunctionSpaceImp, class VolumeQuadratureImp>
struct LocalMassMatrix : public Dune::LocalDGMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>
{
  LocalMassMatrix(const DiscreteFunctionSpaceImp& spc, const int volQuadOrd = -1)
    : Dune::LocalDGMassMatrix<DiscreteFunctionSpaceImp, VolumeQuadratureImp>(spc, volQuadOrd)
  {
  }
};
#endif // if DUNE_VERSION_NEWER_REV(DUNE_FEM, 1, 3, 0)

} // namespace Fem

} // namespace Stuff

} // namespace Dune

#endif // STUFF_LOCALMASSMATRIX_HH

/** Copyright (c) 2012, Rene Milk    , Sven Kaulmann
   * All rights reserved.
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions are met:
   *
   * 1. Redistributions of source code must retain the above copyright notice, this
   *    list of conditions and the following disclaimer.
   * 2. Redistributions in binary form must reproduce the above copyright notice,
   *    this list of conditions and the following disclaimer in the documentation
   *    and/or other materials provided with the distribution.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * The views and conclusions contained in the software and documentation are those
   * of the authors and should not be interpreted as representing official policies,
   * either expressed or implied, of the FreeBSD Project.
   **/
