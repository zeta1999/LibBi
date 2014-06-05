/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_BUFFER_MCMCNETCDFBUFFER_HPP
#define BI_BUFFER_MCMCNETCDFBUFFER_HPP

#include "SimulatorNetCDFBuffer.hpp"
#include "../state/State.hpp"
#include "../method/misc.hpp"

#include <vector>

namespace bi {
/**
 * Buffer for storing, reading and writing results of ParticleMCMC in
 * NetCDF file.
 *
 * @ingroup io_buffer
 */
class MCMCNetCDFBuffer: public SimulatorNetCDFBuffer {
public:
  /**
   * Constructor.
   *
   * @param m Model.
   * @param file NetCDF file name.
   * @param mode File open mode.
   */
  MCMCNetCDFBuffer(const Model& m, const std::string& file,
      const FileMode mode = READ_ONLY, const SchemaMode schema = MULTI);

  /**
   * Constructor.
   *
   * @param m Model.
   * @param P Number of samples in file.
   * @param T Number of times in file.
   * @param file NetCDF file name.
   * @param mode File open mode.
   */
  MCMCNetCDFBuffer(const Model& m, const size_t P, const size_t T,
      const std::string& file, const FileMode mode = READ_ONLY,
      const SchemaMode schema = MULTI);

  /**
   * Write sample.
   *
   * @tparam S1 State type.
   *
   * @param c Sample index.
   * @param s State.
   */
  template<class S1>
  void write(const int c, const S1& s);

  /**
   * Read log-likelihoods.
   *
   * @param p Index of first sample.
   * @param[out] ll Log-likelihoods.
   */
  template<class V1>
  void readLogLikelihoods(const size_t p, V1 ll);

  /**
   * Write log-likelihoods.
   *
   * @param p Index of first sample.
   * @param ll Log-likelihoods.
   */
  template<class V1>
  void writeLogLikelihoods(const size_t p, const V1 ll);

  /**
   * Read log-prior densities.
   *
   * @param p Index of first sample.
   * @param[out] lp Log-prior densities.
   */
  template<class V1>
  void readLogPriors(const size_t p, V1 lp);

  /**
   * Write log-prior densities.
   *
   * @param p Index of first sample.
   * @param lp Log-prior densities.
   */
  template<class V1>
  void writeLogPriors(const size_t p, const V1 lp);

protected:
  /**
   * Set up structure of NetCDF file.
   */
  void create();

  /**
   * Map structure of existing NetCDF file.
   */
  void map();

  /**
   * Log-likelihoods variable.
   */
  int llVar;

  /**
   * Prior log-densities variable.
   */
  int lpVar;
};

}

template<class S1>
void bi::MCMCNetCDFBuffer::write(const int c, const S1& s) {
  if (c == 0) {
    //writeTimes(0, s.getTimes());
  }
  writeLogLikelihood(c, s.logLikelihood1);
  writeLogPrior(c, s.logPrior1);
  writeParameter(c, s.theta1);
  writePath(c, s.path);
}

template<class V1>
void bi::MCMCNetCDFBuffer::readLogLikelihoods(const size_t p, V1 ll) {
  readRange(llVar, p, ll);
}

template<class V1>
void bi::MCMCNetCDFBuffer::writeLogLikelihoods(const size_t p,
    const V1 ll) {
  writeRange(llVar, p, ll);
}

template<class V1>
void bi::MCMCNetCDFBuffer::readLogPriors(const size_t p, V1 lp) {
  readRange(lpVar, p, lp);
}

template<class V1>
void bi::MCMCNetCDFBuffer::writeLogPriors(const size_t p,
    const V1 lp) {
  writeRange(lpVar, p, lp);
}

#endif
