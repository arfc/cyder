#include <gtest/gtest.h>

#include "corrm.h"

#include "agent_tests.h"
#include "context.h"
#include "facility_tests.h"

using recycle::corrm;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class corrmTest : public ::testing::Test {
 protected:
  cyclus::TestContext tc;
  corrm* facility;

  virtual void SetUp() {
    facility = new corrm(tc.get());
  }

  virtual void TearDown() {
    delete facility;
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(corrmTest, InitialState) {
  // Test things about the initial state of the facility here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(corrmTest, Print) {
  EXPECT_NO_THROW(std::string s = facility->str());
  // Test StubFacility specific aspects of the print method here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(corrmTest, Tick) {
  ASSERT_NO_THROW(facility->Tick());
  // Test StubFacility specific behaviors of the Tick function here
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(corrmTest, Tock) {
  EXPECT_NO_THROW(facility->Tock());
  // Test StubFacility specific behaviors of the Tock function here
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Do Not Touch! Below section required for connection with Cyclus
cyclus::Agent* corrmConstructor(cyclus::Context* ctx) {
  return new corrm(ctx);
}
// Required to get functionality in cyclus agent unit tests library
#ifndef CYCLUS_AGENT_TESTS_CONNECTED
int ConnectAgentTests();
static int cyclus_agent_tests_connected = ConnectAgentTests();
#define CYCLUS_AGENT_TESTS_CONNECTED cyclus_agent_tests_connected
#endif  // CYCLUS_AGENT_TESTS_CONNECTED
INSTANTIATE_TEST_CASE_P(corrmFac, FacilityTests,
                        ::testing::Values(&corrmConstructor));
INSTANTIATE_TEST_CASE_P(corrmFac, AgentTests,
                        ::testing::Values(&corrmConstructor));
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
