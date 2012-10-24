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
TEST_F(MatToolsTest, sum_mats){
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
