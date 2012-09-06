// DegRateNuclideTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class DegRateNuclideTest : public ::testing::Test {
  protected:
    DegRateNuclide* deg_rate_ptr_;
    NuclideModel* nuc_model_ptr_;
    double deg_rate_;
    CompMapPtr test_comp_;
    mat_rsrc_ptr test_mat_;
    int one_mol_;
    int u235_, am241_;
    double test_size_;
    double theta_;
    double adv_vel_;
    GeometryPtr geom_;
    Radius r_four_, r_five_;
    Length len_five_;
    point_t origin_;
    int time_;

    virtual void SetUp(){
      // test_deg_rate_nuclide model setup
      deg_rate_ptr_ = new DegRateNuclide();
      deg_rate_ = 0.1;
      nuc_model_ptr_ = dynamic_cast<NuclideModel*>(deg_rate_ptr_);

      // set up geometry. this usually happens in the component init
      r_four_ = 4;
      r_five_ = 5;
      point_t origin_ = {0,0,0}; 
      len_five_ = 5;
      geom_ = GeometryPtr(new Geometry(r_four_, r_five_, origin_, len_five_));

      // other vars
      theta_ = 0.3; // percent porosity
      adv_vel_ = 1; // m/yr
      time_ = 0;

      // composition set up
      u235_=92235;
      one_mol_=1.0;
      test_comp_= CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_] = one_mol_;
      test_size_=10.0;

      // material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);
    }
    virtual void TearDown() {
      delete deg_rate_ptr_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, defaultConstructor) {
  ASSERT_EQ("DEGRATE_NUCLIDE", nuc_model_ptr_->name());
  ASSERT_EQ(DEGRATE_NUCLIDE, nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0,deg_rate_ptr_->deg_rate());
  ASSERT_FLOAT_EQ(0,deg_rate_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, initFunctionNoXML) { 
  EXPECT_NO_THROW(deg_rate_ptr_->init(deg_rate_));
  ASSERT_EQ(deg_rate_, deg_rate_ptr_->deg_rate());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, copy) {
  ASSERT_NO_THROW(deg_rate_ptr_->init(deg_rate_));
  DegRateNuclide* test_copy = new DegRateNuclide();
  EXPECT_NO_THROW(test_copy->copy(deg_rate_ptr_));
  EXPECT_NO_THROW(test_copy->copy(nuc_model_ptr_));
  EXPECT_FLOAT_EQ(deg_rate_, test_copy->deg_rate());
  delete test_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, setGeometry) {  
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  EXPECT_FLOAT_EQ(len_five_ , nuc_model_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, absorb){
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  ASSERT_EQ(0,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), deg_rate_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(1,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), deg_rate_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(2,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), deg_rate_ptr_->contained_mass(time_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, extract){ 
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  //ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  //EXPECT_NO_THROW(nuc_model_ptr_->extract(test_mat_));
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?
  mat_rsrc_ptr to_extract1;
  mat_rsrc_ptr to_extract2;
  mat_rsrc_ptr to_extract3;
  to_extract1 = mat_rsrc_ptr(new Material(test_mat_->isoVector()));
  to_extract2 = mat_rsrc_ptr(new Material(test_mat_->isoVector()));
  to_extract3 = mat_rsrc_ptr(new Material(test_mat_->isoVector()));
  to_extract1->setQuantity(frac*test_size_);
  to_extract2->setQuantity(frac*test_size_);
  to_extract3->setQuantity(frac*test_size_);

  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  ASSERT_EQ(0,time_);
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), deg_rate_ptr_->contained_mass(time_));
  EXPECT_FLOAT_EQ(test_size_, deg_rate_ptr_->contained_mass(time_));

  time_++;
  ASSERT_EQ(1,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->extract(to_extract1));
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, deg_rate_ptr_->contained_mass(time_));

  time_++;
  ASSERT_EQ(2,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->extract(to_extract2));
  EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, deg_rate_ptr_->contained_mass(time_));

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, set_deg_rate){ 
  // the deg rate must be between 0 and 1, inclusive
  deg_rate_=0;
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  deg_rate_=1;
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  // it should accept floats
  deg_rate_= 0.1;
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  // an exception should be thrown if it's set outside the bounds
  deg_rate_= -1;
  EXPECT_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(deg_rate_ptr_->deg_rate(), deg_rate_);
  deg_rate_= 2;
  EXPECT_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(deg_rate_ptr_->deg_rate(), deg_rate_);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, total_degradation){
  deg_rate_=0.3;
  int max_degs = 1.0/deg_rate_;
  // Check deg rate
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  ASSERT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);

  ASSERT_EQ(0, time_);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(deg_rate_*time_++, deg_rate_ptr_->tot_deg());

  ASSERT_EQ(1, time_);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(deg_rate_*time_++, deg_rate_ptr_->tot_deg());

  ASSERT_EQ(2, time_);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(deg_rate_*time_++, deg_rate_ptr_->tot_deg());

  ASSERT_EQ(3, time_);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(deg_rate_*time_++, deg_rate_ptr_->tot_deg());

  ASSERT_EQ(4, time_);
  ASSERT_TRUE(time_ > max_degs);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(1, deg_rate_ptr_->tot_deg());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, sum_mats){
  mat_rsrc_ptr new_test_mat = mat_rsrc_ptr(new Material(test_comp_));
  new_test_mat->setQuantity(2*test_size_);

  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  mats.push_back(new_test_mat);
  
  pair<IsoVector, double> the_sum;
  the_sum = deg_rate_ptr_->sum_mats(mats);
  EXPECT_FLOAT_EQ(3*test_size_, the_sum.second);
  EXPECT_FLOAT_EQ(1, the_sum.first.comp()->atomFraction(u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR0){ 
  // if the degradation rate is zero, nothing should be released
  // set the degradation rate
  deg_rate_=0;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());

  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_, deg_rate_ptr_->deg_rate());
  // get the initial mass
  double initial_mass = deg_rate_ptr_->contained_mass();
  // transport the nuclides
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));
  // check that the contained mass matches the initial mass
  EXPECT_FLOAT_EQ(initial_mass, deg_rate_ptr_->contained_mass()); 
  // check the source term 
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->source_term_bc().second);
  // check the boundary concentration ?
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->dirichlet_bc(u235_));
  // check the boundary flux
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(u235_));
  // check the neumann
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  deg_rate_= 0.5;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());

  // set the degradation rate
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // TRANSPORT NUCLIDES 
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // check that half that material is offered as the source term in one year
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(u235_));

  // remove the source term offered
  mat_rsrc_ptr to_extract;
  to_extract = mat_rsrc_ptr(new Material(nuc_model_ptr_->source_term_bc().first));
  to_extract->setQuantity(nuc_model_ptr_->source_term_bc().second);
  EXPECT_NO_THROW(nuc_model_ptr_->extract(to_extract));
  // TRANSPORT NUCLIDES 
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // check that the remaining half is offered as the source term in year two
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann 
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(u235_));

  // remove the source term offered
  to_extract = mat_rsrc_ptr(new Material(nuc_model_ptr_->source_term_bc().first));
  to_extract->setQuantity(nuc_model_ptr_->source_term_bc().second);
  EXPECT_NO_THROW(nuc_model_ptr_->extract(to_extract));
  // TRANSPORT NUCLIDES 
  time_++;
  ASSERT_EQ(3, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // check that timestep 3 doesn't crash or offer material it doesn't have
  // Source Term
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a timestep
  deg_rate_= 1;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());

  // set the degradation rate
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));

  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  EXPECT_FLOAT_EQ(theta_*adv_vel_*expected_conc, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann 
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(u235_));

  // remove the source term offered
  mat_rsrc_ptr to_extract;
  to_extract = mat_rsrc_ptr(new Material(nuc_model_ptr_->source_term_bc().first));
  to_extract->setQuantity(nuc_model_ptr_->source_term_bc().second);
  EXPECT_NO_THROW(nuc_model_ptr_->extract(to_extract));
  // TRANSPORT NUCLIDES
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));

  // check that nothing more is offered in time step 2
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->source_term_bc().second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}

