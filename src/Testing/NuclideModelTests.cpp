// NuclideModelTests.cpp 
#include <gtest/gtest.h>

#include "NuclideModelTests.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, transportNuclides){
  EXPECT_NO_THROW(nuclide_model_->transportNuclides(0));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, copy){
  NuclideModel* new_nuclide_model = (*GetParam())();
  EXPECT_NO_THROW(new_nuclide_model->copy(nuclide_model_));
  EXPECT_EQ(nuclide_model_->name(), new_nuclide_model->name());
  //copy one model to another, shouldn't throw 
  //check that the name matches. 
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, absorb){
  //check contained mat 
  //create material
  //nuclide_model_->absorb() doesn't throw
  //check contained mat, it should have increased by amount absorbed 
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, extract){
  //check contained mat 
  //create material
  //nuclide_model_->extract() doesn't throw
  //check contained mat, it should have decreased by amount extracted 
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
  // create geometry
  // set it
  // check
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_source_term){
  // check that the source term bc doesn't throw
  // and is >=0
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_dirichlet){
  // check that the source term bc doesn't throw
  // and is >=0
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_neumann){
  // check that the source term bc doesn't throw
  // and is >=0
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_P(NuclideModelTests, crude_cauchy){
  // check that the source term bc doesn't throw
  // and is >= 0
}
