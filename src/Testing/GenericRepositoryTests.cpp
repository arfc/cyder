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
  initSrcFacility();
  initWorld();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepositoryTest::TearDown() { 
  delete src_facility;
  delete incommod_market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepositoryTest::initSrcFacility(){

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, FacilityModelTests, Values(&GenericRepositoryFacilityConstructor));
INSTANTIATE_TEST_CASE_P(GenericRepositoryFac, ModelTests, Values(&GenericRepositoryModelConstructor));


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 







