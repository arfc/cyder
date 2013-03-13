// STCDBTests.h
#include <gtest/gtest.h>
#include "STCDB.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class STCDBTest : public ::testing::Test {
  protected:
    std::vector<std::string> mat_names_;
    std::vector<int> iso_ids_;
    Iso Cs135_, Cs137_, Sr90_, Am241_;
    mat_t salt_struct_;
    double alpha_, k_, spacing_, r_calc_;


    virtual void SetUp(){
      // composition set up
      Cs135_ = 55135;
      Cs137_ = 55137;
      Sr90_ = 38090;
      Am241_ = 95241;
      /// @TODO these values are not valid
      alpha_ = 0;
      /// @TODO these values are not valid
      k_=0;
      /// @TODO these values are not valid
      spacing_=0;
      /// @TODO these values are not valid
      r_calc_=0;
      iso_ids_.push_back(Cs135_);
      iso_ids_.push_back(Cs137_);
      iso_ids_.push_back(Sr90_);
      iso_ids_.push_back(Am241_);
      salt_struct_.a(alpha_).k(k_).s(spacing_).r(r_calc_);
    }

    virtual void TearDown(){
    }
};
