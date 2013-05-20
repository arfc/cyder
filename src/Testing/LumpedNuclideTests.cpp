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
  t_t_ = 1; ///?  

  // composition set up
  u_=92;
  u235_=92235;
  am241_=95241;
  one_kg_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_kg_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_C_0
  test_C_0_ = MatTools::comp_to_conc_map(test_comp_,test_size_,geom_->volume());
  // test_lumped_nuclide model setup
  mat_table_ = MDB->table("clay");
  lumped_ptr_=LumpedNuclidePtr(initNuclideModel());
  lumped_ptr_->set_mat_table(mat_table_);
  lumped_ptr_->set_geom(geom_);
  nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(lumped_ptr_);
  nuc_model_ptr_->set_mat_table(mat_table_);
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
     << "  <porosity>" << theta_ << "</porosity>"
     << "  <transit_time>" << t_t_ << "</transit_time>"
     << "  <formulation>"
     << "    <EXPM/>"
     << "  </formulation>"
     << "</start>";

  ///@TODO  test other formulation stringstreams... 
  XMLParser parser;
  parser.init(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  lumped_ptr_ = LumpedNuclidePtr(LumpedNuclide::create());
  lumped_ptr_->initModuleMembers(engine);
  delete engine;
  return lumped_ptr_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, initial_state){
  EXPECT_EQ(FormulationType(EXPM), lumped_ptr_->formulation());
  EXPECT_EQ(adv_vel_, lumped_ptr_->v());
  EXPECT_EQ(t_t_, lumped_ptr_->t_t());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, defaultConstructor) {
  ASSERT_EQ("LUMPED_NUCLIDE", default_nuc_model_ptr_->name());
  ASSERT_EQ(LUMPED_NUCLIDE, default_nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, default_lumped_ptr_->geom()->length());
  ASSERT_FLOAT_EQ(0, default_lumped_ptr_->v());
  ASSERT_FLOAT_EQ(0, default_lumped_ptr_->t_t());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, copy) {
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  LumpedNuclidePtr test_copy = LumpedNuclidePtr(LumpedNuclide::create());
  LumpedNuclidePtr lumped_shared_ptr = LumpedNuclidePtr(lumped_ptr_);
  NuclideModelPtr nuc_model_shared_ptr = NuclideModelPtr(nuc_model_ptr_);
  EXPECT_NO_THROW(test_copy->copy(*lumped_shared_ptr));
  EXPECT_NO_THROW(test_copy->copy(*nuc_model_shared_ptr));
  EXPECT_FLOAT_EQ(lumped_ptr_->V_T(), test_copy->V_T());
  EXPECT_GT(test_copy->V_T(),0);
  EXPECT_FLOAT_EQ(lumped_ptr_->V_f(), test_copy->V_f());
  EXPECT_FLOAT_EQ(lumped_ptr_->V_s(), test_copy->V_s());
  EXPECT_FLOAT_EQ(lumped_ptr_->porosity(), test_copy->porosity());
  EXPECT_EQ(lumped_ptr_->formulation(), test_copy->formulation());
  EXPECT_FLOAT_EQ(lumped_ptr_->last_updated(), test_copy->last_updated());
  EXPECT_FLOAT_EQ(lumped_ptr_->v(), test_copy->v());
  EXPECT_FLOAT_EQ(lumped_ptr_->t_t(), test_copy->t_t());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  ASSERT_EQ(0,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(1,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  time_++;
  ASSERT_EQ(2,time_);
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
  EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
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
  EXPECT_FLOAT_EQ(test_mat_->quantity(), lumped_ptr_->contained_mass(time_));
  EXPECT_FLOAT_EQ(test_size_, lumped_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(lumped_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, lumped_ptr_->contained_mass(time_));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, set_porosity){ 
  // the deg rate must be between 0 and 1, inclusive
  theta_=0;
  ASSERT_NO_THROW(lumped_ptr_->set_porosity(theta_));
  EXPECT_FLOAT_EQ(lumped_ptr_->porosity(), theta_);
  theta_=1;
  ASSERT_NO_THROW(lumped_ptr_->set_porosity(theta_));
  EXPECT_FLOAT_EQ(lumped_ptr_->porosity(), theta_);
  // it should accept floats
  theta_= 0.1;
  ASSERT_NO_THROW(lumped_ptr_->set_porosity(theta_));
  EXPECT_FLOAT_EQ(lumped_ptr_->porosity(), theta_);
  // an exception should be thrown if it's set outside the bounds
  theta_= -1;
  EXPECT_THROW(lumped_ptr_->set_porosity(theta_), CycRangeException);
  EXPECT_NE(lumped_ptr_->porosity(), theta_);
  theta_= 2;
  EXPECT_THROW(lumped_ptr_->set_porosity(theta_), CycRangeException);
  EXPECT_NE(lumped_ptr_->porosity(), theta_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, set_formulation){
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(DM));
  EXPECT_EQ(DM, lumped_ptr_->formulation());
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EXPM));
  EXPECT_EQ(EXPM, lumped_ptr_->formulation());
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(PFM));
  EXPECT_EQ(PFM, lumped_ptr_->formulation());
  EXPECT_THROW(lumped_ptr_->set_formulation("OTHER"), CycException);
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(LAST_FORMULATION_TYPE));
  EXPECT_EQ(LAST_FORMULATION_TYPE, lumped_ptr_->formulation());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, transportNuclidesPFM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(PFM));
  IsoConcMap zero_conc_map;
  zero_conc_map[u235_] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  double radial_mid = geom_->radial_midpoint();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that the expected amt of material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double expected_conc = test_C_0_[u235_]*exp(-t_t_);

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  double expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; /// @TODO fix units everywhere
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  /// @TODO add behavior for later timesteps.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, transportNuclidesDM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  double Pe = 0.1;
  lumped_ptr_->set_Pe(Pe);
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(DM));
  IsoConcMap zero_conc_map;
  zero_conc_map[u235_] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double pow_arg = (Pe/2)*(1-pow(1+4*t_t_/Pe, 0.5));
  double exponent = exp(pow_arg);
  double expected_conc = (test_C_0_[u235_]*exponent);

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  /// @TODO fix units everywhere
  double expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; 
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  /// @TODO add behavior for later timesteps.
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, transportNuclidesEXPM){ 
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EXPM));
  IsoConcMap zero_conc_map;
  zero_conc_map[u235_] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));

  // check that half that material is offered as the source term in one timestep
  // TRANSPORT NUCLIDES
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  double expected_conc = test_C_0_[u235_]/(1+time_);

  // Source Term
  EXPECT_FLOAT_EQ(expected_conc*(lumped_ptr_->V_f()), nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  /// @TODO fix units everywhere
  double expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; 
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));

  /// @TODO add behavior for later timesteps.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, updateVecHist){ 
  time_++;
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_NO_THROW(lumped_ptr_->set_formulation(EXPM));

  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  lumped_ptr_->set_C_0(test_C_0_);
  EXPECT_NO_THROW(lumped_ptr_->update_vec_hist(time_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, setC_0){
  EXPECT_NO_THROW(lumped_ptr_->set_C_0(test_C_0_));
  IsoConcMap actual = lumped_ptr_->C_0();
  EXPECT_FLOAT_EQ(test_C_0_[u235_], actual[u235_] );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, setGeometry) {  
  /// @TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  EXPECT_FLOAT_EQ(len_five_ , nuc_model_ptr_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , nuc_model_ptr_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , nuc_model_ptr_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, getVolume) {  
  EXPECT_NO_THROW(lumped_ptr_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(lumped_ptr_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , nuc_model_ptr_->geom()->volume());
  EXPECT_THROW(lumped_ptr_->geom()->set_radius(OUTER, numeric_limits<double>::infinity()), CycRangeException);
  EXPECT_NO_THROW(nuc_model_ptr_->geom()->volume());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, C_DM){
  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  double Pe = 0.1;
  lumped_ptr_->set_Pe(Pe);
  lumped_ptr_->set_formulation(DM);

  IsoConcMap conc_map;
  conc_map[u235_] = 10;
  conc_map[95242] = 10;

  IsoConcMap expected;
  expected[u235_] = 10*exp(Pe*(1-pow(1+4*t_t_/Pe,0.5))/2);
  expected[95242] = 10*exp(Pe*(1-pow(1+4*t_t_/Pe,0.5))/2);


  EXPECT_NO_THROW(lumped_ptr_->update_conc_hist(time_, mats));
  EXPECT_NO_THROW(lumped_ptr_->C_DM( conc_map, time_ ));
  EXPECT_FLOAT_EQ(expected[u235_],  lumped_ptr_->C_DM( conc_map, time_ )[u235_]);
  EXPECT_FLOAT_EQ(expected[95242],  lumped_ptr_->C_DM( conc_map, time_ )[95242]);
  time_++;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, C_EXPM){
  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  lumped_ptr_->set_formulation(EXPM);

  IsoConcMap conc_map;
  conc_map[u235_] = 10;
  conc_map[95242] = 10;

  IsoConcMap expected;
  expected[u235_] = 10/(1+t_t_);
  expected[95242] = 10/(1+t_t_);

  EXPECT_NO_THROW(lumped_ptr_->update_conc_hist(time_, mats));
  EXPECT_NO_THROW(lumped_ptr_->C_DM( conc_map, time_ ));
  EXPECT_FLOAT_EQ(expected[u235_],  lumped_ptr_->C_EXPM( conc_map, time_ )[u235_]);
  EXPECT_FLOAT_EQ(expected[95242],  lumped_ptr_->C_EXPM( conc_map, time_ )[95242]);
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, C_PFM){
  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  lumped_ptr_->set_formulation(PFM);

  IsoConcMap conc_map;
  conc_map[u235_] = 10;
  conc_map[95242] = 10;

  IsoConcMap expected;
  expected[u235_] = 10*exp(-t_t_);
  expected[95242] = 10*exp(-t_t_);

  EXPECT_NO_THROW(lumped_ptr_->update_conc_hist(time_, mats));
  EXPECT_NO_THROW(lumped_ptr_->C_DM( conc_map, time_ ));
  EXPECT_FLOAT_EQ(expected[u235_],  lumped_ptr_->C_PFM( conc_map, time_ )[u235_]);
  EXPECT_FLOAT_EQ(expected[95242],  lumped_ptr_->C_PFM( conc_map, time_ )[95242]);
  EXPECT_FLOAT_EQ(expected[95242],  lumped_ptr_->C_t( conc_map, time_ )[95242]);
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(LumpedNuclideTest, C_LAST_FORMULATION_TYPE){
  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  lumped_ptr_->set_formulation(LAST_FORMULATION_TYPE);
  EXPECT_THROW(lumped_ptr_->update_conc_hist(time_, mats), CycException);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(LumpedNuclideModel, NuclideModelTests, Values(&LumpedNuclideModelConstructor));

