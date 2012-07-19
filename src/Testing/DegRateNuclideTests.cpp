// DegRateNuclideTests.cpp
#include <vector>
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class DegRateNuclideTest : public ::testing::Test {
  protected:
    DegRateNuclide* test_deg_rate_nuclide;
    double test_rate;

    virtual void SetUp(){
      test_deg_rate_nuclide = new DegRateNuclide();
      test_rate = 0.1;
    }
    virtual void TearDown() {
      delete test_deg_rate_nuclide;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, defaultConstructor) {
  ASSERT_EQ("DEGRATE_NUCLIDE", test_deg_rate_nuclide->name());
  ASSERT_EQ(DEGRATE_NUCLIDE, test_deg_rate_nuclide->type());
  ASSERT_FLOAT_EQ(0,test_deg_rate_nuclide->deg_rate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, initFunctionNoXML) { 
  EXPECT_NO_THROW(test_deg_rate_nuclide->init(test_rate));
  ASSERT_EQ(test_rate, test_deg_rate_nuclide->deg_rate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, copy) {
  EXPECT_NO_THROW(test_deg_rate_nuclide->init(test_rate));
  DegRateNuclide* test_copy = new DegRateNuclide();
  EXPECT_NO_THROW(test_copy->copy(test_deg_rate_nuclide));
  delete test_copy;
}
