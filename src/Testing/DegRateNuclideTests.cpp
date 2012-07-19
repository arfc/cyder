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
  EXPECT_FLOAT_EQ(test_rate, test_copy->deg_rate());
  delete test_copy;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, extract){ 
  // if you extract a meterial, the conc_map should reflect that
  // you shouldn't extrac more material than you have how much is that?
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR0){ 
  // if the degradation rate is zero, nothing should be released
  test_rate=0;
  EXPECT_NO_THROW();
  EXPECT_FLOAT_EQ();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  // nothing more
  // check that timestep 3 doesn't crash
  // check that it doesn't keep sending material it doesn't have
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a year
  // nothing more
  // check that timestep 2 doesn't crash
  // check that it doesn't keep sending material it doesn't have
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}




