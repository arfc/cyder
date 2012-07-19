// ComponentTests.cpp
#include <gtest/gtest.h>

#include "Component.h"

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

    virtual void SetUp(){
      test_component = new FakeComponent();
      OneHundredCinK=373;
    }
    virtual void TearDown() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, defaultConstructor) {
  ASSERT_EQ("", test_component->name());
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


