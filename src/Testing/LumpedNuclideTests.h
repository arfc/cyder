// LumpedNuclideTests.h
#include <gtest/gtest.h>

#include "LumpedNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class LumpedNuclideTest : public ::testing::Test {
protected:
  
  LumpedNuclidePtr lumped_ptr_;
  LumpedNuclidePtr default_lumped_ptr_;
  NuclideModelPtr nuc_model_ptr_;
  NuclideModelPtr default_nuc_model_ptr_;
  MatDataTablePtr mat_table_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_mol_;
  double t_t_;
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
  LumpedNuclidePtr initNuclideModel();
};

