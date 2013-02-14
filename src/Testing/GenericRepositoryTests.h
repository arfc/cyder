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
  mat_rsrc_ptr hot_mat;
  mat_rsrc_ptr cold_mat;
  
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

