// ThermalModelTests.cpp 
#include <gtest/gtest.h>

#include "ThermalModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, transportHeat){
  EXPECT_NO_THROW(thermal_model_->transportHeat(0));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, copy){
  //copy one model to another, shouldn't throw 
  ThermalModelPtr new_thermal_model = ThermalModelPtr((*GetParam())());
  ThermalModelPtr thermal_model_shared = ThermalModelPtr(thermal_model_);
  EXPECT_NO_THROW(new_thermal_model->copy(*thermal_model_shared));
  //check that the name matches. 
  EXPECT_EQ(thermal_model_->name(), new_thermal_model->name());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, print){
  EXPECT_NO_THROW(thermal_model_->print());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, name){
  EXPECT_NO_THROW(thermal_model_->name());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, type){
  EXPECT_NO_THROW(thermal_model_->type());
  EXPECT_GT(LAST_THERMAL, thermal_model_->type());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(ThermalModelTests, set_geom){
  GeometryPtr geom_copy = GeometryPtr(geom_);
  // set it
  EXPECT_NO_THROW(thermal_model_->set_geom(geom_copy));
  // check
  EXPECT_FLOAT_EQ(len_five_ , thermal_model_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , thermal_model_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , thermal_model_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , thermal_model_->geom()->volume(), 0.1);
  EXPECT_EQ(geom_->x(), thermal_model_->geom()->x());
  EXPECT_EQ(geom_->y(), thermal_model_->geom()->y());
  EXPECT_EQ(geom_->z(), thermal_model_->geom()->z());

  // change it from another pointer
  EXPECT_NO_THROW(geom_->set_length(2000));
  EXPECT_FLOAT_EQ(2000, thermal_model_->geom()->length());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_P(ThermalModelTests, getVolume) {  
  EXPECT_NO_THROW(thermal_model_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , thermal_model_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(thermal_model_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , thermal_model_->geom()->volume());
  EXPECT_THROW(thermal_model_->geom()->set_radius(OUTER, std::numeric_limits<double>::infinity()), CycRangeException);
  EXPECT_NO_THROW(thermal_model_->geom()->volume());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/// @TODO add more generic thermal model tests.
