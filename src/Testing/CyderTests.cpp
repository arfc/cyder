// CyderTests.cpp
#include <gtest/gtest.h>
#include <dlfcn.h>

#include "Cyder.h"
#include "CyderTests.h"
#include "XMLQueryEngine.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CyderTest::SetUp(){
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
void CyderTest::TearDown() { 
  delete src_facility;
  delete incommod_market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Cyder* CyderTest::initSrcFacility(){

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
      src_facility = new Cyder();
      src_facility->initModuleMembers(engine);
      delete engine;
      return src_facility;
    }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void CyderTest::initWorld(){
  incommod_market = new TestMarket();
  incommod_market->setCommodity(in_commod_);
  MarketModel::registerMarket(incommod_market);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST_F(CyderTest, initial_state) {
  EXPECT_EQ(capacity_, src_facility->getCapacity(in_commod_));
  EXPECT_EQ(adv_vel_, src_facility->adv_vel());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(CyderFac, FacilityModelTests, Values(&CyderFacilityConstructor));
INSTANTIATE_TEST_CASE_P(CyderFac, ModelTests, Values(&CyderModelConstructor));

