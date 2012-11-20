// DegRateNuclideTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "DegRateNuclideTests.h"
#include "NuclideModelTests.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"
#include "XMLQueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclideTest::SetUp(){
  // set up geometry. this usually happens in the component init
  r_four_ = 4;
  r_five_ = 5;
  point_t origin_ = {0,0,0}; 
  len_five_ = 5;
  geom_ = GeometryPtr(new Geometry(r_four_, r_five_, origin_, len_five_));

  // other vars
  theta_ = 0.3; // percent porosity
  adv_vel_ = .1; // m/yr @TODO worry about units
  time_ = 0;
  D_ = 0.00063; //@TODO worry about units 

  // composition set up
  u235_=92235;
  one_mol_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_mol_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_deg_rate_nuclide model setup
  deg_rate_ = 0.1;
  initNuclideModel(); //initializes deg_rate_ptr_
  nuc_model_ptr_ = dynamic_cast<NuclideModel*>(deg_rate_ptr_);
  default_deg_rate_ptr_ = new DegRateNuclide();
  default_nuc_model_ptr_ = dynamic_cast<NuclideModel*>(default_deg_rate_ptr_);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclideTest::TearDown() {
  delete deg_rate_ptr_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* DegRateNuclideModelConstructor(){
  return dynamic_cast<NuclideModel*>(new DegRateNuclide());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
DegRateNuclide* DegRateNuclideTest::initNuclideModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <advective_velocity>" << adv_vel_ << "</advective_velocity>"
     << "  <degradation>" << deg_rate_ << "</degradation>"
     << "  <diffusion_coeff>" << D_ << "</diffusion_coeff>"
     << "</start>";

  XMLParser parser(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  deg_rate_ptr_ = new DegRateNuclide();
  deg_rate_ptr_->initModuleMembers(engine);
  delete engine;
  return deg_rate_ptr_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, initial_state){
  EXPECT_EQ(deg_rate_, deg_rate_ptr_->deg_rate());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, defaultConstructor) {
  ASSERT_EQ("DEGRATE_NUCLIDE", default_nuc_model_ptr_->name());
  ASSERT_EQ(DEGRATE_NUCLIDE, default_nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, default_deg_rate_ptr_->deg_rate());
  ASSERT_FLOAT_EQ(0, default_deg_rate_ptr_->geom()->length());
  EXPECT_FLOAT_EQ(0, default_nuc_model_ptr_->contained_mass(time_));
  EXPECT_FLOAT_EQ(default_nuc_model_ptr_->contained_mass(0), default_deg_rate_ptr_->contained_mass());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, copy) {
  DegRateNuclide* test_copy = new DegRateNuclide();
  EXPECT_NO_THROW(test_copy->copy(deg_rate_ptr_));
  EXPECT_NO_THROW(test_copy->copy(nuc_model_ptr_));
  EXPECT_FLOAT_EQ(deg_rate_, test_copy->deg_rate());
  delete test_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, absorb){
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  for(int i=0; i<4; i++){
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
    EXPECT_NO_THROW(deg_rate_ptr_->update_vec_hist(time_));
    EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), nuc_model_ptr_->contained_mass(time_));
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, extract){ 
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?

  ASSERT_EQ(0,time_);
  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(deg_rate_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), nuc_model_ptr_->contained_mass(time_));
  EXPECT_FLOAT_EQ(test_size_, nuc_model_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(deg_rate_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, nuc_model_ptr_->contained_mass(time_));
  }

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

  for(int i=0; i<4; i++){
    ASSERT_EQ(i, time_);
    ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ(deg_rate_*time_++, deg_rate_ptr_->tot_deg());
  }

  ASSERT_EQ(4, time_);
  ASSERT_TRUE(time_ > max_degs);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(1, deg_rate_ptr_->tot_deg());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR0){ 
  // if the degradation rate is zero, nothing should be released
  // set the degradation rate
  deg_rate_=0;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  double radial_midpoint = outer_radius + (outer_radius - nuc_model_ptr_->geom()->inner_radius())/2;

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
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // check the neumann bc
  EXPECT_FLOAT_EQ(0 , nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  deg_rate_= 0.5;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  double expected_conc_w_vel = theta_*adv_vel_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

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
  // Neumann
  double expected_neumann= -expected_conc/(outer_radius*2 - deg_rate_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));
  // Cauchy
  double expected_cauchy = -D_*expected_neumann + adv_vel_*expected_conc; // @TODO fix units everywhere
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES 
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // check that the remaining half is offered as the source term in year two
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  expected_neumann= -expected_conc/(outer_radius*2 - deg_rate_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  expected_cauchy = -D_*expected_neumann + adv_vel_*expected_conc; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
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
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a timestep
  deg_rate_= 1;
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  double expected_conc_w_vel = theta_*adv_vel_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // set the degradation rate
  ASSERT_NO_THROW(deg_rate_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_ptr_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - deg_rate_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  double expected_cauchy = -D_*expected_neumann + adv_vel_*expected_conc; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  // check that nothing more is offered in time step 2
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->source_term_bc().second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, updateVecHist){ 
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, contained_mass){ 
  time_++;
  EXPECT_FLOAT_EQ(0, deg_rate_ptr_->contained_mass());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, updateDegradation){ 
  double deg_rate=0.1;
   
  for(int i=0; i<5; i++){
    time_++;
    EXPECT_NO_THROW(deg_rate_ptr_->update_degradation(time_, deg_rate));
    EXPECT_EQ(time_*deg_rate,deg_rate_ptr_->tot_deg());
  }
  EXPECT_NO_THROW(deg_rate_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,deg_rate_ptr_->tot_deg());
  EXPECT_NO_THROW(deg_rate_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,deg_rate_ptr_->tot_deg());
  EXPECT_NO_THROW(deg_rate_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,deg_rate_ptr_->tot_deg());
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, setGeometry) {  
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  EXPECT_FLOAT_EQ(len_five_ , nuc_model_ptr_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , nuc_model_ptr_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , nuc_model_ptr_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, getVolume) {  
  EXPECT_NO_THROW(deg_rate_ptr_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(deg_rate_ptr_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , nuc_model_ptr_->geom()->volume());
  EXPECT_NO_THROW(deg_rate_ptr_->geom()->set_radius(OUTER, numeric_limits<double>::infinity()));
  EXPECT_FLOAT_EQ( numeric_limits<double>::infinity(), nuc_model_ptr_->geom()->volume());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(DegRateNuclideTest, calc_conc_grad) {  
  Concentration c_out, c_in;
  Radius r_out, r_in;
  c_out = 0;
  c_in = 2;
  r_out = 5;
  r_in = 4;

  EXPECT_FLOAT_EQ( (c_out-c_in)/(r_out-r_in), deg_rate_ptr_->calc_conc_grad(c_out, c_in, r_out, r_in)); 
  EXPECT_THROW( deg_rate_ptr_->calc_conc_grad(c_out, c_in, r_in, r_out), CycRangeException); 
  EXPECT_THROW( deg_rate_ptr_->calc_conc_grad(c_out, c_in, r_in, r_in), CycRangeException); 
  EXPECT_THROW( deg_rate_ptr_->calc_conc_grad(c_out, c_in, r_in, numeric_limits<double>::infinity()), CycRangeException); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(DegRateNuclideModel, NuclideModelTests, Values(&DegRateNuclideModelConstructor));

