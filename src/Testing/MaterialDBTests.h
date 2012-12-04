// MaterialDBTests.h
#include <gtest/gtest.h>
#include "MaterialDB.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialDBTest : public ::testing::Test {
  protected:
    std::vector<std::string> mat_names_;
    std::vector<int> elem_ids_;
    int u_, am_, th_, pb_, one_g_; 

    virtual void SetUp(){
      // composition set up
      u_ = 92;
      am_ = 95;
      th_ = 90;
      pb_ = 82;
      one_g_ = 1.0;

    }

    virtual void TearDown(){
    }
};
