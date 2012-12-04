// ComponentTests.cpp
#include <gtest/gtest.h>

#include "Component.h"
#include "CycException.h"
#include "StubThermal.h"
#include "StubNuclide.h"
#include "DegRateNuclide.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class ComponentTest : public ::testing::Test {
  protected:
    ComponentPtr test_component_;
    Temp OneHundredCinK;
    string name_;
    string mat_;
    double infty_;
    ComponentType type_;
    Radius inner_radius_, outer_radius_;
    ThermalModelPtr thermal_model_;
    NuclideModelPtr nuclide_model_;

    virtual void SetUp(){
      test_component_ = ComponentPtr(new Component());
      OneHundredCinK=373;
      name_ = "Test";
      infty_ = numeric_limits<double>::infinity();
      type_ = BUFFER;
      mat_ = "clay";
      inner_radius_ = 2;
      outer_radius_ = 10;
      thermal_model_ = StubThermal::create();
      nuclide_model_ = DegRateNuclide::create();
    }
    virtual void TearDown() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, defaultConstructor) {
  ASSERT_EQ("", test_component_->name());
  ASSERT_EQ(LAST_EBS, test_component_->type());
  ASSERT_FLOAT_EQ(0,test_component_->inner_radius());
  ASSERT_EQ(infty_, test_component_->outer_radius());

  ASSERT_FLOAT_EQ(0, test_component_->centroid().x_);
  ASSERT_FLOAT_EQ(0, test_component_->centroid().y_);
  ASSERT_FLOAT_EQ(0, test_component_->centroid().z_);

  ASSERT_FLOAT_EQ(0, test_component_->x());
  ASSERT_FLOAT_EQ(0, test_component_->y());
  ASSERT_FLOAT_EQ(0, test_component_->z());

  ASSERT_FLOAT_EQ(0, test_component_->temp());
  ASSERT_FLOAT_EQ(OneHundredCinK, test_component_->temp_lim());
  ASSERT_EQ(false, !test_component_->thermal_model());
  ASSERT_EQ(false, !test_component_->nuclide_model());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, initFunctionNoXML) { 
  test_component_->init(name_, type_, mat_, inner_radius_, outer_radius_, thermal_model_, nuclide_model_);
  EXPECT_NO_THROW(test_component_->init(name_, type_, mat_, inner_radius_, outer_radius_, 
        thermal_model_, nuclide_model_));
  ASSERT_EQ(name_, test_component_->name());
  ASSERT_EQ(type_, test_component_->type());
  ASSERT_EQ(inner_radius_, test_component_->inner_radius());
  ASSERT_EQ(outer_radius_, test_component_->outer_radius());
  ASSERT_EQ(thermal_model_, test_component_->thermal_model());
  ASSERT_EQ(nuclide_model_, test_component_->nuclide_model());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, copy) {
  ComponentPtr test_copy = ComponentPtr(new Component());
  ASSERT_NO_THROW(test_copy->copy(test_copy));
  EXPECT_EQ("STUB_THERMAL", test_copy->thermal_model()->name());
  EXPECT_EQ("STUB_NUCLIDE", test_copy->nuclide_model()->name());

  EXPECT_NO_THROW(test_component_->init(name_, type_, mat_, inner_radius_, outer_radius_, 
        thermal_model_, nuclide_model_));
  EXPECT_NO_THROW(test_copy->copy(test_component_));

  EXPECT_EQ(inner_radius_, test_copy->inner_radius());
  EXPECT_EQ(outer_radius_, test_copy->outer_radius());
  EXPECT_EQ("STUB_THERMAL", test_copy->thermal_model()->name());
  EXPECT_EQ("DEGRATE_NUCLIDE", test_copy->nuclide_model()->name());
}
