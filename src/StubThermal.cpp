/*! \file StubThermal.cpp
    \brief Implements the StubThermal class, an example of a concrete ThermalModel
    \author Kathryn D. Huff
 */
#include <iostream>
#include <algorithm>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "StubThermal.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::initModuleMembers(QueryEngine* qe){
  // for now, just say you've done it... 
  LOG(LEV_DEBUG2,"GRSThm") << "The StubThermal Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubThermalPtr StubThermal::deepCopy(){
  StubThermalPtr toRet = StubThermalPtr(new StubThermal());
  toRet->copy(StubThermalPtr(this));
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::copy(ThermalModelPtr src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubThermal::print(){
    LOG(LEV_DEBUG2,"GRSThm") << "StubThermal Model";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubThermal::transportHeat(int time){
  // This will transport the heat through the component at hand. 
  // It should send some kind of heat object or reset the temperatures. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::peak_temp(){
  TempHist::iterator start = temp_hist_.begin();
  TempHist::iterator stop = temp_hist_.end();
  TempHist::iterator max = max_element(start, stop);
  return (*max).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::temp(){
  return temperature_;
}

