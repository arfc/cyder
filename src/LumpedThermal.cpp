/*! \file LumpedThermal.cpp
    \brief Implements the LumpedThermal class, an example of a concrete ThermalModel
    \author Kathryn D. Huff
 */
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "LumpedThermal.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedThermal::initModuleMembers(QueryEngine* qe){
  // move the xml pointer to the current model
  QueryEngine* lumped_thermal = qe->queryElement("LumpedThermal");
  // for now, just say you've done it... 
  LOG(LEV_DEBUG2,"GRSThm") << "The LumpedThermal Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedThermal* LumpedThermal::deepCopy(){
  LumpedThermal* toRet = new LumpedThermal();
  toRet->copy(this);
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedThermal::copy(ThermalModel* src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedThermal::print(){
    LOG(LEV_DEBUG2,"GRSThm") << "LumpedThermal Model";
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedThermal::transportHeat(int time){
  // This will transport the heat through the component at hand. 
  // It should send some kind of heat object or reset the temperatures. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp LumpedThermal::peak_temp(){
  TempHist::iterator start = temp_hist_.begin();
  TempHist::iterator stop = temp_hist_.end();
  TempHist::iterator max = max_element(start, stop);
  return (*max).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp LumpedThermal::temp(){
  return temperature_;
}
