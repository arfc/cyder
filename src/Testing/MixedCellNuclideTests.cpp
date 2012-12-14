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
  porosity_ = 0.1;
  deg_rate_ = 0.1;
  kd_limited_ = 0; // false
  sol_limited_ = 0; // false

  false_str_ = "false"; // false


  // composition set up
  u235_=92235;
  u_=92;
  one_mol_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_mol_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_mixed_cell_nuclide model setup
  mat_table_ = MDB->table("clay");
  mixed_cell_ptr_ = MixedCellNuclidePtr(initNuclideModel());
  nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(mixed_cell_ptr_);
  mixed_cell_ptr_->set_mat_table(mat_table_);
  default_mixed_cell_ptr_ = MixedCellNuclidePtr(MixedCellNuclide::create());
  default_nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(default_mixed_cell_ptr_);
  default_mixed_cell_ptr_->set_mat_table(mat_table_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclideTest::TearDown() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModelPtr MixedCellNuclideModelConstructor (){
  return boost::dynamic_pointer_cast<NuclideModel>(MixedCellNuclide::create());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MixedCellNuclidePtr MixedCellNuclideTest::initNuclideModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <advective_velocity>" << adv_vel_ << "</advective_velocity>"
     << "  <degradation>" << deg_rate_ << "</degradation>"
     << "  <kd_limited>" << kd_limited_ << "</kd_limited>"
     << "  <porosity>" << porosity_ << "</porosity>"
     << "  <sol_limited>" << sol_limited_ << "</sol_limited>"
     << "</start>";

  XMLParser parser(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  mixed_cell_ptr_ = MixedCellNuclidePtr(MixedCellNuclide::create(engine));
  delete engine;
  return mixed_cell_ptr_;  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, initial_state) {
  EXPECT_EQ(deg_rate_, mixed_cell_ptr_->deg_rate());
  EXPECT_EQ(false, mixed_cell_ptr_->kd_limited());
  EXPECT_EQ(porosity_, mixed_cell_ptr_->porosity());
  EXPECT_EQ(adv_vel_, mixed_cell_ptr_->v());
  EXPECT_EQ(false, mixed_cell_ptr_->sol_limited());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, defaultConstructor) {
  ASSERT_EQ("MIXEDCELL_NUCLIDE", default_nuc_model_ptr_->name());
  ASSERT_EQ(MIXEDCELL_NUCLIDE, default_nuc_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, default_mixed_cell_ptr_->deg_rate());
  ASSERT_EQ(true, default_mixed_cell_ptr_->kd_limited());
  ASSERT_EQ(true, default_mixed_cell_ptr_->sol_limited());
  ASSERT_FLOAT_EQ(0, default_mixed_cell_ptr_->geom()->length());
  ASSERT_FLOAT_EQ(0, default_mixed_cell_ptr_->contained_mass());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, copy) {
  MixedCellNuclidePtr test_copy = MixedCellNuclidePtr(MixedCellNuclide::create());
  MixedCellNuclidePtr mixed_cell_shared_ptr = MixedCellNuclidePtr(mixed_cell_ptr_);
  NuclideModelPtr nuc_model_shared_ptr = NuclideModelPtr(nuc_model_ptr_);
  EXPECT_NO_THROW(test_copy->copy(*mixed_cell_shared_ptr));
  EXPECT_NO_THROW(test_copy->copy(*nuc_model_shared_ptr));
  EXPECT_FLOAT_EQ(porosity_, test_copy->porosity());
  EXPECT_FLOAT_EQ(sol_limited_, test_copy->sol_limited());
  EXPECT_FLOAT_EQ(kd_limited_, test_copy->kd_limited());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  for(int i=0; i<4; i++){
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
    EXPECT_NO_THROW(mixed_cell_ptr_->update_vec_hist(time_));
    EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), nuc_model_ptr_->contained_mass(time_));
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, extract){ 
  // @TODO tests like this should be interface tests for NuclideModel class concrete instances
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?

  ASSERT_EQ(0,time_);
  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(mixed_cell_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), mixed_cell_ptr_->contained_mass());
  EXPECT_FLOAT_EQ(test_size_, nuc_model_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(mixed_cell_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, mixed_cell_ptr_->contained_mass());
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, set_deg_rate){ 
  // the deg rate must be between 0 and 1, inclusive
  deg_rate_=0;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);
  deg_rate_=1;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);
  // it should accept floats
  deg_rate_= 0.1;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);
  // an exception should be thrown if it's set outside the bounds
  deg_rate_= -1;
  EXPECT_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(mixed_cell_ptr_->deg_rate(), deg_rate_);
  deg_rate_= 2;
  EXPECT_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_), CycRangeException);
  EXPECT_NE(mixed_cell_ptr_->deg_rate(), deg_rate_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, set_porosity){ 
  // the deg rate must be between 0 and 1, inclusive
  porosity_=0;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  porosity_=1;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  // it should accept floats
  porosity_= 0.1;
  ASSERT_NO_THROW(mixed_cell_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->porosity(), porosity_);
  // an exception should be thrown if it's set outside the bounds
  porosity_= -1;
  EXPECT_THROW(mixed_cell_ptr_->set_porosity(porosity_), CycRangeException);
  EXPECT_NE(mixed_cell_ptr_->porosity(), porosity_);
  porosity_= 2;
  EXPECT_THROW(mixed_cell_ptr_->set_porosity(porosity_), CycRangeException);
  EXPECT_NE(mixed_cell_ptr_->porosity(), porosity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, total_degradation){
  deg_rate_=0.3;
  int max_degs = 1.0/deg_rate_;
  // Check deg rate
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  ASSERT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);

  for(int i=0; i<4; i++){
    ASSERT_EQ(i, time_);
    ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ(deg_rate_*time_++, mixed_cell_ptr_->tot_deg());
  }

  ASSERT_EQ(4, time_);
  ASSERT_TRUE(time_ > max_degs);
  ASSERT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(1, mixed_cell_ptr_->tot_deg());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, transportNuclidesDR0){ 
  // if the degradation rate is zero, nothing should be released
  // set the degradation rate
  deg_rate_=0;
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  double radial_midpoint = outer_radius + (outer_radius - nuc_model_ptr_->geom()->inner_radius())/2;

  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(deg_rate_, mixed_cell_ptr_->deg_rate());
  // get the initial mass
  double initial_mass = mixed_cell_ptr_->contained_mass();
  // transport the nuclides
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));
  // check that the contained mass matches the initial mass
  EXPECT_FLOAT_EQ(initial_mass, mixed_cell_ptr_->contained_mass()); 
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
TEST_F(MixedCellNuclideTest, transportNuclidesDRhalf){ 
  // if the degradation rate is .5, everything should be released in two years
  deg_rate_= 0.5;
  // set the degradation rate
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);
  // set geometry
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc;
  ASSERT_FLOAT_EQ(0, mixed_cell_ptr_->tot_deg());
  ASSERT_FLOAT_EQ(0, mixed_cell_ptr_->V_ff());
  expected_conc = 0;
  double expected_conc_w_vel = theta_*adv_vel_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  if(mixed_cell_ptr_->sol_limited()){
    double sol_lim = mat_table_->S(u_);
    expected_conc = min(sol_lim, expected_conc);
  }

  // TRANSPORT NUCLIDES 
  ASSERT_EQ(0, time_);
  time_++;
  ASSERT_EQ(1, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  ASSERT_GT(mixed_cell_ptr_->V_ff(),0);
  expected_conc = expected_src/(mixed_cell_ptr_->V_ff());

  // check that half that material is offered as the source term in one year
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann
  double expected_neumann= -expected_conc/(outer_radius*2 - mixed_cell_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));
  // Cauchy
  double expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; // @TODO fix units everywhere
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  CompMapPtr extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  double extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES 
  time_++;
  ASSERT_EQ(2, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  ASSERT_GT(mixed_cell_ptr_->V_ff(),0);
  expected_conc = expected_src/(mixed_cell_ptr_->V_ff());

  // check that the remaining half is offered as the source term in year two
  // Source Term
  EXPECT_FLOAT_EQ(expected_src, nuc_model_ptr_->source_term_bc().second);
  // Dirichlet
  EXPECT_FLOAT_EQ(expected_conc, nuc_model_ptr_->dirichlet_bc(u235_));
  // Neumann 
  expected_neumann= -expected_conc/(outer_radius*2 - mixed_cell_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; // @TODO fix
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));

  // remove the source term offered
  extract_comp = nuc_model_ptr_->source_term_bc().first.comp();
  extract_mass = nuc_model_ptr_->source_term_bc().second;
  EXPECT_NO_THROW(nuc_model_ptr_->extract(extract_comp, extract_mass));
  // TRANSPORT NUCLIDES 
  time_++;
  ASSERT_EQ(3, time_);
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_));
  ASSERT_GT(mixed_cell_ptr_->V_ff(), 0);
  expected_conc = expected_src/(mixed_cell_ptr_->V_ff());

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
TEST_F(MixedCellNuclideTest, transportNuclidesDR1){ 
  // if the degradation rate is one, everything should be released in a timestep
  deg_rate_= 1;
  // set the degradation rate
  ASSERT_NO_THROW(mixed_cell_ptr_->set_deg_rate(deg_rate_));
  EXPECT_FLOAT_EQ(mixed_cell_ptr_->deg_rate(), deg_rate_);
  // fill it with some material
  EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  // set the geometry
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  double expected_src = deg_rate_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  double expected_conc_w_vel = theta_*adv_vel_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  if(mixed_cell_ptr_->sol_limited() && mixed_cell_ptr_->kd_limited()){
    double sol_lim = mat_table_->S(u_);
    expected_conc = min(sol_lim, expected_conc);
  };

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
  double expected_neumann= -expected_conc/(outer_radius*2 - mixed_cell_ptr_->geom()->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));
  // Cauchy
  double expected_cauchy = -mat_table_->D(u_)*expected_neumann + adv_vel_*expected_conc; // @TODO fix
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
TEST_F(MixedCellNuclideTest, transportNuclidesDRsmall){ 
  // if the degradation rate is very very small, see if the model behaves well 
  // in the long term. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, updateVecHist){ 
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, contained_mass){ 
  time_++;
  EXPECT_FLOAT_EQ(0, mixed_cell_ptr_->contained_mass());

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, updateDegradation){ 
  double deg_rate=0.1;
   
  for(int i=0; i<5; i++){
    time_++;
    EXPECT_NO_THROW(mixed_cell_ptr_->update_degradation(time_, deg_rate));
    EXPECT_EQ(time_*deg_rate,mixed_cell_ptr_->tot_deg());
  }
  EXPECT_NO_THROW(mixed_cell_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,mixed_cell_ptr_->tot_deg());
  EXPECT_NO_THROW(mixed_cell_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,mixed_cell_ptr_->tot_deg());
  EXPECT_NO_THROW(mixed_cell_ptr_->update_degradation(time_, deg_rate));
  EXPECT_EQ(time_*deg_rate,mixed_cell_ptr_->tot_deg());
  time_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, setGeometry) {  
  //@TODO tests like this should be interface tests for the NuclideModel class concrete instances.
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  EXPECT_FLOAT_EQ(len_five_ , nuc_model_ptr_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , nuc_model_ptr_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , nuc_model_ptr_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, getVolume) {  
  EXPECT_NO_THROW(mixed_cell_ptr_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuc_model_ptr_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(mixed_cell_ptr_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , nuc_model_ptr_->geom()->volume());
  EXPECT_NO_THROW(mixed_cell_ptr_->geom()->set_radius(OUTER, numeric_limits<double>::infinity()));
  EXPECT_FLOAT_EQ( numeric_limits<double>::infinity(), nuc_model_ptr_->geom()->volume());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, calc_conc_grad) {  
  Concentration c_out, c_in;
  Radius r_out, r_in;
  c_out = 0;
  c_in = 2;
  r_out = 5;
  r_in = 4;

  EXPECT_FLOAT_EQ( (c_out-c_in)/(r_out-r_in), mixed_cell_ptr_->calc_conc_grad(c_out, c_in, r_out, r_in)); 
  EXPECT_THROW( mixed_cell_ptr_->calc_conc_grad(c_out, c_in, r_in, r_out), CycRangeException); 
  EXPECT_THROW( mixed_cell_ptr_->calc_conc_grad(c_out, c_in, r_in, r_in), CycRangeException); 
  EXPECT_THROW( mixed_cell_ptr_->calc_conc_grad(c_out, c_in, r_in, numeric_limits<double>::infinity()), CycRangeException); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, sorb){
  mixed_cell_ptr_->set_kd_limited(true);
  ASSERT_TRUE(mixed_cell_ptr_->kd_limited());
  EXPECT_NO_THROW(mixed_cell_ptr_->sorb(time_, u_, 10));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, dont_sorb){
  // if kd_limited = false, don't call sorb()
  EXPECT_THROW( mixed_cell_ptr_->sorb(time_, u_, 10), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, precipitate){
  mixed_cell_ptr_->set_sol_limited(true);
  ASSERT_TRUE(mixed_cell_ptr_->sol_limited());
  EXPECT_NO_THROW(mixed_cell_ptr_->precipitate(time_, u_, 10));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MixedCellNuclideTest, dont_precipitate){
  // if sol_limited = false, don't call precipitate()
  EXPECT_THROW(mixed_cell_ptr_->precipitate(time_, u_, 10), CycException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(MixedCellNuclideModel, NuclideModelTests, Values(&MixedCellNuclideModelConstructor));
