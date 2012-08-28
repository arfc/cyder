// ComponentTests.cpp
#include <gtest/gtest.h>

#include "Component.h"
#include "CycException.h"
#include "StubThermal.h"
#include "StubNuclide.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class ComponentTest : public ::testing::Test {
  protected:
    Component* test_component_;
    Temp OneHundredCinK;
    string name_;
    ComponentType type_;
    Radius inner_radius_, outer_radius_;
    ThermalModel* thermal_model_;
    NuclideModel* nuclide_model_;

    virtual void SetUp(){
      test_component_ = new Component();
      OneHundredCinK=373;
      name_ = "Test";
      type_ = BUFFER;
      inner_radius_ = 2;
      outer_radius_ = 10;
      thermal_model_ = new StubThermal();
      nuclide_model_ = new StubNuclide();
    }
    virtual void TearDown() {
      delete test_component_;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, defaultConstructor) {
  ASSERT_EQ("", test_component_->name());
  ASSERT_EQ(LAST_EBS, test_component_->type());
  ASSERT_FLOAT_EQ(0,test_component_->inner_radius());
  ASSERT_EQ(NULL, test_component_->outer_radius());

  ASSERT_FLOAT_EQ(0, test_component_->centroid().x_);
  ASSERT_FLOAT_EQ(0, test_component_->centroid().y_);
  ASSERT_FLOAT_EQ(0, test_component_->centroid().z_);

  ASSERT_FLOAT_EQ(0, test_component_->x());
  ASSERT_FLOAT_EQ(0, test_component_->y());
  ASSERT_FLOAT_EQ(0, test_component_->z());

  ASSERT_FLOAT_EQ(0, test_component_->temp());
  ASSERT_FLOAT_EQ(OneHundredCinK, test_component_->temp_lim());
  ASSERT_EQ(NULL, test_component_->thermal_model());
  ASSERT_EQ(NULL, test_component_->nuclide_model());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, initFunctionNoXML) { 
  EXPECT_THROW(test_component_->init(name_, type_, inner_radius_, outer_radius_, 
        NULL, NULL), CycException);
  EXPECT_NO_THROW(test_component_->init(name_, type_, inner_radius_, outer_radius_, 
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
  Component* test_copy = new Component();
  ASSERT_THROW(test_copy->copy(test_copy), CycException);

  EXPECT_NO_THROW(test_component_->init(name_, type_, inner_radius_, outer_radius_, 
        thermal_model_, nuclide_model_));
  EXPECT_NO_THROW(test_copy->copy(test_component_));

  EXPECT_EQ(inner_radius_, test_copy->inner_radius());
  delete test_copy;
}
