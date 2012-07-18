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

    virtual void SetUp(){
      test_component = new FakeComponent();
    }
    virtual void TearDown() {
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(ComponentTest, defaultConstructor) {
  ASSERT_EQ(test_component->name(), "");
  ASSERT_EQ(test_component->inner_radius(), 0);
  ASSERT_EQ(test_component->outer_radius(), NULL);
}


