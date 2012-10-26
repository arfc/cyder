// NuclideModelTests.cpp 
#include <gtest/gtest.h>

#include "NuclideModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, transportNuclides){
  EXPECT_NO_THROW(nuclide_model_->transportNuclides(0));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, copy){
  //copy one model to another, shouldn't throw 
  NuclideModel* new_nuclide_model = (*GetParam())();
  EXPECT_NO_THROW(new_nuclide_model->copy(nuclide_model_));
  //check that the name matches. 
  EXPECT_EQ(nuclide_model_->name(), new_nuclide_model->name());
  delete new_nuclide_model;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, absorb){
  //nuclide_model_->absorb() doesn't throw
  EXPECT_NO_THROW(nuclide_model_->absorb(test_mat_));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, extract){
  //nuclide_model_->extract() doesn't throw
  EXPECT_NO_THROW(nuclide_model_->absorb(test_mat_));
  EXPECT_NO_THROW(nuclide_model_->extract(test_comp_, test_size_));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, print){
  EXPECT_NO_THROW(nuclide_model_->print());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, name){
  EXPECT_NO_THROW(nuclide_model_->name());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, type){
  EXPECT_NO_THROW(nuclide_model_->type());
  EXPECT_GT(LAST_NUCLIDE, nuclide_model_->type());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, set_geom){
  // set it
  EXPECT_NO_THROW(nuclide_model_->set_geom(geom_));
  // check
  EXPECT_FLOAT_EQ(len_five_ , nuclide_model_->geom()->length());
  EXPECT_FLOAT_EQ(r_four_ , nuclide_model_->geom()->inner_radius());
  EXPECT_FLOAT_EQ(r_five_ , nuclide_model_->geom()->outer_radius());
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuclide_model_->geom()->volume(), 0.1);
  EXPECT_EQ(geom_->x(), nuclide_model_->geom()->x());
  EXPECT_EQ(geom_->y(), nuclide_model_->geom()->y());
  EXPECT_EQ(geom_->z(), nuclide_model_->geom()->z());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_P(NuclideModelTests, getVolume) {  
  EXPECT_NO_THROW(nuclide_model_->set_geom(geom_));
  double vol = len_five_*3.14159*(r_five_*r_five_ - r_four_*r_four_);
  EXPECT_NEAR( vol , nuclide_model_->geom()->volume(), 0.1);
  EXPECT_NO_THROW(nuclide_model_->geom()->set_radius(OUTER, r_four_));
  EXPECT_FLOAT_EQ( 0 , nuclide_model_->geom()->volume());
  EXPECT_NO_THROW(nuclide_model_->geom()->set_radius(OUTER, std::numeric_limits<double>::infinity()));
  EXPECT_FLOAT_EQ( std::numeric_limits<double>::infinity(), nuclide_model_->geom()->volume());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_source_term){
  // check that the source term bc doesn't throw
  // before any contaminants, it had best be 0
  EXPECT_FLOAT_EQ(0, nuclide_model_->source_term_bc(u235_));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_dirichlet){
  // check that the source term bc doesn't throw
  // before any contaminants, it had best be 0
  EXPECT_FLOAT_EQ(0, nuclide_model_->dirichlet_bc(u235_));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_cauchy){
  // check that the source term bc doesn't throw
  // before any contaminants, it had best be 0
  IsoConcMap zeromap;
  zeromap.insert(std::make_pair(92235,0));
  EXPECT_NO_THROW(nuclide_model_->set_geom(geom_));
  EXPECT_FLOAT_EQ(0, nuclide_model_->cauchy_bc(zeromap,r_five_+10,u235_));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_neumann){
  // check that the source term bc doesn't throw
  // before any contaminants, it had best be 0
  IsoConcMap zeromap;
  zeromap.insert(std::make_pair(92235,0));
  EXPECT_NO_THROW(nuclide_model_->set_geom(geom_));
  EXPECT_FLOAT_EQ(0, nuclide_model_->neumann_bc(zeromap,r_five_+10,u235_));
}
