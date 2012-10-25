// DegRateNuclideTests.h
#include <gtest/gtest.h>

#include "DegRateNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class DegRateNuclideTest : public ::testing::Test {
protected:
  
  DegRateNuclide* deg_rate_ptr_;
  DegRateNuclide* default_deg_rate_ptr_;
  NuclideModel* nuc_model_ptr_;
  NuclideModel* default_nuc_model_ptr_;
  double deg_rate_;
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
  
  virtual void SetUp();
  virtual void TearDown();
  DegRateNuclide* initNuclideModel();
};

