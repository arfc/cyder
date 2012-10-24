// GenericRepositoryTests.h
#include <gtest/gtest.h>

#include "GenericRepository.h"
#include "FacilityModelTests.h"
#include "ModelTests.h"
#include <string>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class GenericRepositoryTest : public ::testing::Test {
protected:
  
  GenericRepository* src_facility;
  int lifetime_,start_op_yr_,start_op_mo_;
  double innerradius_, outerradius_, x_,y_,z_,dx_,dy_,dz_, capacity_, inventory_size_;
  std::string in_commod_, cname_, componenttype_;
  TestMarket* incommod_market;
  
  virtual void SetUp();
  virtual void TearDown();
  GenericRepository* initSrcFacility();
  void initWorld();

public:
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* GenericRepositoryModelConstructor(){
  return dynamic_cast<Model*>(new GenericRepository());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel* GenericRepositoryFacilityConstructor(){
  return dynamic_cast<FacilityModel*>(new GenericRepository());
}

