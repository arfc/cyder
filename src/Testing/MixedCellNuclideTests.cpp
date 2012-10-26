// MixedCellNuclideTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "MixedCellNuclideTests.h"
#include "NuclideModelTests.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"
#include "XMLQueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclideTest::SetUp(){
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
  porosity_ = 0;
  deg_rate_ = 0;

  // composition set up
  u235_=92235;
  one_mol_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_mol_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_mixed_cell_nuclide model setup
  mixed_cell_ptr_ = new MixedCellNuclide();
  default_mixed_cell_ptr_ = new MixedCellNuclide();
  nuc_model_ptr_ = dynamic_cast<NuclideModel*>(mixed_cell_ptr_);
  default_nuc_model_ptr_ = dynamic_cast<NuclideModel*>(mixed_cell_ptr_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclideTest::TearDown() {
  delete mixed_cell_ptr_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* MixedCellNuclideModelConstructor(){
  return dynamic_cast<NuclideModel*>(new MixedCellNuclide());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MixedCellNuclide* MixedCellNuclideTest::initNuclideModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <degradation>" << deg_rate_ << "</degradation>"
     << "  <porosity>" << porosity_ << "</porosity>"
     << "</start>";

  XMLParser parser(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  mixed_cell_ptr_ = new MixedCellNuclide();
  mixed_cell_ptr_->initModuleMembers(engine);
  delete engine;
  return mixed_cell_ptr_;  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, initial_state) {
  EXPECT_EQ(deg_rate_, mixed_cell_ptr_->deg_rate());
  EXPECT_EQ(porosity_, mixed_cell_ptr_->porosity());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, defaultConstructor) {
  ASSERT_EQ("MIXEDCELL_NUCLIDE", nuc_model_ptr_->name());
  ASSERT_EQ(MIXEDCELL_NUCLIDE, nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, mixed_cell_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, initFunctionNoXML) { 
  //EXPECT_NO_THROW(mixed_cell_ptr_->init(porosity_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, copy) {
  //ASSERT_NO_THROW(mixed_cell_ptr_->init(porosity_));
  MixedCellNuclide* test_copy = new MixedCellNuclide();
  EXPECT_NO_THROW(test_copy->copy(mixed_cell_ptr_));
  EXPECT_NO_THROW(test_copy->copy(nuc_model_ptr_));
  //EXPECT_FLOAT_EQ(porosity_, test_copy->porosity());
  delete test_copy;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  for(int i=0; i<4; i++){
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
    //EXPECT_NO_THROW(mixed_cell_ptr_->update_vec_hist(time_));
    //EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), mixed_cell_ptr_->contained_mass(time_));
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, extract){ 
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?

  ASSERT_EQ(0,time_);
  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(mixed_cell_ptr_->transportNuclides(time_));
  //EXPECT_FLOAT_EQ(test_mat_->quantity(), mixed_cell_ptr_->contained_mass(time_));
  //EXPECT_FLOAT_EQ(test_size_, mixed_cell_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(mixed_cell_ptr_->transportNuclides(time_));
    //EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, mixed_cell_ptr_->contained_mass(time_));
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, set_porosity){ 
  // the deg rate must be between 0 and 1, inclusive
  porosity_=0;
  //ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  //EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  porosity_=1;
  //ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  //EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  // it should accept floats
  porosity_= 0.1;
  //ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  //EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  // an exception should be thrown if it's set outside the bounds
  porosity_= -1;
  //EXPECT_THROW(mixed_cell_ptr_->set_porosity(porosity_), CycRangeException);
  //EXPECT_NE(mixed_cell_ptr_->porosity(), porosity_);
  porosity_= 2;
  //EXPECT_THROW(mixed_cell_ptr_->set_porosity(porosity_), CycRangeException);
  //EXPECT_NE(mixed_cell_ptr_->porosity(), porosity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, transportNuclidesZero){ 
  // for some setting, nothing should be released
  porosity_=0;
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  double expected_src = porosity_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  double radial_midpoint = outer_radius + (outer_radius - nuc_model_ptr_->geom()->inner_radius())/2;

  //ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  //EXPECT_FLOAT_EQ(porosity_, mixed_cell_ptr_->porosity());
  // get the initial mass
  //double initial_mass = mixed_cell_ptr_->contained_mass();
  // transport the nuclides
  //EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));
  // check that the contained mass matches the initial mass
  //EXPECT_FLOAT_EQ(initial_mass, mixed_cell_ptr_->contained_mass()); 
  // check the source term 
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->source_term_bc().second);
  // check the boundary concentration ?
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->dirichlet_bc(u235_));
  // check the boundary flux
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(u235_));
  // check the neumann bc
  EXPECT_FLOAT_EQ(0 , nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, transportNuclidesOther){ 
  // if the degradation rate is .5, everything should be released in two years
  porosity_= 0.5;
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  double expected_src = porosity_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  double expected_conc_w_vel = theta_*adv_vel_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // set the degradation rate
  //ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  //EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
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
  double expected_cauchy = 900; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann
  //double expected_neumann= -expected_conc/(outer_radius*2 - mixed_cell_ptr_->radial_midpoint());
  //EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));

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
  // Cauchy
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann 
  //expected_neumann= -expected_conc/(outer_radius*2 - mixed_cell_ptr_->radial_midpoint());
  //EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->cauchy_bc(u235_));
  // Neumann
  EXPECT_FLOAT_EQ(0, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, contained_mass){ 
  time_++;
  //EXPECT_FLOAT_EQ(0, mixed_cell_ptr_->contained_mass());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(MixedCellNuclideModel, NuclideModelTests, Values(&MixedCellNuclideModelConstructor));

