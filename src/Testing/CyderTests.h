// CyderTests.h
#include <gtest/gtest.h>

#include "Cyder.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CyderTest : public ::testing::Test {
protected:
  
  Cyder* src_facility_;
  Cyder* clone_facility_;
  int lifetime_,start_op_yr_,start_op_mo_, time_;
  double wfinnerradius_, wfouterradius_;
  double wpinnerradius_, wpouterradius_; 
  double binnerradius_, bouterradius_;
  double ffinnerradius_, ffouterradius_; 
  double x_,y_,z_,dx_,dy_,dz_,adv_vel_,capacity_,t_lim_,inventory_size_;
  std::string in_commod_, cname_, componenttype_;
  std::string wfname_, wftype_;
  std::string wpname_, wptype_;
  std::string bname_, btype_;
  std::string ffname_, fftype_;
  TestMarket* incommod_market;

  Temp high_t_lim_, low_t_lim_;
  Radius far_r_lim_, near_r_lim_;
  int Cs135_, Cs137_;

  CompMapPtr hot_comp_;
  CompMapPtr cold_comp_;

  mat_rsrc_ptr hot_mat_;
  mat_rsrc_ptr cold_mat_;
  
  virtual void SetUp();
  virtual void TearDown();
  Cyder* initSrcFacility();
  void initWorld();

public:
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* CyderModelConstructor(){
  return dynamic_cast<Model*>(new Cyder());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* CyderFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new Cyder());
}

