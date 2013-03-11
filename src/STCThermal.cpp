/*! \file STCThermal.cpp
    \brief Implements the STCThermal class, an example of a concrete ThermalModel
    \author Kathryn D. Huff
 */
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "MatTools.h"
#include "STCThermal.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCThermal::STCThermal():
  alpha_th_(0),
  k_th_(0),
  mat_("clay"),
  spacing_(0)
{
  set_geom(GeometryPtr(new Geometry()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCThermal::STCThermal(QueryEngine* qe):
  alpha_th_(0),
  k_th_(0),
  mat_("clay"),
  spacing_(0)
{
  set_geom(GeometryPtr(new Geometry()));
  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::initModuleMembers(QueryEngine* qe){
  set_alpha_th(lexical_cast<double>(qe->getElementContent("alpha_th")));
  set_k_th(lexical_cast<double>(qe->getElementContent("k_th")));
  set_mat(lexical_cast<string>(qe->getElementContent("material_data")));
  set_spacing(lexical_cast<double>(qe->getElementContent("spacing")));
  LOG(LEV_DEBUG2,"GRSThm") << "The STCThermal Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::copy(const ThermalModel& src){
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


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::set_alpha_th(double alpha_th){
  MatTools::validate_finite_pos(alpha_th);
  alpha_th_=alpha_th;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::set_k_th(double k_th){
  MatTools::validate_finite_pos(k_th);
  k_th_=k_th;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::set_spacing(double spacing){
  MatTools::validate_finite_pos(spacing);
  spacing_=spacing;
};

