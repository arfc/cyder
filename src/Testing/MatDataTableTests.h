// MatDataTableTests.h
#include <gtest/gtest.h>
#include <MatDataTable.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MatDataTableTest : public ::testing::Test {
  protected:
    vector mat_names_;
    vector<int> iso_ids_;
    vector<> iso_ids_;

    virtual void SetUp(){
      // composition set up
      u235_ = 92235;
      am241_ = 95241;
      th228_ = 90228;
      pb208_ = 82208;
      one_g_ = 1.0;

    }

    virtual void TearDown(){
    }
};
