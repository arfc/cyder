// CyderTests.h
#include <gtest/gtest.h>

#include "Cyder.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CyderTest : public ::testing::Test {
protected:
  
  Cyder* src_facility;
  int lifetime_,start_op_yr_,start_op_mo_;
  double wfinnerradius_, wfouterradius_;
  double wpinnerradius_, wpouterradius_; 
  double binnerradius_, bouterradius_;
  double ffinnerradius_, ffouterradius_; 
  double x_,y_,z_,dx_,dy_,dz_,adv_vel_,capacity_,inventory_size_;
  std::string in_commod_, cname_, componenttype_;
  std::string wfname_, wftype_;
  std::string wpname_, wptype_;
  std::string bname_, btype_;
  std::string ffname_, fftype_;
  TestMarket* incommod_market;
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

