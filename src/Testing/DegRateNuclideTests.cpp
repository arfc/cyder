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
    double theta_;
    double adv_vel_;
    double vol_;

    virtual void SetUp(){
      // test_deg_rate_nuclide model setup
      deg_rate_model_ = new DegRateNuclide();
      deg_rate_ = 0.1;

      // other vars
      theta_ = 0.3; // percent porosity
      adv_vel_ = 1; // m/yr
      vol_ = 2*3.1.159; // m^3 

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
  // set the degradation rate
  deg_rate_=0;
  double expected_src = deg_rate*test_size_;
  double expected_conc = expected_src/vol_;


  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_, deg_rate_model_->deg_rate());
  // get the initial mass
  double initial_mass = deg_rate_model_->contained_mass();
  // transport the nuclides
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());
  // check that the contained mass matches the initial mass
  EXPECT_FLOAT_EQ(initial_mass, deg_rate_model_->contained_mass()); 
  // check the source term 
  EXPECT_FLOAT_EQ(0, deg_rate_model_->source_term_bc()->quantity());
  // check the boundary concentration ?
  EXPECT_FLOAT_EQ(0, deg_rate_model_->dirichlet_bc());
  // check the boundary flux
  EXPECT_FLOAT_EQ(0, deg_rate_model_->cauchy_bc());
  // check the neumann
  EXPECT_FLOAT_EQ(0, deg_rate_model_->neumann_bc());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  deg_rate_= 0.5;
  double expected_src = deg_rate*test_size_;
  double expected_conc = expected_src/vol_;

  // set the degradation rate
  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_model_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(deg_rate_model_->absorb(test_mat_));

  // TRANSPORT NUCLIDES 
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());

  // check that half that material is offered as the source term in one year
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, deg_rate_model_->source_term_bc()->quantity());
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, deg_rate_model_->dirichlet_bc());
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, deg_rate_model_->cauchy_bc());
  // Neumann
  EXPECT_FLOAT_EQ(0, deg_rate_model_->neumann_bc());

  // remove the source term offered
  EXPECT_NO_THROW(deg_rate_model_->extract(deg_rate_model_->source_term_bc()));
  // TRANSPORT NUCLIDES 
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());

  // check that the remaining half is offered as the source term in year two
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, deg_rate_model_->source_term_bc()->quantity());
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, deg_rate_model_->dirichlet_bc());
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, deg_rate_model_->cauchy_bc());
  // Neumann 
  EXPECT_FLOAT_EQ(0, deg_rate_model_->neumann_bc());

  // remove the source term offered
  EXPECT_NO_THROW(deg_rate_model_->extract(deg_rate_model_->source_term_bc()));
  // TRANSPORT NUCLIDES 
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());

  // check that timestep 3 doesn't crash or offer material it doesn't have
  // Source Term
  EXPECT_FLOAT_EQ(0, deg_rate_model_->source_term_bc()->quantity());
  // Dirichlet
  EXPECT_FLOAT_EQ(0, deg_rate_model_->dirichlet_bc());
  // Cauchy
  EXPECT_FLOAT_EQ(0, deg_rate_model_->cauchy_bc());
  // Neumann
  EXPECT_FLOAT_EQ(0, deg_rate_model_->neumann_bc());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a timestep
  deg_rate_= 1;
  double expected_src = deg_rate*test_size_;
  double expected_conc = expected_src/vol_;

  // set the degradation rate
  EXPECT_NO_THROW(deg_rate_model_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_model_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(deg_rate_model_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());

  // Source Term
  EXPECT_FLOAT_EQ(expected_src, deg_rate_model_->source_term_bc()->quantity());
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, deg_rate_model_->dirichlet_bc());
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, deg_rate_model_->cauchy_bc());
  // Neumann 
  EXPECT_FLOAT_EQ(0, deg_rate_model_->neumann_bc());

  // remove the source term offered
  EXPECT_NO_THROW(deg_rate_model_->extract(deg_rate_model_->source_term_bc()));
  // TRANSPORT NUCLIDES
  EXPECT_NO_THROW(deg_rate_model_->transportNuclides());

  // check that nothing more is offered in time step 2
  EXPECT_FLOAT_EQ(0, deg_rate_model_->source_term_bc()->quantity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}

