// OneDimPPMNuclideTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>
#include <boost/math/special_functions/erf.hpp>

#include "OneDimPPMNuclideTests.h"
#include "NuclideModelTests.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"
#include "XMLQueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclideTest::SetUp(){
  // set up geometry. this usually happens in the component init
  r_four_ = 4;
  r_five_ = 5;
  point_t origin_ = {0,0,0}; 
  len_five_ = 5;
  geom_ = GeometryPtr(new Geometry(r_four_, r_five_, origin_, len_five_));

  // other vars
  theta_ = 0.3; // percent porosity
  v_ = 1; // m/yr
  time_ = 0;
  Ci_ = 1;
  Co_ = 2;
  porosity_ = 0.1;
  D_ = 4;
  rho_ = 5;
  Kd_ = 6;

  // composition set up
  u235_=92235;
  am241_=95241;
  one_kg_=1.0;
  test_comp_= CompMapPtr(new CompMap(MASS));
  (*test_comp_)[u235_] = one_kg_;
  test_size_=10.0;

  // material creation
  test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
  test_mat_->setQuantity(test_size_);

  // test_one_dim_ppm_nuclide model setup
  mat_table_ = MDB->table("clay");
  one_dim_ppm_ptr_ = OneDimPPMNuclidePtr(initNuclideModel());
  one_dim_ppm_ptr_->set_mat_table(mat_table_);
  one_dim_ppm_ptr_->set_geom(geom_);
  nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(one_dim_ppm_ptr_);
  nuc_model_ptr_->set_mat_table(mat_table_);
  default_one_dim_ppm_ptr_ = OneDimPPMNuclidePtr(OneDimPPMNuclide::create());
  default_nuc_model_ptr_ = boost::dynamic_pointer_cast<NuclideModel>(default_one_dim_ppm_ptr_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclideTest::TearDown() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double OneDimPPMNuclideTest::calculate_conc(IsoConcMap C_0, IsoConcMap C_i, double r, Iso iso, int t0, int t){
  double D = mat_table_->D(iso/1000);
  double pi = boost::math::constants::pi<double>();
  //@TODO add sorption to this model. For now, R=1, no sorption. 
  double R=1;
  //@TODO convert months to seconds

  double At_frac = (R*r - v_*t)/(2*pow(D*R*t, 0.5));
  double At = 0.5*boost::math::erfc(At_frac);
  double Att0_frac = (R*r - v_*t)/(2*pow(D*R*(t-t0), 0.5));
  double Att0 = 0.5*boost::math::erfc(Att0_frac);
  double A = At - Att0;
  double B1_frac = (R*r - v_*t)/(2*pow(D*R*t, 0.5));
  double B1 = 0.5*boost::math::erfc(B1_frac);
  double B2_exp = -pow(R*r-v_*t,2)/(4*D*R*t);
  double B2 = pow(v_*v_*t/(pi*R*D),0.5)*exp(B2_exp);
  double B3_exp = v_*r/D;
  double B3_frac = (R*r + v_*t)/(2*pow(D*R*t,0.5));
  double B3_factor = -0.5*(1+(v_*r)/(D) + (v_*v_*t)/(D*R));
  double B3 = B3_factor*exp(B3_exp)*boost::math::erfc(B3_frac);  ;
  double B = C_i[iso]*(B1 + B2 + B3);

  double to_ret = C_0[iso]*A + B;
  
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclideTest::calculate_conc(IsoConcMap C_0, double r, int the_time) {
  IsoConcMap to_ret;
  IsoConcMap::iterator it;
  Iso iso;
  for( it=C_0.begin(); it!=C_0.end(); ++it){
    iso = (*it).first;
    to_ret[iso] = calculate_conc(C_0, C_0,  r, iso, the_time-1, the_time);
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModelPtr OneDimPPMNuclideModelConstructor (){
  return boost::dynamic_pointer_cast<NuclideModel>(OneDimPPMNuclide::create());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
OneDimPPMNuclidePtr OneDimPPMNuclideTest::initNuclideModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <advective_velocity>" << v_ << "</advective_velocity>"
     << "  <porosity>" << porosity_ << "</porosity>"
     << "  <bulk_density>" << rho_ << "</bulk_density>"
     << "</start>";

  XMLParser parser;
  parser.init(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  OneDimPPMNuclidePtr one_dim_ppm_ptr = OneDimPPMNuclidePtr(OneDimPPMNuclide::create());
  one_dim_ppm_ptr->initModuleMembers(engine);
  delete engine;
  return one_dim_ppm_ptr;  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, initial_state) {
  EXPECT_EQ(v_, one_dim_ppm_ptr_->v());
  EXPECT_EQ(rho_, one_dim_ppm_ptr_->rho());
  EXPECT_EQ(porosity_, one_dim_ppm_ptr_->porosity());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, defaultConstructor) {
  ASSERT_EQ("ONEDIMPPM_NUCLIDE", default_one_dim_ppm_ptr_->name());
  ASSERT_EQ(ONEDIMPPM_NUCLIDE, default_one_dim_ppm_ptr_->type());
  ASSERT_FLOAT_EQ(0, default_one_dim_ppm_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, copy) {
  OneDimPPMNuclidePtr test_copy = OneDimPPMNuclidePtr(OneDimPPMNuclide::create());
  OneDimPPMNuclidePtr one_dim_ppm_shared_ptr = OneDimPPMNuclidePtr(one_dim_ppm_ptr_);
  NuclideModelPtr nuc_model_shared_ptr = NuclideModelPtr(nuc_model_ptr_);
  EXPECT_NO_THROW(test_copy->copy(*one_dim_ppm_shared_ptr));
  EXPECT_NO_THROW(test_copy->copy(*nuc_model_shared_ptr));
  EXPECT_FLOAT_EQ(porosity_, test_copy->porosity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, absorb){
  // if you absorb a material, the conc_map should reflect that
  // you shouldn't absorb more material than you can handle. how much is that?
  for(int i=0; i<4; i++){
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
    EXPECT_NO_THROW(one_dim_ppm_ptr_->update_vec_hist(time_));
    EXPECT_FLOAT_EQ((1+time_)*test_mat_->quantity(), one_dim_ppm_ptr_->contained_mass(time_));
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, extract){ 
  // it should be able to extract all of the material it absorbed
  double frac = 0.2;
  
  // if you extract a material, the conc_map should reflect that
  // you shouldn't extract more material than you have how much is that?

  ASSERT_EQ(0,time_);
  ASSERT_NO_THROW(nuc_model_ptr_->absorb(test_mat_));
  EXPECT_NO_THROW(one_dim_ppm_ptr_->transportNuclides(time_));
  EXPECT_FLOAT_EQ(test_mat_->quantity(), one_dim_ppm_ptr_->contained_mass(time_));
  EXPECT_FLOAT_EQ(test_size_, one_dim_ppm_ptr_->contained_mass(time_));

  for(int i=1; i<4; i++){
    time_++;
    ASSERT_EQ(i,time_);
    EXPECT_NO_THROW(nuc_model_ptr_->extract(test_comp_, frac*test_size_));
    EXPECT_NO_THROW(one_dim_ppm_ptr_->transportNuclides(time_));
    EXPECT_FLOAT_EQ((1 - frac*time_)*test_size_, one_dim_ppm_ptr_->contained_mass(time_));
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, set_porosity){ 
  // the deg rate must be between 0 and 1, inclusive
  porosity_=0;
  ASSERT_NO_THROW(one_dim_ppm_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(one_dim_ppm_ptr_->porosity(), porosity_);
  porosity_=1;
  ASSERT_NO_THROW(one_dim_ppm_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(one_dim_ppm_ptr_->porosity(), porosity_);
  // it should accept floats
  porosity_= 0.1;
  ASSERT_NO_THROW(one_dim_ppm_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(one_dim_ppm_ptr_->porosity(), porosity_);
  // an exception should be thrown if it's set outside the bounds
  porosity_= -1;
  EXPECT_THROW(one_dim_ppm_ptr_->set_porosity(porosity_), CycRangeException);
  EXPECT_NE(one_dim_ppm_ptr_->porosity(), porosity_);
  porosity_= 2;
  EXPECT_THROW(one_dim_ppm_ptr_->set_porosity(porosity_), CycRangeException);
  EXPECT_NE(one_dim_ppm_ptr_->porosity(), porosity_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, transportNuclidesZero){ 
  // for some settings, nothing should be released
  porosity_=0;
  double expected_src = porosity_*test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume());
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();
  double radial_midpoint = outer_radius + (outer_radius - nuc_model_ptr_->geom()->inner_radius())/2;

  ASSERT_NO_THROW(one_dim_ppm_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(porosity_, one_dim_ppm_ptr_->porosity());
  // get the initial mass
  double initial_mass = one_dim_ppm_ptr_->contained_mass(time_);
  // transport the nuclides
  EXPECT_NO_THROW(nuc_model_ptr_->transportNuclides(time_++));
  // check that the contained mass matches the initial mass
  EXPECT_FLOAT_EQ(initial_mass, one_dim_ppm_ptr_->contained_mass(time_)); 
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
TEST_F(OneDimPPMNuclideTest, transportNuclidesOther){ 
  // if the degradation rate is .5, everything should be released in two years
  porosity_= 0.5;
  double expected_src = test_size_;
  double expected_conc = expected_src/(nuc_model_ptr_->geom()->volume()*porosity_);
  double expected_conc_w_vel = theta_*v_*expected_conc; 
  IsoConcMap zero_conc_map;
  zero_conc_map[92235] = 0;
  double outer_radius = nuc_model_ptr_->geom()->outer_radius();

  // set the porosity
  ASSERT_NO_THROW(one_dim_ppm_ptr_->set_porosity(porosity_));
  EXPECT_FLOAT_EQ(one_dim_ppm_ptr_->porosity(), porosity_);
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
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann
  double expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2,u235_));

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
  EXPECT_FLOAT_EQ(expected_cauchy, nuc_model_ptr_->cauchy_bc(zero_conc_map, outer_radius*2, u235_));
  // Neumann 
  expected_neumann= -expected_conc/(outer_radius*2 - geom_->radial_midpoint());
  EXPECT_FLOAT_EQ(expected_neumann, nuc_model_ptr_->neumann_bc(zero_conc_map, outer_radius*2, u235_));

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
TEST_F(OneDimPPMNuclideTest, Azt){
  double expected = 0.0000000000076; 
  EXPECT_FLOAT_EQ(0.00000000000001, pow(10.,-14));
  EXPECT_FLOAT_EQ(2629740.0, SECSPERMONTH );
  double actual = one_dim_ppm_ptr_->Azt(1, 1, pow(10.,-14), SECSPERMONTH , pow(10.,-8));  
  EXPECT_FLOAT_EQ(expected, actual);
  for(int i =0; i<15; ++i){
    double actual = one_dim_ppm_ptr_->Azt(1, 1, pow(10.,-i), SECSPERMONTH , pow(10.,-8));  
    EXPECT_FLOAT_EQ(expected, actual);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, Bzt){
  double actual = one_dim_ppm_ptr_->Bzt(1, 1, pow(10.,-14), SECSPERMONTH , pow(10.,-8), 0.99);  
  double expected = 0;
  EXPECT_FLOAT_EQ(0.00000000000001, pow(10.,-14));
  EXPECT_FLOAT_EQ(2629740.0, SECSPERMONTH );
  EXPECT_FLOAT_EQ(expected, actual);
  for(int i =0; i<15; ++i){
    double actual = one_dim_ppm_ptr_->Bzt(1, 1, pow(10.,-i), SECSPERMONTH , pow(10.,-8), 0.99);  
    EXPECT_FLOAT_EQ(expected, actual);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, trap_rule){
  double a=10;
  double b=20;
  int n=11;
  double r;
  map<double, IsoConcMap> rfmap; 
  for(int i = a; i <= b; ++i){
    IsoConcMap rf; 
    rf[u235_] = 1.0;
    r = i;
    rfmap[a] = rf;
  }
  double expected = 1*(b-a); 
  IsoConcMap actual_map = one_dim_ppm_ptr_->trap_rule(a, b, n, rfmap);
  double actual = actual_map[u235_];
  EXPECT_FLOAT_EQ(expected, actual);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(OneDimPPMNuclideTest, contained_mass){ 
  time_++;
  EXPECT_FLOAT_EQ(0, one_dim_ppm_ptr_->contained_mass(time_));

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(OneDimPPMNuclideModel, NuclideModelTests, Values(&OneDimPPMNuclideModelConstructor));

