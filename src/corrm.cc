#include "corrm.h"
using cyclus::Material;
using cyclus::Composition;
using cyclus::toolkit::ResBuf;
using cyclus::toolkit::MatVec;
using cyclus::KeyError;
using cyclus::ValueError;
using cyclus::Request;

namespace recycle {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
corrm::corrm(cyclus::Context* ctx) : cyclus::Facility(ctx) {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string corrm::str() {
  return Facility::str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void corrm::Tick() {std::cout << "Hello, ";}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void corrm::Tock() {std::cout << "World!\n";}

// WARNING! Do not change the following this function!!! This enables your
// archetype to be dynamically loaded and any alterations will cause your
// archetype to fail.
extern "C" cyclus::Agent* Constructcorrm(cyclus::Context* ctx) {
  return new corrm(ctx);
}

}  // namespace recycle
