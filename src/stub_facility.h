#ifndef RECYCLE_SRC_STUB_FACILITY_H_
#define RECYCLE_SRC_STUB_FACILITY_H_

#include <string>

#include "cyclus.h"
#include "recycle_version.h"

namespace recycle {


class StubFacility : public cyclus::Facility,
  public cyclus::toolkit::CommodityProducer  {
 public:
  /// Constructor for StubFacility Class
  /// @param ctx the cyclus context for access to simulation-wide parameters
  explicit StubFacility(cyclus::Context* ctx);

  /// The Prime Directive
  /// Generates code that handles all input file reading and restart operations
  /// (e.g., reading from the database, instantiating a new object, etc.).
  /// @warning The Prime Directive must have a space before it! (A fix will be
  /// in 2.0 ^TM)

  #pragma cyclus

  #pragma cyclus note {"doc": "A stub facility is provided as a skeleton " \
                              "for the design of new facility agents."}

  /// A verbose printer for the StubFacility
  virtual std::string str();

  /// The handleTick function specific to the StubFacility.
  /// @param time the time of the tick
  virtual void Tick();

  /// The handleTick function specific to the StubFacility.
  /// @param time the time of the tock
  virtual void Tock();

  // And away we go!
};

}  // namespace recycle

#endif  // RECYCLE_SRC_STUB_FACILITY_H_
