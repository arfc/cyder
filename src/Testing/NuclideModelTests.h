// NuclideModelTests.h
#include <gtest/gtest.h>

#include "NuclideModel.h"
#include "suffix.h"
#include "TestInst.h"
#include "TestMarket.h"

#if GTEST_HAS_PARAM_TEST

using ::testing::TestWithParam;
using ::testing::Values;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Inside the test body, fixture constructor, SetUp(), and TearDown() we
// can refer to the test parameter by GetParam().  In this case, the test
// parameter is a pointer to a concrete NuclideModel instance 
typedef NuclideModel* NuclideModelConstructor();

class NuclideModelTests : public TestWithParam<NuclideModelConstructor*> {
 public:
  virtual void SetUp() { 
    nuclide_model_ = (*GetParam())();
      // set up geometry. this usually happens in the component init
      r_four_ = 4;
      r_five_ = 5;
      point_t origin_ = {0,0,0}; 
      len_five_ = 5;
      geom_ = GeometryPtr(new Geometry(r_four_, r_five_, origin_, len_five_));

      // other vars
      time_ = 0;

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
  virtual void TearDown(){ 
    delete nuclide_model_;
  }
    
 protected:
  NuclideModel* nuclide_model_;
  CompMapPtr test_comp_;
  mat_rsrc_ptr test_mat_;
  int one_mol_;
  int u235_, am241_;
  double test_size_;
  GeometryPtr geom_;
  Radius r_four_, r_five_;
  Length len_five_;
  point_t origin_;
  int time_;
};

#else

// Google Test may not support value-parameterized tests with some
// compilers. If we use conditional compilation to compile out all
// code referring to the gtest_main library, MSVC linker will not link
// that library at all and consequently complain about missing entry
// point defined in that library (fatal error LNK1561: entry point
// must be defined). This dummy test keeps gtest_main linked in.
TEST(DummyTest, ValueParameterizedTestsAreNotSupportedOnThisPlatform) {}

#endif  // GTEST_HAS_PARAM_TEST


