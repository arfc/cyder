// OneDimPPMNuclideTests.h
#include <gtest/gtest.h>

#include "OneDimPPMNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class OneDimPPMNuclideTest : public ::testing::Test {
protected:
  
  OneDimPPMNuclide* one_dim_ppm_ptr_;
  OneDimPPMNuclide* default_one_dim_ppm_ptr_;
  NuclideModel* nuc_model_ptr_;
  NuclideModel* default_nuc_model_ptr_;
  double one_dim_ppm_;
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
  double Ci_,Co_,n_, D_, rho_, Kd_; 
  
  virtual void SetUp();
  virtual void TearDown();
  OneDimPPMNuclide* initNuclideModel();
};

