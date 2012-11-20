// MixedCellNuclideTests.h
#include <gtest/gtest.h>

#include "MixedCellNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MixedCellNuclideTest : public ::testing::Test {
protected:
  
  MixedCellNuclide* mixed_cell_ptr_;
  MixedCellNuclide* default_mixed_cell_ptr_;
  NuclideModel* nuc_model_ptr_;
  NuclideModel* default_nuc_model_ptr_;
  double mixed_cell_;
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
  double porosity_, diffusion_coeff_;
  double deg_rate_;
  
  virtual void SetUp();
  virtual void TearDown();
  MixedCellNuclide* initNuclideModel();
};

