/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#ifndef BI_MPI_MPI_HPP
#define BI_MPI_MPI_HPP

#ifdef ENABLE_MPI
#include <mpi.h>
#endif

#include <string>

#ifdef ENABLE_SINGLE
#define MPI_REAL MPI_FLOAT
#else
#define MPI_REAL MPI_DOUBLE
#endif

namespace bi {
/**
 * Append rank to file name.
 *
 * @param name File name.
 */
std::string append_rank(const std::string& name);

/**
 * Message tags.
 */
enum MPITag {
  /**
   * Server tags.
   */
  MPI_TAG_JOIN,
  MPI_TAG_DISCONNECT,

  /*
   * Stopper tags.
   */
  MPI_TAG_STOPPER_STOP,
  MPI_TAG_STOPPER_ADD_WEIGHT,
  MPI_TAG_STOPPER_ADD_WEIGHTS,
  MPI_TAG_STOPPER_MAX_WEIGHT,

  /*
   * Adapter tags.
   */
  MPI_TAG_ADAPTER_SAMPLE,
  MPI_TAG_ADAPTER_WEIGHT,
  MPI_TAG_ADAPTER_SAMPLES,
  MPI_TAG_ADAPTER_WEIGHTS,

  /*
   * Base tag index when redistributing particles.
   */
  MPI_TAG_PARTICLE
};

}

#endif
