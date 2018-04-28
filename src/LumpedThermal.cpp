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
  LOG(LEV_DEBUG2,"GRSThm") << "The LumpedThermal Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedThermal::copy(const ThermalModel& src){
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
bool LumpedThermal::mat_acceptable(mat_rsrc_ptr mat, Radius r_lim, Temp t_lim){
  /// @TODO obviously, put some logic here.
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp LumpedThermal::temp(){
  return temperature_;
}
