#include <gtest/gtest.h>

#include "pyre.h"

#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using recycle::Pyre;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class PyreTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc;
  Pyre* facility;

  virtual void SetUp() {
    facility = new Pyre(tc.get());
  }

  virtual void TearDown() {
    delete facility;
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(PyreTest, InitialState) {
  // Test things about the initial state of the facility here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Do Not Touch! Below section required for connection with Cyclus
cyclus::Agent* PyreConstructor(cyclus::Context* ctx) {
  return new Pyre(ctx);
}
// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
INSTANTIATE_TEST_CASE_P(PyreFac, FacilityTests,
                        ::testing::Values(&PyreConstructor));
INSTANTIATE_TEST_CASE_P(PyreFac, AgentTests,
                        ::testing::Values(&PyreConstructor));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -