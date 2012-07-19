// DegRateNuclideTests.cpp
#include <vector>
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeDegRateNuclide : public DegRateNuclide {
  public:
    FakeDegRateNuclide() : DegRateNuclide() {

      // initialize ordinary objects

      // initialize things that don't depend on the input
    }

    virtual ~FakeDegRateNuclide() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class DegRateNuclideTest : public ::testing::Test {
  protected:
    FakeDegRateNuclide* test_deg_rate_nuclide;

    virtual void SetUp(){
      test_deg_rate_nuclide = new FakeDegRateNuclide();
    }
    virtual void TearDown() {
      delete test_deg_rate_nuclide;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, defaultConstructor) {
  ASSERT_EQ("DEGRATE_NUCLIDE", test_deg_rate_nuclide->name());
  ASSERT_EQ(DEGRATE_NUCLIDE, test_deg_rate_nuclide->type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, initFunctionNoXML) { 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, copy) {
}
