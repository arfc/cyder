// MixedCellNuclideTests.h
#include <gtest/gtest.h>

#include "MixedCellNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MixedCellNuclideTest : public ::testing::Test {
protected:
  
  MixedCellNuclidePtr mixed_cell_ptr_;
  MixedCellNuclidePtr default_mixed_cell_ptr_;
  MatDataTablePtr mat_table_;
  NuclideModelPtr nuc_model_ptr_;
  NuclideModelPtr default_nuc_model_ptr_;
  double mixed_cell_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_mol_;
  int u235_, am241_;
  Elem u_;
  double test_size_;
  double theta_;
  double adv_vel_;
  GeometryPtr geom_;
  Radius r_four_, r_five_;
  Length len_five_;
  point_t origin_;
  int time_;
  double porosity_, D_;
  double deg_rate_;
  
  virtual void SetUp();
  virtual void TearDown();
  MixedCellNuclidePtr initNuclideModel();
};

