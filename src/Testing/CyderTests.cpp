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
  wfname_ = "wf_name";
  wfinnerradius_ = 0;
  wfouterradius_ = 1;
  wftype_ = "WF";
  wpname_ = "wp_name";
  wpinnerradius_ = 1;
  wpouterradius_ = 2;
  wptype_ = "WP";
  bname_ = "wp_name";
  binnerradius_ = 2;
  bouterradius_ = 20;
  btype_ = "BUFFER";
  ffname_ = "ff_name";
  ffinnerradius_ = 20;
  ffouterradius_ = 100;
  fftype_ = "FF";
  src_facility = initSrcFacility();
  initWorld();

  hot_comp_ = CompMapPtr(new CompMap(MASS));
  cold_comp_ = CompMapPtr(new CompMap(MASS));

  hot_mat _ =mat_rsrc_ptr(new Material(hot_comp_));
  cold_mat_ =mat_rsrc_ptr(new Material(cold_comp_));
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
      
      stringstream wfs("");
      wfs << "  <component>"
         << "    <name>" << wfname_ << "</name>" 
         << "    <innerradius>" << wfinnerradius_ << "</innerradius>" 
         << "    <outerradius>" << wfouterradius_ << "</outerradius>" 
         << "    <type>" << wftype_ << "</type>" 
         << "    <allowedcommod>" << in_commod_ << "</allowedcommod>" 
         << st
         << sn
         << "  </component>";

      stringstream wps("");
      wps << "  <component>"
         << "    <name>" << wpname_ << "</name>" 
         << "    <innerradius>" << wpinnerradius_ << "</innerradius>" 
         << "    <outerradius>" << wpouterradius_ << "</outerradius>" 
         << "    <wptype>" << wptype_ << "</wptype>" 
         << "    <allowedwf>" << wfname_ << "</allowedwf>" 
         << st
         << sn
         << "  </component>";

      stringstream bs("");
      bs << "  <component>"
         << "    <name>" << bname_ << "</name>" 
         << "    <innerradius>" << binnerradius_ << "</innerradius>" 
         << "    <outerradius>" << bouterradius_ << "</outerradius>" 
         << "    <type>" << btype_ << "</type>" 
         << st
         << sn
         << "  </component>";

      stringstream ffs("");
      ffs << "  <component>"
         << "    <name>" << ffname_ << "</name>" 
         << "    <innerradius>" << ffinnerradius_ << "</innerradius>" 
         << "    <outerradius>" << ffouterradius_ << "</outerradius>" 
         << "    <type>" << fftype_ << "</type>" 
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
TEST_F(GenericRepositoryTest, set_thermal_limit){
  double radius = 3;
  double temp = 100; // kelvin?
  EXPECT_NO_THROW(src_facility->set_thermal_limit_radius(radius));
  EXPECT_NO_THROW(src_facility->set_thermal_limit_temp(temp));
  EXPECT_FLOAT_EQ(radius, src_facility->thermal_limit_radius());
  EXPECT_FLOAT_EQ(temp, src_facility->thermal_limit_temp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GenericRepositoryTest, reject_all_0_thermal_lim){
  EXPECT_NO_THROW(src_facility->set_thermal_limit_temp(0))
  EXPECT_NO_THROW(src_facility->set_thermal_limit_radius(0.2))
  EXPECT_BOOL_EQ(false, src_facility->mat_acceptable(hot_mat));
  EXPECT_BOOL_EQ(false, src_facility->mat_acceptable(cold_mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(GenericRepositoryTest, accept_all_high_thermal_lim){
  EXPECT_NO_THROW(src_facility->set_thermal_limit_temp(numeric_limits<double>::infinity()))
  EXPECT_NO_THROW(src_facility->set_thermal_limit_radius(0.2))
  EXPECT_BOOL_EQ(true, src_facility->mat_acceptable(hot_mat));
  EXPECT_BOOL_EQ(true, src_facility->mat_acceptable(cold_mat));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
INSTANTIATE_TEST_CASE_P(CyderFac, FacilityModelTests, Values(&CyderFacilityConstructor));
INSTANTIATE_TEST_CASE_P(CyderFac, ModelTests, Values(&CyderModelConstructor));

