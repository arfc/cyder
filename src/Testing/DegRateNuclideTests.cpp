// DegRateNuclideTests.cpp
#include <vector>
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "CycException.h"
#include "Material.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class DegRateNuclideTest : public ::testing::Test {
  protected:
    DegRateNuclide* deg_rate_model_;
    double deg_rate_;
    CompMapPtr test_comp_;
    mat_rsrc_ptr test_mat_;
    int one_mol_;
    int u235_, am241_;
    double test_size_;
    int time_;

    virtual void SetUp(){
      // test_deg_rate_nuclide model setup
      deg_rate_model_ = new DegRateNuclide();
      deg_rate_ = 0.1;

      // composition set up
      u235_=92235;
      one_mol_=1.0;
      test_comp_= CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_] = one_mol_;
      test_size_=10.0;

      // material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);
      int time_ = TI->time();
    }
    virtual void TearDown() {
      delete deg_rate_model_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, defaultConstructor) {
  ASSERT_EQ("DEGRATE_NUCLIDE", deg_rate_model_->name());
  ASSERT_EQ(DEGRATE_NUCLIDE, deg_rate_model_->type());
  ASSERT_FLOAT_EQ(0,deg_rate_model_->deg_rate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, initFunctionNoXML) { 
  EXPECT_NO_THROW(deg_rate_model_->init(deg_rate_));
  ASSERT_EQ(deg_rate_, deg_rate_model_->deg_rate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, copy) {
  EXPECT_NO_THROW(deg_rate_model_->init(deg_rate_));
  DegRateNuclide* test_copy = new DegRateNuclide();
  EXPECT_NO_THROW(test_copy->copy(deg_rate_model_));
  EXPECT_FLOAT_EQ(deg_rate_, test_copy->deg_rate());
  delete test_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  EXPECT_NO_THROW(deg_rate_model_->absorb(test_mat_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, extract){ 
  // it should be able to extract all of the material it absorbed
  EXPECT_NO_THROW(deg_rate_model_->absorb(test_mat_));
  EXPECT_NO_THROW(deg_rate_model_->extract(test_mat_));
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, set_deg_rate){ 
  // the deg rate must be between 0 and 1, inclusive
  deg_rate_=0;
  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_model_->deg_rate(), deg_rate_);
  deg_rate_=1;
  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_model_->deg_rate(), deg_rate_);
  // it should accept floats
  deg_rate_= 0.1;
  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_model_->deg_rate(), deg_rate_);
  // an exception should be thrown if it's set outside the bounds
  deg_rate_= -1;
  EXPECT_THROW(deg_rate_model_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(deg_rate_model_->deg_rate(), deg_rate_);
  deg_rate_= 2;
  EXPECT_THROW(deg_rate_model_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(deg_rate_model_->deg_rate(), deg_rate_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR0){ 
  // if the degradation rate is zero, nothing should be released
  deg_rate_=0;
  deg_rate_model_->set_deg_rate(deg_rate_);

  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  // nothing more
  // check that timestep 3 doesn't crash
  // check that it doesn't keep sending material it doesn't have
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a year
  // nothing more
  // check that timestep 2 doesn't crash
  // check that it doesn't keep sending material it doesn't have
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}

