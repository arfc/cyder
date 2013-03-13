// STCDBTests.h
#include <gtest/gtest.h>
#include "STCDB.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class STCDBTest : public ::testing::Test {
  protected:
    std::vector<std::string> mat_names_;
    std::vector<int> iso_ids_;
    Iso Cs135_, Cs137_, Sr90_, Am241_;

    virtual void SetUp(){
      // composition set up
      Cs135_ = 55135;
      Cs137_ = 55137;
      Sr90_ = 38090;
      Am241_ = 95241;
      iso_ids_.push_back(Cs135_);
      iso_ids_.push_back(Cs137_);
      iso_ids_.push_back(Sr90_);
      iso_ids_.push_back(Am241_);
    }

    virtual void TearDown(){
    }
};
