// MaterialDBTests.h
#include <gtest/gtest.h>
#include <MatDataTable.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class MaterialDBTest : public ::testing::Test {
  protected:
    std::vector<std::string> mat_names_;
    std::vector<int> elem_ids_;

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
