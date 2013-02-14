// GenericRepositoryTests.cpp
#include <gtest/gtest.h>
#include <dlfcn.h>

#include "GenericRepository.h"
#include "GenericRepositoryTests.h"
#include "XMLQueryEngine.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepositoryTest::SetUp(){
  // initialize ordinary objects
  x_ = 10;
  y_ = 10;
  z_ = 10;
  dx_ = 1;
  dy_ = 1;
  dz_ = 1;
  adv_vel_ = .000631;
  capacity_ = 100;
  in_commod_ = "in_commod";
  inventory_size_ = 70000;
  lifetime_ = 3000000;
  start_op_yr_ = 1; 
  start_op_mo_ = 1;
  cname_ = "component";
  innerradius_ = 0;
  outerradius_ = 100;
  componenttype_ = "FF";
  src_facility = initSrcFacility();
  initWorld();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepositoryTest::TearDown() { 
  delete src_facility;
  delete incommod_market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
GenericRepository* GenericRepositoryTest::initSrcFacility(){

      stringstream st("");
      st << "    <thermalmodel>" 
         << "      <StubThermal/>"
         << "    </thermalmodel>";

      stringstream sn("");
      sn << "    <nuclidemodel>" 
         << "      <StubNuclide/>"
         << "    </nuclidemodel>";
      
      stringstream cs("");
      cs << "  <component>"
         << "    <name>" << cname_ << "</name>" 
         << "    <innerradius>" << innerradius_ << "</innerradius>" 
         << "    <outerradius>" << outerradius_ << "</outerradius>" 
         << "    <componenttype>" << componenttype_ << "</componenttype>" 
         << st
         << sn
         << "  </component>";
    
      stringstream ss("");
      ss << "<start>"
         << "  <x>" << x_ << "</x>"
         << "  <y>" << y_ << "</y>"
         << "  <z>" << z_ << "</z>"
         << "  <dx>" << dx_ << "</dx>"
         << "  <dy>" << dy_ << "</dy>"
         << "  <dz>" << dz_ << "</dz>"
         << "  <advective_velocity>" << adv_vel_ << "</advective_velocity>"
         << "  <capacity>" << capacity_ << "</capacity>"
         << "  <incommodity>" << in_commod_ << "</incommodity>"
         << "  <inventorysize>" << inventory_size_ << "</inventorysize>"
         << "  <lifetime>" << lifetime_ << "</lifetime>"
         << "  <startOperMonth>" << start_op_mo_ << "</startOperMonth>"
         << "  <startOperYear>" << start_op_yr_ << "</startOperYear>"
         << cs  
         << "</start>";

      XMLParser parser(ss);
      XMLQueryEngine* engine = new XMLQueryEngine(parser);
      src_facility = new GenericRepository();
      src_facility->initModuleMembers(engine);
      delete engine;
      return src_facility;
    }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepositoryTest::initWorld(){
  incommod_market = new TestMarket();
  incommod_market->setCommodity(in_commod_);
  MarketModel::registerMarket(incommod_market);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(GenericRepositoryTest, initial_state) {
  EXPECT_EQ(capacity_, src_facility->getCapacity(in_commod_));
  EXPECT_EQ(adv_vel_, src_facility->adv_vel());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GenericRepositoryTest, assess_capacity_crude){
  // after stuff is absorbed, the capacity should be lower than before.
  EXPECT_NO_THROW(src_facility->emplaceWaste());
  EXPECT_NO_THROW(src_facility->handleTick(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GenericRepositoryTest, reject_hot_mat){
  EXPECT_EQ(false, src_facility->mat_acceptable(hot_mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GenericRepositoryTest, accept_cold_mat){
  EXPECT_EQ(true, src_facility->mat_acceptable());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, FacilityModelTests, Values(&GenericRepositoryFacilityConstructor));
INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, ModelTests, Values(&GenericRepositoryModelConstructor));

