// TwoDimPPMNuclideTests.h
#include <gtest/gtest.h>

#include "TwoDimPPMNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TwoDimPPMNuclideTest : public ::testing::Test {
protected:
  
  TwoDimPPMNuclide* two_dim_ppm_ptr_;
  TwoDimPPMNuclide* default_two_dim_ppm_ptr_;
  NuclideModel* nuc_model_ptr_;
  NuclideModel* default_nuc_model_ptr_;
  double two_dim_ppm_;
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
  TwoDimPPMNuclide* initNuclideModel();
};

