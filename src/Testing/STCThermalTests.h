// STCThermalTests.h
#include <gtest/gtest.h>

#include "STCThermal.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include "MatDataTable.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class STCThermalTest : public ::testing::Test {
protected:
  
  STCThermalPtr stc_ptr_;
  STCThermalPtr default_stc_ptr_;
  ThermalModelPtr therm_model_ptr_;
  ThermalModelPtr default_therm_model_ptr_;
  double alpha_th_;
  double k_th_;
  double spacing_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_mol_;
  int u235_, am241_;
  Elem u_;
  double test_size_;
  std::string mat_;
  MatDataTablePtr mat_table_;
  GeometryPtr geom_;
  Radius r_four_, r_five_;
  Length len_five_;
  point_t origin_;
  int time_;
  
  virtual void SetUp();
  virtual void TearDown();
  STCThermalPtr initThermalModel();
};

