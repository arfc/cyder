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
  contained_mass_ = map<int,double>();
  contained_mass_.insert(make_pair(TI->time(), 0));
  conc_hist_ = ConcHist();
  IsoConcMap zero_map;
  zero_map.insert(make_pair(92235,0.0));
  conc_hist_.insert(make_pair(TI->time(), zero_map ));

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
  deg_rate_ = deg_rate;
  last_degraded_ = TI->time();
  if (deg_rate_ < 0 | deg_rate_ > 1) {
    string err = "Expected a fractional degradation rate. The value provided: ";
    err += deg_rate_ ;
    err += ", is not between 0 and 1 (inclusive).";
    LOG(LEV_ERROR,"GRDRNuc") << err ;;
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* DegRateNuclide::copy(NuclideModel* src){
  DegRateNuclide* toRet = new DegRateNuclide();
  deg_rate_ = dynamic_cast<DegRateNuclide*>(src)->deg_rate_;
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
void DegRateNuclide::transportNuclides(){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  //
  // Sometimes it's easier to use the dissolution rate as a source term in the 
  // transport equations
  //
  // Dissolution rate : m_dot[i,t] = C[i,t]*Q;
  // if S is the crossectional area perpendicular to water flow and v is water 
  // velocity : f[i,z,t] = C[i,t] * v * d(z)  = volumetric source term

  // retrieve data about the component
  IsoConcMap current_map = update_hist(TI->time());
  set_bcs(TI->time(), current_map);
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
  return contained_mass_.at(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(){
  return contained_mass(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr DegRateNuclide::source_term_bc(){
  mat_rsrc_ptr src_term= new Material( avail_iso_vec_ );
  src_term->setQuantity(avail_kg_);
  return src_term;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::dirichlet_bc(){
  IsoConcMap dirichlet;
  IsoConcMap whole_vol = conc_hist_.at(TI->time()); 
  IsoConcMap::const_iterator it;
  for( it=whole_vol.begin(); it!=whole_vol.end(); ++it){
    dirichlet.insert(make_pair((*it).first, tot_deg()*(*it).second));
  }
  return dirichlet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::neumann_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time()); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::cauchy_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time()); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_bcs(int time, IsoConcMap conc_map){
  set_source_term_bc(time, conc_map);
  set_dirichlet_bc(time, conc_map);
  set_neumann_bc(time, conc_map);
  set_cauchy_bc(time, conc_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_source_term_bc(int time, IsoConcMap conc_map){ 
  // the source term is just the contained material times the current degradation 
  // That'll be part of each contained waste
  avail_kg_= contained_mass(time)*deg_rate_;
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
IsoConcMap DegRateNuclide::update_hist(int time){
  update_degradation(time, deg_rate_);
  return update_conc_hist(time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::update_conc_hist(int time, vector<mat_rsrc_ptr> wastes){

  IsoConcMap to_ret;
  double kg_sum;
  IsoVector vec_sum;

  sum_mats(wastes);

  avail_iso_vec_ = IsoVector(avail_iso_vec_);
  contained_mass_[time]=avail_kg_;

  double scale = avail_kg_/geom_->volume();

  CompMap::iterator it;
  CompMapPtr curr_comp = avail_iso_vec_.comp();
  for(it = (*curr_comp).begin(); it != (*curr_comp).end(); ++it) {
    to_ret.insert(make_pair((*it).first, ((*it).second)*scale));
  }
  conc_hist_.insert(make_pair(time, to_ret));

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::sum_mats(vector<mat_rsrc_ptr> mats){
  IsoVector vec_to_add, vec;
  vector<mat_rsrc_ptr>::iterator mat;
  avail_kg_ = 0;
  double this_mass = 0;
  double ratio = 0;

  for(mat = mats.begin(); mat != mats.end(); ++mat){ 
    this_mass = (*mat)->mass(KG);
    avail_kg_ += this_mass;
    ratio = this_mass/avail_kg_;
    vec_to_add = IsoVector((*mat)->isoVector().comp());
    vec.mix(vec_to_add, ratio);
  }

  vec.normalize();
  avail_iso_vec_=vec;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::update_degradation(int time, double deg_rate){
  assert(last_degraded_ <= time);
  assert(deg_rate<=1.0 && deg_rate >= 0.0);
  tot_deg_ += deg_rate*(time - last_degraded_);
  last_degraded_ = time;

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::tot_deg(){
  return tot_deg_;
}
