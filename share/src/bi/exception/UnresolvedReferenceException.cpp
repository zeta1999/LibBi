/**
 * @file
 *
 * @author Lawrence Murray <lawrence.murray@csiro.au>
 * $Rev$
 * $Date$
 */
#include "UnresolvedReferenceException.hpp"

#include <sstream>

biprog::UnresolvedReferenceException::UnresolvedReferenceException(
    boost::shared_ptr<Reference> ref) {
  std::stringstream buf;
  buf << "unresolved reference " << ref << std::endl;
  msg = buf.str();
}
