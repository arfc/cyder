// LumpedNuclideTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "LumpedNuclide.h"
#include "LumpedNuclideTests.h"
#include "NuclideModelTests.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"
#include "XMLQueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclideTest::SetUp(){

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
  t_t_ = 1;

  // composition set up
  u235_=92235;
  one_mol_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_mol_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_lumped_nuclide model setup
  lumped_ptr_=LumpedNuclidePtr(initNuclideModel());
  nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(lumped_ptr_);
  default_lumped_ptr_ = LumpedNuclidePtr(LumpedNuclide::create());
  default_nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(default_lumped_ptr_);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclideTest::TearDown() {  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModelPtr LumpedNuclideModelConstructor (){
  return boost::dynamic_pointer_cast<NuclideModel>(LumpedNuclide::create());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
LumpedNuclidePtr LumpedNuclideTest::initNuclideModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <advective_velocity>" << adv_vel_ << "</advective_velocity>"
     << "  <transit_time>" << t_t_ << "</transit_time>"
     << "  <formulation>"
     << "    <EM/>"
     << "  </formulation>"
     << "</start>";

  XMLParser parser(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  lumped_ptr_ = LumpedNuclidePtr(LumpedNuclide::create());
  lumped_ptr_->initModuleMembers(engine);
  delete engine;
  return lumped_ptr_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, initial_state){
  EXPECT_EQ(t_t_, lumped_ptr_->transit_time());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, defaultConstructor) {
  ASSERT_EQ("LUMPED_NUCLIDE", nuc_model_ptr_->name());
  ASSERT_EQ(LUMPED_NUCLIDE, nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, lumped_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, initFunctionNoXML) { 
  //EXPECT_NO_THROW(lumped_ptr_->init(some_param_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, copy) {
  //ASSERT_NO_THROW(lumped_ptr_->init(some_param_));
  LumpedNuclidePtr test_copy = LumpedNuclidePtr(LumpedNuclide::create());
  LumpedNuclidePtr lumped_shared_ptr = LumpedNuclidePtr(lumped_ptr_);
  NuclideModelPtr nuc_model_shared_ptr = NuclideModelPtr(nuc_model_ptr_);
  EXPECT_NO_THROW(test_copy->copy(*lumped_shared_ptr));
  EXPECT_NO_THROW(test_copy->copy(*nuc_model_shared_ptr));
  //EXPECT_FLOAT_EQ(some_param_, test_copy->some_param());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  ASSERT_EQ(0,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  //EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  //EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(1,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  //EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  //EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(2,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  //EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  //EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, extract){ 
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?

  ASSERT_EQ(0,time_);
  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(lumped_ptr_->transportNuclides(time_));
  //EXPECT_FLOAT_EQ(test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  //EXPECT_FLOAT_EQ(test_size_, lumped_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(lumped_ptr_->transportNuclides(time_));
    //EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, lumped_ptr_->contained_mass(time_));
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, set_some_param){ 
  // the deg rate must be between 0 and 1, inclusive
  some_param_=0;
  //ASSERT_NO_THROW(lumped_ptr_->set_some_param(some_param_));
  //EXPECT_FLOAT_EQ(lumped_ptr_->some_param(), some_param_);
  some_param_=1;
  //ASSERT_NO_THROW(lumped_ptr_->set_some_param(some_param_));
  //EXPECT_FLOAT_EQ(lumped_ptr_->some_param(), some_param_);
  // it should accept floats
  some_param_= 0.1;
  //ASSERT_NO_THROW(lumped_ptr_->set_some_param(some_param_));
  //EXPECT_FLOAT_EQ(lumped_ptr_->some_param(), some_param_);
  // an exception should be thrown if it's set outside the bounds
  some_param_= -1;
  //EXPECT_THROW(lumped_ptr_->set_some_param(some_param_), CycRangeException);
  //EXPECT_NE(lumped_ptr_->some_param(), some_param_);
  some_param_= 2;
  //EXPECT_THROW(lumped_ptr_->set_some_param(some_param_), CycRangeException);
  //EXPECT_NE(lumped_ptr_->some_param(), some_param_);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, total_degradation){
  some_param_=0.3;
  int max_degs = 1.0/some_param_;
  // Check deg rate
  for(int i=0; i<10; i++){
    ASSERT_EQ(i, time_);
    //ASSERT_NO_THROW(lumped_ptr_->set_some_param(some_param_));
    //ASSERT_FLOAT_EQ(lumped_ptr_->some_param(), some_param_);
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, set_formulation){
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(DM));
  EXPECT_EQ(DM, lumped_ptr_->formulation());
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EM));
  EXPECT_EQ(EM, lumped_ptr_->formulation());
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EPM));
  EXPECT_EQ(EPM, lumped_ptr_->formulation());
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(PFM));
  EXPECT_EQ(PFM, lumped_ptr_->formulation());
  EXPECT_THROW(lumped_ptr_->set_formulation(6), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, transportNuclidesPFM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(PFM));
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double expected_conc = test_size_*exp(time_)/lumped_ptr_->V_f();

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  double expected_cauchy = 900; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
TEST_F(LumpedNuclideTest, transportNuclidesDM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(DM));
  double expected_conc = (test_size_/lumped_ptr_->V_f())/(1+time_);
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  expected_conc = (test_size_/lumped_ptr_->V_f())/(1+time_);

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  double expected_cauchy = 900; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
TEST_F(LumpedNuclideTest, transportNuclidesEM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EM));
  double Pe =  
  double expected_conc = (test_size_*exp(Pe)*pow(1-(1+(4*time_)/Pe),0.5))/(2*lumped_ptr_->V_f());
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double expected_conc = test_size_*exp(time_)/lumped_ptr_->V_f();

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  double expected_cauchy = 900; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
TEST_F(LumpedNuclideTest, transportNuclidesEPM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(PFM));
  double expected_conc = test_size_*exp(time_)/lumped_ptr_->V_f();
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double expected_conc = test_size_*exp(time_)/lumped_ptr_->V_f();

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Cauchy
  double expected_cauchy = 900; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
TEST_F(LumpedNuclideTest, updateVecHist){ 
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, contained_mass){ 
  time_++;
  //EXPECT_FLOAT_EQ(0, lumped_ptr_->contained_mass());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(LumpedNuclideModel, NuclideModelTests, Values(&LumpedNuclideModelConstructor));

