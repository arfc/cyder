// MatToolsTests.cpp
#include <deque>
#include <map>
#include <gtest/gtest.h>

#include "CycLimits.h"
#include "CycException.h"
#include "Material.h"
#include "MatTools.h"
#include "NuclideModel.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MatToolsTest : public ::testing::Test {
  protected:
    IsoConcMap test_conc_map_;
    CompMapPtr test_comp_;
    CompMapPtr second_comp_map_;
    CompMapPtr third_comp_map_;
    mat_rsrc_ptr test_mat_;
    mat_rsrc_ptr second_mat_;
    mat_rsrc_ptr third_mat_;
    double one_;
    double half_;
    int u235_, am241_, pu239_;
    double test_size_;
    Radius r_four_, r_five_;
    Length len_five_;
    point_t origin_;
    int time_;

    virtual void SetUp(){
      // composition set up
      u235_=92235;
      pu239_=92239;
      am241_=95241;
      one_=1.0;
      half_=0.5;
      test_comp_= CompMapPtr(new CompMap(MASS));
      (*test_comp_)[u235_] = one_;
      test_size_=10.0;
      // conc setup
      test_conc_map_[u235_] = test_size_; 
      // test material creation
      test_mat_ = mat_rsrc_ptr(new Material(test_comp_));
      test_mat_->setQuantity(test_size_);

      // second composition set up
      second_comp_map_= CompMapPtr(new CompMap(MASS));
      (*second_comp_map_)[u235_] = one_;
      (*second_comp_map_)[am241_] = half_;
      second_mat_ = mat_rsrc_ptr(new Material(second_comp_map_));
      second_mat_->setQuantity(test_size_);

      // third composition set up
      third_comp_map_= CompMapPtr(new CompMap(MASS));
      (*third_comp_map_)[pu239_] = one_;
      third_mat_ = mat_rsrc_ptr(new Material(third_comp_map_));
      third_mat_->setQuantity(test_size_);
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
  EXPECT_TRUE(the_sum.first.comp()->map().empty());
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
TEST_F(MatToolsTest, addConcMaps){
  IsoConcMap orig;
  IsoConcMap to_add;
  orig[u235_] = 1.0;
  to_add[u235_] = 2.0;
  IsoConcMap sum = MatTools::addConcMaps(orig, to_add);
  EXPECT_FLOAT_EQ(3, sum[u235_]); 
  sum = MatTools::addConcMaps(orig, orig);
  EXPECT_FLOAT_EQ(2, sum[u235_]); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, sum_mats_small_entry){

  CompMapPtr comp_to_add;
  comp_to_add = CompMapPtr(new CompMap(MASS));
  (*comp_to_add)[u235_] = 1;
  (*comp_to_add)[am241_] = 1;
  mat_rsrc_ptr mat_to_add = mat_rsrc_ptr(new Material(comp_to_add));
  mat_to_add->setQuantity(2*test_size_);

  deque<mat_rsrc_ptr> mats;
  mats.push_back(test_mat_);
  mats.push_back(mat_to_add);

  double exp_u235 = test_mat_->mass(u235_) + mat_to_add->mass(u235_);
  double exp_am241 = mat_to_add->mass(am241_);
  double exp_tot = test_mat_->mass(MassUnit(KG)) + mat_to_add->mass(MassUnit(KG));
  ASSERT_FLOAT_EQ(3*test_size_, exp_tot);

  pair<IsoVector, double> the_sum;
  the_sum = MatTools::sum_mats(mats);
  EXPECT_FLOAT_EQ(exp_tot, the_sum.second);
  EXPECT_FLOAT_EQ(exp_u235/exp_tot, the_sum.first.comp()->massFraction(u235_));
  EXPECT_FLOAT_EQ(exp_am241/exp_tot, the_sum.first.comp()->massFraction(am241_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, KahanSumZero){
  vector<double> to_sum;
  for(int i=0; i<10; ++i){
    to_sum.push_back(0);
  }
  EXPECT_FLOAT_EQ(0, MatTools::KahanSum(to_sum));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, KahanSumOrdinary){
  vector<double> ten_vec;
  vector<double> factorial_vec;
  double factorial_sum = 0;
  for(int i=0; i<10; ++i){
    ten_vec.push_back(1);
    factorial_vec.push_back(i);
    factorial_sum +=i;
  }
  EXPECT_FLOAT_EQ(10, MatTools::KahanSum(ten_vec));
  EXPECT_FLOAT_EQ(factorial_sum, MatTools::KahanSum(factorial_vec));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, KahanSumExtremeVals){
  vector<double> extreme_vec;
  double expected = 0;
  extreme_vec.push_back(1e16);
  extreme_vec.push_back(1e-16);
  extreme_vec.push_back(1);
  EXPECT_FLOAT_EQ(1e16+1e-16+1, MatTools::KahanSum(extreme_vec));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, combine_mats_zero) { 
  deque<mat_rsrc_ptr> mat_list;
  mat_rsrc_ptr result = mat_rsrc_ptr(MatTools::combine_mats(mat_list));
  EXPECT_EQ(0, mat_list.size());
  EXPECT_EQ(0, result->mass(KG));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, combine_mats_one) { 
  deque<mat_rsrc_ptr> mat_list;
  mat_list.push_back(test_mat_);
  mat_rsrc_ptr result = MatTools::combine_mats(mat_list);
  EXPECT_FLOAT_EQ(test_size_, result->mass(KG));
  EXPECT_EQ(0, mat_list.size());
  mat_list.push_back(result);
  EXPECT_EQ(1, mat_list.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, combine_mats_complex) { 
  mat_rsrc_ptr test_copy = mat_rsrc_ptr(new Material(test_comp_));
  test_copy->setQuantity(test_mat_->mass(KG));
  mat_rsrc_ptr second_copy = mat_rsrc_ptr(new Material(second_comp_map_));
  second_copy->setQuantity(second_mat_->mass(KG));
  mat_rsrc_ptr third_copy = mat_rsrc_ptr(new Material(third_comp_map_));
  third_copy->setQuantity(third_mat_->mass(KG));

  deque<mat_rsrc_ptr> mat_list;
  mat_list.push_back(test_mat_);
  mat_list.push_back(second_mat_);
  mat_list.push_back(third_mat_);

  mat_rsrc_ptr result = MatTools::combine_mats(mat_list);
  EXPECT_FLOAT_EQ(3*test_size_, result->mass(KG));

  CompMapPtr result_comp = result->isoVector().comp();
  CompMap::const_iterator it;
  for( it=(*result_comp).begin(); 
      it!=(*result_comp).end(); 
      ++it) {
    int iso = (*it).first;
    double term1 = test_copy->mass(iso);
    double term2 = second_copy->mass(iso);
    double term3 = third_copy->mass(iso);
    EXPECT_FLOAT_EQ(term1+term2+term3, result->mass(iso));
  }

  EXPECT_EQ(0, mat_list.size());
  mat_list.push_back(result);
  EXPECT_EQ(1, mat_list.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, absorbtest){
  // This should be covered by the material class, but i'm just checking... 
  mat_rsrc_ptr glob = mat_rsrc_ptr(new Material());
  glob->absorb(test_mat_);
  EXPECT_FLOAT_EQ(test_size_, glob->mass(KG));
  EXPECT_FLOAT_EQ(test_size_, glob->mass(u235_));
  glob->absorb(second_mat_);
  EXPECT_FLOAT_EQ(2*test_size_, glob->mass(KG));
  EXPECT_FLOAT_EQ(test_size_*(one_+ 2./3.), glob->mass(u235_));
  EXPECT_FLOAT_EQ(test_size_*(1./3.), glob->mass(am241_));
  glob->absorb(third_mat_);
  EXPECT_FLOAT_EQ(3*test_size_, glob->mass(KG));
  EXPECT_FLOAT_EQ(test_size_, glob->mass(pu239_));
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
      EXPECT_NO_THROW(MatTools::comp_to_conc_map(test_comp_map, m, 0));
    }
  }

  // it should result in a real object when scaled by zero volume
  test_conc_map = MatTools::comp_to_conc_map(test_comp_map, 1, 0);
  EXPECT_FLOAT_EQ(0, test_conc_map[u235_]);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, scale_conc_map){
  double scale;
  IsoConcMap test_conc_map;
  IsoConcMap scaled_map;
  test_conc_map[u235_]=test_size_;
  test_conc_map[am241_]=2*test_size_;
  for(int i = 1; i<10; i++){
    scale = i*0.5;
    EXPECT_NO_THROW(MatTools::scaleConcMap(test_conc_map,scale));
    scaled_map = MatTools::scaleConcMap(test_conc_map,scale);
    EXPECT_FLOAT_EQ(scale*test_conc_map[u235_], scaled_map[u235_]);
    EXPECT_FLOAT_EQ(scale*test_conc_map[am241_], scaled_map[am241_]);
  }

  EXPECT_THROW(MatTools::scaleConcMap(test_conc_map, -1), CycRangeException);
  EXPECT_THROW(MatTools::scaleConcMap(test_conc_map, numeric_limits<double>::infinity()), CycRangeException);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, scale_zero_conc_map){
  double scale;
  IsoConcMap scaled_map;
  IsoConcMap test_zero_map;
  test_zero_map[u235_]=0;
  test_zero_map[am241_]=0;
  for(int i = 1; i<10; i++){
    scale = i*0.5;
    EXPECT_NO_THROW(MatTools::scaleConcMap(test_zero_map,scale));
    scaled_map = MatTools::scaleConcMap(test_zero_map,scale);
    EXPECT_FLOAT_EQ(0, scale*test_zero_map[u235_]);
    EXPECT_FLOAT_EQ(scale*test_zero_map[u235_], scaled_map[u235_]);
    EXPECT_FLOAT_EQ(scale*test_zero_map[am241_], scaled_map[am241_]);
  }

  EXPECT_THROW(MatTools::scaleConcMap(test_zero_map, -1), CycRangeException);
  EXPECT_THROW(MatTools::scaleConcMap(test_zero_map, numeric_limits<double>::infinity()), CycRangeException);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, validate_finite_pos){
  for(int i; i<10; i++){
    EXPECT_NO_THROW(MatTools::validate_finite_pos(i));
  }
  EXPECT_THROW(MatTools::validate_finite_pos(-1), CycRangeException);
  EXPECT_THROW(MatTools::validate_finite_pos(numeric_limits<double>::infinity()), CycRangeException);
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


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, conc_to_conc_map) {
  pair <CompMapPtr, double> test_comp_pair = MatTools::conc_to_comp_map(test_conc_map_ , 0);
  EXPECT_FLOAT_EQ(0, test_comp_pair.second );
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, isoToElem){
  int u235 = 92235;
  int u = 92;
  int pu600 = 94600; // yes, I get that this isn't real, it's a TEST.
  int pu = 94;
  int o16 = 8016;
  int o = 8;
  EXPECT_EQ(u, MatTools::isoToElem(u235));
  EXPECT_EQ(pu, MatTools::isoToElem(pu600));
  EXPECT_EQ(o, MatTools::isoToElem(o16));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatToolsTest, linspace){
  int n = 11;
  double a = 0;
  double b = 10;
  vector<double> points = MatTools::linspace(a, b, n);
  EXPECT_EQ(points.size(), n);
  int i = 0;
  vector<double>::const_iterator pt;
  for(pt=points.begin(); pt!=points.end(); ++pt){
    EXPECT_EQ(i, (*pt));
    EXPECT_FLOAT_EQ(i, (*pt));
    i+=1;
  }

  a = 10;
  b = 20;
  points = MatTools::linspace(a,b,n);
  EXPECT_EQ(points.size(), n);
  i=10;
  for(pt=points.begin(); pt!=points.end(); ++pt){
    EXPECT_FLOAT_EQ(i, (*pt));
    i+=1;
  }

  n=21;
  points = MatTools::linspace(a, b, n);
  EXPECT_EQ(points.size(), n);
  double d=10;
  for(pt=points.begin(); pt!=points.end(); ++pt){
    EXPECT_FLOAT_EQ(d, (*pt));
    d+=0.5;
  }
}

