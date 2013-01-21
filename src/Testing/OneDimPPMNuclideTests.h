// OneDimPPMNuclideTests.h
#include <gtest/gtest.h>

#include "OneDimPPMNuclide.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class OneDimPPMNuclideTest : public ::testing::Test {
protected:
  
  OneDimPPMNuclidePtr one_dim_ppm_ptr_;
  OneDimPPMNuclidePtr default_one_dim_ppm_ptr_;
  NuclideModelPtr nuc_model_ptr_;
  NuclideModelPtr default_nuc_model_ptr_;
  MatDataTablePtr mat_table_;
  double one_dim_ppm_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_kg_;
  int u235_, am241_;
  double test_size_;
  double theta_;
  double v_;
  GeometryPtr geom_;
  Radius r_four_, r_five_;
  Length len_five_;
  point_t origin_;
  int time_;
  double Ci_,Co_,porosity_, D_, rho_, Kd_; 
  
  virtual void SetUp();
  virtual void TearDown();
  OneDimPPMNuclidePtr initNuclideModel();
  IsoConcMap calculate_conc(IsoConcMap C_0, Radius r, int the_time);
  double calculate_conc(IsoConcMap C_0, Radius r, Iso iso, int the_time); 
};

