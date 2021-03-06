/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 */
#ifndef BI_CUDA_UPDATER_SPARSESTATICLOGDENSITYMATRIXVISITORGPU_CUH
#define BI_CUDA_UPDATER_SPARSESTATICLOGDENSITYMATRIXVISITORGPU_CUH

namespace bi {
/**
 * Matrix visitor for sparse static log-density updates, on device.
 *
 * @tparam B Model type.
 * @tparam S Action type list.
 * @tparam PX Parents type.
 * @tparam OX Output type.
 */
template<class B, class S, class PX, class OX>
class SparseStaticLogDensityMatrixVisitorGPU {
public:
  /**
   * Update log-density.
   *
   * @tparam T1 Scalar type.
   *
   * @param s State.
   * @param mask Mask.
   * @param p Trajectory id.
   * @param pax Parents.
   * @param[out] x Output.
   * @param[in,out] lp Log-density.
   */
  template<class T1>
  static CUDA_FUNC_DEVICE void accept(State<B,ON_DEVICE>& s,
      const Mask<ON_DEVICE>& mask, const int p, const PX& pax, OX& x,
      T1& lp);
};

/**
 * @internal
 *
 * Base case of SparseStaticLogDensityMatrixVisitorGPU.
 */
template<class B, class PX, class OX>
class SparseStaticLogDensityMatrixVisitorGPU<B,empty_typelist,PX,OX> {
public:
  template<class T1>
  static CUDA_FUNC_DEVICE void accept(State<B,ON_DEVICE>& s,
      const Mask<ON_DEVICE>& mask, const int p, const PX& pax, OX& x,
      T1& lp) {
    //
  }
};

}

#include "../../typelist/front.hpp"
#include "../../typelist/pop_front.hpp"

template<class B, class S, class PX, class OX>
template<class T1>
inline void bi::SparseStaticLogDensityMatrixVisitorGPU<B,S,PX,OX>::accept(
    State<B,ON_DEVICE>& s, const Mask<ON_DEVICE>& mask, const int p,
    const PX& pax, OX& x, T1& lp) {
  typedef typename front<S>::type front;
  typedef typename pop_front<S>::type pop_front;
  typedef typename front::target_type target_type;

  const int id = var_id<target_type>::value;

  if (mask.isDense(id)) {
    front::logDensities(s, p, pax, x, lp);
  } else if (mask.isSparse(id)) {
    BI_ASSERT_MSG(false, "Cannot do sparse update with matrix expression");
  }

  SparseStaticLogDensityMatrixVisitorGPU<B,pop_front,PX,OX>::accept(s, mask,
      p, pax, x, lp);
}

#endif
