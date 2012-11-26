// StubNuclideTests.h
#include <gtest/gtest.h>

#include "StubNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class StubNuclideTest : public ::testing::Test {
protected:
  
  StubNuclidePtr stub_ptr_;
  StubNuclidePtr default_stub_ptr_;
  NuclideModelPtr nuc_model_ptr_;
  NuclideModelPtr default_nuc_model_ptr_;
  double stub_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_mol_;
  int u235_, am241_;
  double test_size_;
  double theta_;
  double adv_vel_;
  GeometryPtr geom_;
  Radius r_four_, r_five_;
  Length len_five_;
  point_t origin_;
  int time_;
  int some_param_;
  
  virtual void SetUp();
  virtual void TearDown();
  StubNuclidePtr initNuclideModel();
};

