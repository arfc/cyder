// ComponentTests.cpp
#include <gtest/gtest.h>

#include "Component.h"
#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class FakeComponent : public Component {
  public:
    FakeComponent() : Component() {

      // initialize ordinary objects

      // initialize things that don't depend on the input
    }

    virtual ~FakeComponent() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
class ComponentTest : public ::testing::Test {
  protected:
    FakeComponent* test_component;
    Temp OneHundredCinK;
    string name;
    ComponentType type;
    Radius inner_radius, outer_radius;
    ThermalModel* thermal_model;
    NuclideModel* nuclide_model;

    virtual void SetUp(){
      test_component = new FakeComponent();
      OneHundredCinK=373;
      name = "Test";
      type = BUFFER;
      inner_radius = 2;
      outer_radius = 10;
      thermal_model=NULL;
      nuclide_model=NULL;
    }
    virtual void TearDown() {
      delete test_component;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, defaultConstructor) {
  ASSERT_EQ("", test_component->name());
  ASSERT_EQ(LAST_EBS, test_component->type());
  ASSERT_FLOAT_EQ(0,test_component->inner_radius());
  ASSERT_EQ(NULL, test_component->outer_radius());

  ASSERT_FLOAT_EQ(0, test_component->centroid().x_);
  ASSERT_FLOAT_EQ(0, test_component->centroid().y_);
  ASSERT_FLOAT_EQ(0, test_component->centroid().z_);

  ASSERT_FLOAT_EQ(0, test_component->x());
  ASSERT_FLOAT_EQ(0, test_component->y());
  ASSERT_FLOAT_EQ(0, test_component->z());

  ASSERT_FLOAT_EQ(0, test_component->temp());
  ASSERT_FLOAT_EQ(OneHundredCinK, test_component->temp_lim());
  ASSERT_EQ(NULL, test_component->thermal_model());
  ASSERT_EQ(NULL, test_component->nuclide_model());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, initFunctionNoXML) { 
  EXPECT_NO_THROW(test_component->init(name, type, inner_radius, outer_radius, 
        thermal_model, nuclide_model));
  ASSERT_EQ(name, test_component->name());
  ASSERT_EQ(type, test_component->type());
  ASSERT_EQ(inner_radius, test_component->inner_radius());
  ASSERT_EQ(outer_radius, test_component->outer_radius());
  ASSERT_EQ(thermal_model, test_component->thermal_model());
  ASSERT_EQ(nuclide_model, test_component->nuclide_model());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, copy) {
  EXPECT_NO_THROW(test_component->init(name, type, inner_radius, outer_radius, 
        thermal_model, nuclide_model));
  Component* test_copy = new Component();
  ASSERT_THROW(test_copy->copy(test_component), CycException);
  delete test_copy;
}
