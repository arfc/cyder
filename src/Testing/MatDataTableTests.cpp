// MatDataTableTests.cpp
#include <gtest/gtest.h>
#include "MatDataTableTests.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, default_constructor){
  EXPECT_FLOAT_EQ(NULL, default_table_->ref_disp());
  EXPECT_FLOAT_EQ(NULL, default_table_->ref_kd());
  EXPECT_FLOAT_EQ(NULL, default_table_->ref_sol());
  EXPECT_EQ("", default_table_->mat());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, constructor){
  EXPECT_FLOAT_EQ(ref_disp_, mat_table_->ref_disp());
  EXPECT_FLOAT_EQ(ref_kd_, mat_table_->ref_kd());
  EXPECT_FLOAT_EQ(ref_sol_, mat_table_->ref_sol());
  EXPECT_EQ(mat_, mat_table_->mat());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, ref){
  EXPECT_FLOAT_EQ(ref_disp_, mat_table_->ref(DISP));
  EXPECT_FLOAT_EQ(ref_kd_, mat_table_->ref(KD));
  EXPECT_FLOAT_EQ(ref_sol_, mat_table_->ref(SOL));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, rel){
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it!=elem_ids_.end(); ++it){
    EXPECT_FLOAT_EQ(d_[(*it)]/d_[h_], mat_table_->rel((*it),DISP));
    EXPECT_FLOAT_EQ(kd_[(*it)]/kd_[h_], mat_table_->rel((*it),KD));
    EXPECT_FLOAT_EQ(sol_[(*it)]/sol_[h_], mat_table_->rel((*it),SOL));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, S){
  // the DB should give appropriate solubility limits for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it!=elem_ids_.end(); ++it){
    EXPECT_NO_THROW(mat_table_->S((*it)));
    EXPECT_GT(mat_table_->S((*it)),0);
    double exp_sol = sol_[(*it)]*ref_sol_/sol_[h_];
    EXPECT_FLOAT_EQ(exp_sol, mat_table_->S((*it)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, K_d){
  // the DB should give appropriate K_d values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(mat_table_->K_d(*it));
    EXPECT_GT(mat_table_->K_d(*it), 0);
    double exp_kd = kd_[(*it)]*ref_kd_/kd_[h_];
    EXPECT_FLOAT_EQ(exp_kd, mat_table_->K_d((*it)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(MatDataTableTest, D){
  // the DB should give appropriate D values for canonical mat/elems, within a range.
  std::vector<Elem>::iterator it;
  for(it=elem_ids_.begin(); it<elem_ids_.end(); it++){
    EXPECT_NO_THROW(mat_table_->D(*it));
    EXPECT_GT(mat_table_->D(*it),0);
    double exp_disp = d_[(*it)]*ref_disp_/d_[h_];
    EXPECT_FLOAT_EQ(exp_disp, mat_table_->D((*it)));
  }
}

