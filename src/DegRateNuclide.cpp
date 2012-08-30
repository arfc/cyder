/*! \file DegRateNuclide.cpp
    \brief Implements the DegRateNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>
#include <assert.h>

#include "CycException.h"
#include "InputXML.h"
#include "Logger.h"
#include "Timer.h"
#include "DegRateNuclide.h"
#include "Material.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::DegRateNuclide(){
  set_deg_rate(0);

  vec_hist_ = VecHist();
  vec_hist_.insert(make_pair(0, make_pair(IsoVector(),0)));

  conc_hist_ = ConcHist();
  IsoConcMap zero_map;
  zero_map.insert(make_pair(92235,0));
  conc_hist_.insert(make_pair(0, zero_map));

  last_degraded_ = 0;
  tot_deg_ = 0;
  set_geom(GeometryPtr(new Geometry()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::~DegRateNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/DegRateNuclide");
  double deg_rate = strtod(XMLinput->get_xpath_content(cur, "degradation"), NULL);
  init(deg_rate);
  LOG(LEV_DEBUG2,"GRDRNuc") << "The DegRateNuclide Class init(cur) function has been called";;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::init(double deg_rate) {
  if (deg_rate_ < 0 | deg_rate_ > 1) {
    string err = "Expected a fractional degradation rate. The value provided: ";
    err += deg_rate_ ;
    err += ", is not between 0 and 1 (inclusive).";
    LOG(LEV_ERROR,"GRDRNuc") << err ;;
    throw CycException(err);
  } else {
    deg_rate_ = deg_rate;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* DegRateNuclide::copy(NuclideModel* src){
  DegRateNuclide* toRet = new DegRateNuclide();
  deg_rate_ = dynamic_cast<DegRateNuclide*>(src)->deg_rate_;
  last_degraded_ = TI->time();
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::print(){
    LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::extract(mat_rsrc_ptr matToRem)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  update_hist(time);
  set_bcs(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_deg_rate(double deg_rate){
  if( deg_rate < 0 || deg_rate > 1 ) {
    string msg = "The DegRateNuclide degradation rate range is 0 to 1, inclusive.";
    msg += " The value provided was ";
    msg += deg_rate;
    msg += ".";
    throw CycRangeException(msg);
  } else {
    deg_rate_ = deg_rate;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(int time){ 
  return (*vec_hist_.find(time)).second.second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(){
  return contained_mass(last_degraded_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr DegRateNuclide::source_term_bc(){
  mat_rsrc_ptr src_term = mat_rsrc_ptr( new Material( contained_vec(last_degraded_) ) );
  src_term->setQuantity( tot_deg()*contained_mass(last_degraded_) );
  return src_term;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::dirichlet_bc(){
  IsoConcMap dirichlet, whole_vol;
  whole_vol = conc_hist(last_degraded_);
  IsoConcMap::const_iterator it;
  for( it=whole_vol.begin(); it!=whole_vol.end(); ++it){
    dirichlet.insert(make_pair((*it).first, tot_deg()*(*it).second));
  }
  return dirichlet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::neumann_bc(){
  /// @TODO This is just a placeholder
  return conc_hist(last_degraded_); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::cauchy_bc(){
  /// @TODO This is just a placeholder
  return conc_hist(last_degraded_); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_bcs(int time){
  set_source_term_bc(time, vec_hist(time));
  set_dirichlet_bc(time, conc_hist(time));
  set_neumann_bc(time, conc_hist(time));
  set_cauchy_bc(time, conc_hist(time));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_source_term_bc(int time, pair<IsoVector,double> vec){ 
  // the source term is just the contained material times the current degradation 
  // That'll be part of each contained waste
  // nothing really to set
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_dirichlet_bc(int time, IsoConcMap conc_map){
  // using the information about the available material,
  // the available concentration should be that material,
  // divided by the volume.
  // it should be a ConcMap to give the concentration for each isotope
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_neumann_bc(int time, IsoConcMap conc_map){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_cauchy_bc(int time, IsoConcMap conc_map){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::update_hist(int time){
  update_degradation(time, deg_rate_);
  update_vec_hist(time);
  update_conc_hist(time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::update_conc_hist(int time, vector<mat_rsrc_ptr> wastes){

  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair; 
  sum_pair = make_pair(vec_hist(time).first, vec_hist(time).second);

  double scale = sum_pair.second/geom_->volume();

  CompMap::iterator it;
  CompMapPtr curr_comp = sum_pair.first.comp();
  for(it = (*curr_comp).begin(); it != (*curr_comp).end(); ++it) {
    to_ret.insert(make_pair((*it).first, ((*it).second)*scale));
  }
  conc_hist_.insert(make_pair(time, to_ret));

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::sum_mats(vector<mat_rsrc_ptr> mats){
  IsoVector vec_to_add, vec;
  vector<mat_rsrc_ptr>::iterator mat;
  double kg = 0;
  double this_mass = 0;
  double ratio = 0;

  for(mat = mats.begin(); mat != mats.end(); ++mat){ 
    this_mass = (*mat)->mass(KG);
    kg += this_mass;
    ratio = this_mass/kg;
    vec_to_add = IsoVector((*mat)->isoVector().comp());
    vec.mix(vec_to_add, ratio);
  }

  vec.normalize();
  return make_pair(vec, kg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::update_degradation(int time, double deg_rate){
  assert(last_degraded_ <= time);
  assert(deg_rate<=1.0 && deg_rate >= 0.0);
  double total;
  total = tot_deg_ + deg_rate*(time - last_degraded_);
  tot_deg_ = min(1.0, total);
  last_degraded_ = time;

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::tot_deg(){
  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::update_vec_hist(int time){
  vec_hist_.insert( make_pair( time, sum_mats(wastes_)) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector DegRateNuclide::contained_vec(int time){
  IsoVector to_ret = vec_hist(time).first;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::vec_hist(int time){
  pair<IsoVector, double> to_ret;
  VecHist::iterator it;
  it = vec_hist_.find(time);
  if( it != vec_hist_.end() ){
    to_ret = (*it).second;
  } else {
    to_ret = make_pair(IsoVector(),0); // zero
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::conc_hist(int time){
  IsoConcMap to_ret;
  ConcHist::iterator it;
  it = conc_hist_.find(time);
  if( it != conc_hist_.end() ){
    to_ret = (*it).second;
  } else {
    to_ret.insert(make_pair(92235,0)); // zero
  }
  return to_ret;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Concentration DegRateNuclide::conc_hist(int time, Iso tope){
  Concentration to_ret;
  IsoConcMap conc_map = conc_hist(time);
  IsoConcMap::iterator it;
  it = conc_map.find(tope);
  if(it != conc_map.end()){
    to_ret = (*it).second;
  }else{
    to_ret = 0;
  }
  return to_ret;
}


