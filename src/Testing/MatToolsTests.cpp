// MatToolsTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "MatTools.h"
#include "NuclideModel.h"
#include "CycException.h"
#include "Material.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MatToolsTest : public ::testing::Test {
  protected:
    CompMapPtr test_comp_;
    mat_rsrc_ptr test_mat_;
    int one_mol_;
    int u235_, am241_;
    double test_size_;
    Radius r_four_, r_five_;
    Length len_five_;
    point_t origin_;
    int time_;

    virtual void SetUp(){
      // composition set up
      u235_=92235;
      am241_=95241;
      one_mol_=1.0;
      test_comp_= CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_] = one_mol_;
      test_size_=10.0;
      // material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);
    }
    virtual void TearDown() {
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, sum_mats_empty){

  deque<mat_rsrc_ptr> mats;
  
  pair<IsoVector, double> the_sum;
  the_sum = MatTools::sum_mats(mats);
  EXPECT_FLOAT_EQ(0, the_sum.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, sum_mats_one){

  mat_rsrc_ptr new_test_mat = mat_rsrc_ptr(new Material(test_comp_));
  new_test_mat->setQuantity(2*test_size_);

  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  mats.push_back(new_test_mat);
  
  pair<IsoVector, double> the_sum;
  the_sum = MatTools::sum_mats(mats);
  EXPECT_FLOAT_EQ(3*test_size_, the_sum.second);
  EXPECT_FLOAT_EQ(1, the_sum.first.comp()->atomFraction(u235_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, extract){
  //@TODO this is just a placeholder, to remind you to write a test.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, convert_comp_to_conc){ 
  IsoConcMap test_conc_map; 

  // composition set up
  CompMapPtr test_comp_map= CompMapPtr(new CompMap(MASS));
  (*test_comp_map)[u235_] = 1.;
  (*test_comp_map)[am241_] = 0.5;
  test_comp_map->normalize();
  double exp_u235_conc, exp_am241_conc;

  for(int v=1; v<10; v++){
    for(int m=1; m<10; m++){
      EXPECT_NO_THROW(MatTools::comp_to_conc_map(test_comp_map, m, v));
      test_conc_map=MatTools::comp_to_conc_map(test_comp_map, m, v);
      exp_u235_conc = (*test_comp_map)[u235_]*m/v;
      EXPECT_FLOAT_EQ(exp_u235_conc, test_conc_map[u235_]);
      exp_am241_conc = (*test_comp_map)[am241_]*m/v;
      EXPECT_FLOAT_EQ(exp_am241_conc, test_conc_map[am241_]);
    }
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_f){
  double V_T = 10;
  double theta;
  for(int i = 1; i < 10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ(theta*V_T, MatTools::V_f(V_T, theta));
  }
  EXPECT_FLOAT_EQ(0, MatTools::V_f(V_T, 0));
  EXPECT_FLOAT_EQ(V_T, MatTools::V_f(V_T, 1));
  EXPECT_THROW(MatTools::V_f(V_T, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_f(V_T, -1),CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_ff){
  double V_T = 10;
  double theta;
  double d;
  for(int i = 1; i<10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ(0, MatTools::V_ff(V_T, theta, 0));
    for(int j = 1; j<10; j++){
      d = j*0.1;
      EXPECT_FLOAT_EQ(d*theta*V_T,MatTools::V_ff(V_T, theta, d));
      EXPECT_FLOAT_EQ(0, MatTools::V_ff(V_T, 0, d));
    }
  }
  EXPECT_FLOAT_EQ(V_T, MatTools::V_ff(V_T, 1, 1));
  EXPECT_FLOAT_EQ(0, MatTools::V_ff(V_T, 1, 0));
  EXPECT_THROW(MatTools::V_ff(V_T, 2, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ff(V_T, -1, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ff(V_T, 1, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_ff(V_T, 1, -1),CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_mf){
  double V_T = 10;
  double theta;
  double d;
  for(int i = 1; i<10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ(theta*V_T, MatTools::V_mf(V_T, theta, 0));
    for(int j = 1; j<10; j++){
      d = j*0.1;
      EXPECT_FLOAT_EQ((1-d)*theta*V_T,MatTools::V_mf(V_T, theta, d));
      EXPECT_FLOAT_EQ(0, MatTools::V_mf(V_T, 0, d));
    }
  }
  EXPECT_FLOAT_EQ(V_T, MatTools::V_mf(V_T, 1, 0));
  EXPECT_FLOAT_EQ(0, MatTools::V_mf(V_T, 1, 1));
  EXPECT_THROW(MatTools::V_mf(V_T, 2, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_mf(V_T, -1, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_mf(V_T, 1, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_mf(V_T, 1, -1),CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_s){
  double V_T = 10;
  double theta;
  for(int i = 1; i < 10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ((1-theta)*V_T, MatTools::V_s(V_T, theta));
  }
  EXPECT_FLOAT_EQ(V_T, MatTools::V_s(V_T, 0));
  EXPECT_FLOAT_EQ(0, MatTools::V_s(V_T, 1));
  EXPECT_THROW(MatTools::V_s(V_T, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_s(V_T, -1),CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_ds){
  double V_T = 10;
  double theta;
  double d;
  for(int i = 1; i<10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ(0, MatTools::V_ds(V_T, theta, 0));
    for(int j = 1; j<10; j++){
      d = j*0.1;
      EXPECT_FLOAT_EQ(d*(1-theta)*V_T,MatTools::V_ds(V_T, theta, d));
      EXPECT_FLOAT_EQ(0, MatTools::V_ds(V_T, 1, d));
    }
  }
  EXPECT_FLOAT_EQ(V_T, MatTools::V_ds(V_T, 0, 1));
  EXPECT_FLOAT_EQ(0, MatTools::V_ds(V_T, 1, 0));
  EXPECT_THROW(MatTools::V_ds(V_T, 2, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ds(V_T, -1, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ds(V_T, 1, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_ds(V_T, 1, -1),CycRangeException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, V_ms){
  double V_T = 10;
  double theta;
  double d;
  for(int i = 1; i<10; i++){
    theta = i*0.1;
    EXPECT_FLOAT_EQ(0, MatTools::V_ms(V_T, theta, 1));
    for(int j = 1; j<10; j++){
      d = j*0.1;
      EXPECT_FLOAT_EQ((1-d)*(1-theta)*V_T,MatTools::V_ms(V_T, theta, d));
      EXPECT_FLOAT_EQ(0, MatTools::V_ms(V_T, 1, d));
    }
  }
  EXPECT_FLOAT_EQ(V_T, MatTools::V_ms(V_T, 0, 0));
  EXPECT_FLOAT_EQ(0, MatTools::V_ms(V_T, 1, 1));
  EXPECT_THROW(MatTools::V_ms(V_T, 2, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ms(V_T, -1, 1),CycRangeException);
  EXPECT_THROW(MatTools::V_ms(V_T, 1, 2),CycRangeException);
  EXPECT_THROW(MatTools::V_ms(V_T, 1, -1),CycRangeException);
}


