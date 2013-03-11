// STCThermalTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "STCThermalTests.h"
#include "ThermalModelTests.h"
#include "CycException.h"
#include "Material.h"
#include "MaterialDB.h"
#include "XMLQueryEngine.h"

using namespace std;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermalTest::SetUp(){
  // set up geometry. this usually happens in the component init
  r_four_ = 4;
  r_five_ = 5;
  point_t origin_ = {0,0,0}; 
  len_five_ = 5;
  geom_ = GeometryPtr(new Geometry(r_four_, r_five_, origin_, len_five_));

  // other vars
  k_th_ = .1; // ___  @TODO worry about units
  spacing_ = .1; // ___ @TODO worry about units
  alpha_th_ = .1; // ___ @TODO worry about units
  time_ = 0;
  mat_ = "clay";
  mat_table_=MDB->table(mat_);

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

  // test_stc_thermal model setup
  stc_ptr_ = STCThermalPtr(initThermalModel()); //initializes stc_ptr_
  therm_model_ptr_ = boost::dynamic_pointer_cast<ThermalModel>(stc_ptr_);
  stc_ptr_->set_mat_table(mat_table_);
  stc_ptr_->set_geom(geom_);
  default_stc_ptr_ = STCThermalPtr(STCThermal::create());
  default_therm_model_ptr_ = boost::dynamic_pointer_cast<ThermalModel>(default_stc_ptr_);
  default_stc_ptr_->set_mat_table(mat_table_);

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermalTest::TearDown() {
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ThermalModelPtr STCThermalModelConstructor(){
  return boost::dynamic_pointer_cast<ThermalModel>(STCThermal::create());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
STCThermalPtr STCThermalTest::initThermalModel(){
  stringstream ss("");
  ss << "<start>"
     << "  <material>" << mat_ << "</material>"
     << "  <alpha_th>" << alpha_th_ << "</alpha_th>"
     << "  <k_th>" << k_th_ << "</k_th>"
     << "  <spacing>" << spacing_ << "</spacing>"
     << "</start>";

  XMLParser parser(ss);
  XMLQueryEngine* engine = new XMLQueryEngine(parser);
  stc_ptr_ = STCThermalPtr(STCThermal::create());
  stc_ptr_->initModuleMembers(engine);
  delete engine;
  return stc_ptr_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, initial_state){
  EXPECT_EQ(alpha_th_, stc_ptr_->alpha_th());
  EXPECT_EQ(k_th_, stc_ptr_->k_th());
  EXPECT_EQ(spacing_, stc_ptr_->spacing());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, defaultConstructor) {
  ASSERT_EQ("STC_THERMAL", default_therm_model_ptr_->name());
  ASSERT_EQ(STC_THERMAL, default_therm_model_ptr_->type());
  ASSERT_FLOAT_EQ(0, default_stc_ptr_->alpha_th());
  ASSERT_FLOAT_EQ(0, default_stc_ptr_->k_th());
  ASSERT_FLOAT_EQ(0, default_stc_ptr_->spacing());
  ASSERT_FLOAT_EQ(0, default_stc_ptr_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, copy) {
  STCThermalPtr test_copy = STCThermalPtr(STCThermal::create());
  STCThermalPtr stc_shared_ptr = STCThermalPtr(stc_ptr_);
  ThermalModelPtr therm_model_shared_ptr = ThermalModelPtr(therm_model_ptr_);
  EXPECT_NO_THROW(test_copy->copy(*stc_shared_ptr));
  EXPECT_NO_THROW(test_copy->copy(*therm_model_shared_ptr));
  EXPECT_FLOAT_EQ(alpha_th_, stc_ptr_->alpha_th());
  EXPECT_FLOAT_EQ(k_th_, stc_ptr_->k_th());
  EXPECT_FLOAT_EQ(spacing_, stc_ptr_->spacing());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, set_alpha_th){ 
  for( alpha_th_=0; alpha_th_<10; ++alpha_th_){ 
    // it should accept positive floats
    alpha_th_*=0.2;
    ASSERT_NO_THROW(stc_ptr_->set_alpha_th(alpha_th_));
    EXPECT_FLOAT_EQ(stc_ptr_->alpha_th(), alpha_th_);
  }
  // an exception should be thrown if it's set outside the bounds
  alpha_th_= -1;
  EXPECT_THROW(stc_ptr_->set_alpha_th(alpha_th_), CycRangeException);
  EXPECT_NE(stc_ptr_->alpha_th(), alpha_th_);
  alpha_th_= std::numeric_limits<double>::infinity();
  EXPECT_THROW(stc_ptr_->set_alpha_th(alpha_th_), CycRangeException);
  EXPECT_NE(stc_ptr_->alpha_th(), alpha_th_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, set_k_th){ 
  for( k_th_=0; k_th_<10; ++k_th_){ 
    // it should accept positive floats
    k_th_*=0.2;
    ASSERT_NO_THROW(stc_ptr_->set_k_th(k_th_));
    EXPECT_FLOAT_EQ(stc_ptr_->k_th(), k_th_);
  }
  // an exception should be thrown if it's set outside the bounds
  k_th_= -1;
  EXPECT_THROW(stc_ptr_->set_k_th(k_th_), CycRangeException);
  EXPECT_NE(stc_ptr_->k_th(), k_th_);
  k_th_= std::numeric_limits<double>::infinity();
  EXPECT_THROW(stc_ptr_->set_k_th(k_th_), CycRangeException);
  EXPECT_NE(stc_ptr_->k_th(), k_th_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, set_spacing){ 
  for( spacing_=0; spacing_<10; ++spacing_){ 
    // it should accept positive floats
    spacing_*=0.2;
    ASSERT_NO_THROW(stc_ptr_->set_spacing(spacing_));
    EXPECT_FLOAT_EQ(stc_ptr_->spacing(), spacing_);
  }
  // an exception should be thrown if it's set outside the bounds
  spacing_= -1;
  EXPECT_THROW(stc_ptr_->set_spacing(spacing_), CycRangeException);
  EXPECT_NE(stc_ptr_->spacing(), spacing_);
  spacing_= std::numeric_limits<double>::infinity();
  EXPECT_THROW(stc_ptr_->set_spacing(spacing_), CycRangeException);
  EXPECT_NE(stc_ptr_->spacing(), spacing_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, transportHeatClay){ 
  EXPECT_NO_THROW(stc_ptr_->set_geom(geom_));
  /// @TODO set up a clay object
  // transport the heat
  EXPECT_NO_THROW(therm_model_ptr_->transportHeat(time_++));
  /// @TODO add checks and expectiations
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, transportHeatGranite){ 
  EXPECT_NO_THROW(stc_ptr_->set_geom(geom_));
  /// @TODO set up a salt object
  // transport the heat
  EXPECT_NO_THROW(therm_model_ptr_->transportHeat(time_++));
  /// @TODO add checks and expectiations
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, transportHeatSalt){ 
  EXPECT_NO_THROW(stc_ptr_->set_geom(geom_));
  /// @TODO set up a salt object
  // transport the heat
  EXPECT_NO_THROW(therm_model_ptr_->transportHeat(time_++));
  /// @TODO add checks and expectiations
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, setGeometry) {  
  //@TODO tests like this should be interface tests for the ThermalModel class concrete instances.
  EXPECT_NO_THROW(stc_ptr_->set_geom(geom_));
  EXPECT_FLOAT_EQ(len_five_ , therm_model_ptr_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , therm_model_ptr_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , therm_model_ptr_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , therm_model_ptr_->geom()->volume(), 0.1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(STCThermalTest, getVolume) {  
  EXPECT_NO_THROW(stc_ptr_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , therm_model_ptr_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(stc_ptr_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , therm_model_ptr_->geom()->volume());
  EXPECT_THROW(stc_ptr_->geom()->set_radius(OUTER, numeric_limits<double>::infinity()), CycRangeException);
  EXPECT_NO_THROW(therm_model_ptr_->geom()->volume());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
INSTANTIATE_TEST_CASE_P(STCThermalModel, ThermalModelTests, Values(&STCThermalModelConstructor));

