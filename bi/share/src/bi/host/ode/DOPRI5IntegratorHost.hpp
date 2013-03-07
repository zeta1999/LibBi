/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_HOST_ODE_DOPRI5INTEGRATORHOST_HPP
#define BI_HOST_ODE_DOPRI5INTEGRATORHOST_HPP

namespace bi {
/**
 * Dormand-Prince 5(4) integrator.
 *
 * @ingroup method_updater
 *
 * @tparam B Model type.
 * @tparam S Action type list.
 * @tparam T1 Scalar type.
 */
template<class B, class S, class T1>
class DOPRI5IntegratorHost {
public:
  /**
   * Integrate.
   *
   * @param t1 Start of time interval.
   * @param t2 End of time interval.
   * @param[in,out] s State.
   */
  static void update(const T1 t1, const T1 t2, State<B,ON_HOST>& s);
};
}

#include "DOPRI5VisitorHost.hpp"
#include "IntegratorConstants.hpp"
#include "../host.hpp"
#include "../shared_host.hpp"
#include "../../state/Pa.hpp"
#include "../../typelist/front.hpp"
#include "../../typelist/pop_front.hpp"
#include "../../traits/block_traits.hpp"
#include "../../math/view.hpp"

template<class B, class S, class T1>
void bi::DOPRI5IntegratorHost<B,S,T1>::update(const T1 t1, const T1 t2,
    State<B,ON_HOST>& s) {
  /* pre-condition */
  BI_ASSERT(t1 < t2);

  typedef host_vector_reference<real> vector_reference_type;
  typedef Pa<ON_HOST,B,host,host,host,shared_host<S> > PX;
  typedef DOPRI5VisitorHost<B,S,S,real,PX,real> Visitor;

  static const int N = block_size<S>::value;
  const int P = s.size();

  bind(s);

  #pragma omp parallel
  {
    real buf[11*N]; // use of dynamic array faster than heap allocation
    vector_reference_type x0(buf, N);
    vector_reference_type x1(buf + N, N);
    vector_reference_type x2(buf + 2*N, N);
    vector_reference_type x3(buf + 3*N, N);
    vector_reference_type x4(buf + 4*N, N);
    vector_reference_type x5(buf + 5*N, N);
    vector_reference_type x6(buf + 6*N, N);
    vector_reference_type err(buf + 7*N, N);
    vector_reference_type k1(buf + 8*N, N);
    vector_reference_type k7(buf + 9*N, N);
    vector_reference_type shared(buf + 10*N, N);
    sharedHostState = &shared;

    real t, h, e, e2, logfacold, logfac11, fac;
    int n, id, p;
    bool k1in;
    PX pax;

    #pragma omp for
    for (p = 0; p < P; ++p) {
      /* initialise shared memory from global memory */
      shared_host_init<B,S>(s, p);

      t = t1;
      h = h_h0;
      logfacold = bi::log(BI_REAL(1.0e-4));
      k1in = false;
      n = 0;
      x0 = *sharedHostState;

      /* integrate */
      while (t < t2 && n < h_nsteps) {
        if (BI_REAL(0.1)*bi::abs(h) <= bi::abs(t)*h_uround) {
          // step size too small
        }
        if (t + BI_REAL(1.01)*h - t2 > BI_REAL(0.0)) {
          h = t2 - t;
          if (h <= 0.0) {
            t = t2;
            break;
          }
        }

        /* stages */
        Visitor::stage1(t, h, s, p, pax, x0.buf(), x1.buf(), x2.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), k1.buf(), err.buf(), k1in);
        k1in = true; // can reuse from previous iteration in future
        sharedHostState->swap(x1);

        Visitor::stage2(t, h, s, p, pax, x0.buf(), x2.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sharedHostState->swap(x2);

        Visitor::stage3(t, h, s, p, pax, x0.buf(), x3.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sharedHostState->swap(x3);

        Visitor::stage4(t, h, s, p, pax, x0.buf(), x4.buf(), x5.buf(), x6.buf(), err.buf());
        sharedHostState->swap(x4);

        Visitor::stage5(t, h, s, p, pax, x0.buf(), x5.buf(), x6.buf(), err.buf());
        sharedHostState->swap(x5);

        Visitor::stage6(t, h, s, p, pax, x0.buf(), x6.buf(), err.buf());

        /* compute error */
        Visitor::stageErr(t, h, s, p, pax, x0.buf(), x6.buf(), k7.buf(), err.buf());

        e2 = 0.0;
        for (id = 0; id < N; ++id) {
          e = err(id)*h/(h_atoler + h_rtoler*bi::max(bi::abs(x0(id)), bi::abs(x6(id))));
          e2 += e*e;
        }
        e2 /= N;

        /* accept/reject */
        if (e2 <= BI_REAL(1.0)) {
          /* accept */
          t += h;
          x0.swap(x6);
          k1.swap(k7);
        }
        *sharedHostState = x0;

        /* compute next step size */
        if (t < t2) {
          logfac11 = h_expo*bi::log(e2);
          if (e2 > BI_REAL(1.0)) {
            /* step was rejected */
            h *= bi::max(h_facl, bi::exp(h_logsafe - logfac11));
          } else {
            /* step was accepted */
            fac = bi::exp(h_beta*logfacold + h_logsafe - logfac11); // Lund-stabilization
            fac = bi::min(h_facr, bi::max(h_facl, fac));  // bound
            h *= fac;
            logfacold = BI_REAL(0.5)*bi::log(bi::max(e2, BI_REAL(1.0e-8)));
          }
        }

        ++n;
      }

      /* write from shared back to global memory */
      shared_host_commit<B,S>(s, p);
    }
  }

  unbind(s);
}

#endif
