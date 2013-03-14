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
  spacing_(0),
  r_calc_(0),
  mat_("")
{
  set_geom(GeometryPtr(new Geometry()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STCThermal::STCThermal(QueryEngine* qe):
  alpha_th_(0),
  k_th_(0),
  spacing_(0),
  r_calc_(0),
  mat_("")
{
  set_geom(GeometryPtr(new Geometry()));
  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::initModuleMembers(QueryEngine* qe){
  int n_predef_mats = qe->nElementsMatchingQuery("material_data");
  if(n_predef_mats == 1 ){
    set_mat(lexical_cast<string>(qe->getElementContent("material_data")));
    /// @TODO use this to set alpha, k, s, r
  } else {
    set_alpha_th(lexical_cast<double>(qe->getElementContent("alpha_th")));
    set_k_th(lexical_cast<double>(qe->getElementContent("k_th")));
    set_spacing(lexical_cast<double>(qe->getElementContent("spacing")));
    set_r_calc(lexical_cast<double>(qe->getElementContent("r_calc")));
  }
  LOG(LEV_DEBUG2,"GRSThm") << "The STCThermal Class init(cur) function has been called";;
  initializeSTCTable();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void STCThermal::copy(const ThermalModel& src){
  const STCThermal* src_ptr = dynamic_cast<const STCThermal*>(&src);
  set_alpha_th(src_ptr->alpha_th());
  set_k_th(src_ptr->k_th());
  set_spacing(src_ptr->spacing());
  set_r_calc(src_ptr->r_calc());
  set_mat(src_ptr->mat());
  set_geom(GeometryPtr(new Geometry()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::print(){
    LOG(LEV_DEBUG2,"GRSThm") << "STCThermal Model";
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::transportHeat(int the_time){
  // This will transport the heat through the component at hand. 
  // It should send some kind of heat object or reset the temperatures. 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::initializeSTCTable(){
  mat_t mat;
  mat.a(alpha_th_).k(k_th_).s(spacing_).r(r_calc_);
  table_ = STCDataTablePtr(SDB->table(mat));
  ///@TODO this is where the interpolation must occur.
  ///@TODO you want to make a NEW STCDataTable out of whatever tables, indexes, 
  //etc that you need to, then set it to stc_table_.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp STCThermal::getTempChange(Iso tope, int the_time) {
  return table_->stc(tope, the_time); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::map<int, Temp> STCThermal::getTempChange(mat_rsrc_ptr mat){
  map<int, Temp> to_ret;
  CompMapPtr comp = mat->isoVector().comp();
  CompMap::iterator it;
  map<int,int>::iterator step;
  Iso iso;
  int time_ind;
  for(it=(*comp).begin(); it!=(*comp).end(); ++it) {
    iso = (*it).first;
    for(step=table_->timeIndex().begin(); step!=table_->timeIndex().end(); ++step){
      time_ind=(*step).second;
      to_ret[time_ind] += getTempChange(iso, time_ind)*mat->mass(iso);
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp STCThermal::getTempChange(mat_rsrc_ptr mat, int the_time){
  Temp to_ret;
  try{
    to_ret = getTempChange(mat).at(the_time);
  } catch (const std::out_of_range& oor) {
    stringstream msg_ss;
    msg_ss << oor.what();
    msg_ss << "The time ";
    msg_ss << the_time;
    msg_ss << " is not contained in the temperature change history."; 
    LOG(LEV_ERROR, "CydSTC") << msg_ss.str();
    throw CycRangeException(msg_ss.str());
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::pair<int,Temp> STCThermal::getMaxTempChange(mat_rsrc_ptr mat){
  std::map<int, Temp> temp_map = getTempChange(mat);
  map<int, Temp>::iterator it;
  int max_time = 0;
  Temp max_val = 0;
  
  for(it=temp_map.begin(); it!=temp_map.end(); ++it){
    if( (*it).second > max_val ) {
      max_val = (*it).second;
      max_time = (*it).first;
    }
  }
  return make_pair(max_time, max_val);
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
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void STCThermal::set_r_calc(double r_calc){
  MatTools::validate_finite_pos(r_calc);
  r_calc_=r_calc;
};

