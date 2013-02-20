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
  double innerradius_, outerradius_, x_,y_,z_,dx_,dy_,dz_,adv_vel_,capacity_,inventory_size_;
  std::string in_commod_, cname_, componenttype_;
  TestMarket* incommod_market;
  
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

