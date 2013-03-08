/*! \file STCThermal.cpp
    \brief Implements the STCThermal class, an example of a concrete ThermalModel
    \author Kathryn D. Huff
 */
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "STCThermal.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::initModuleMembers(QueryEngine* qe){
  LOG(LEV_DEBUG2,"GRSThm") << "The STCThermal Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCThermalPtr STCThermal::deepCopy(){
  STCThermalPtr toRet = STCThermalPtr(new STCThermal());
  toRet->copy(STCThermalPtr(this));
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::copy(ThermalModelPtr src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::print(){
    LOG(LEV_DEBUG2,"GRSThm") << "STCThermal Model";
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::transportHeat(int time){
  // This will transport the heat through the component at hand. 
  // It should send some kind of heat object or reset the temperatures. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp STCThermal::peak_temp(){
  TempHist::iterator start = temp_hist_.begin();
  TempHist::iterator stop = temp_hist_.end();
  TempHist::iterator max = max_element(start, stop);
  return (*max).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool STCThermal::mat_acceptable(mat_rsrc_ptr mat, Radius r_lim, Temp t_lim){
  /// @TODO obviously, put some logic here.
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp STCThermal::temp(){
  return temperature_;
}
