/*! \file DegRateNuclide.cpp
    \brief Implements the DegRateNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
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
  tot_deg_ = 0;
  last_degraded_ = 0;
  wastes_ = deque<mat_rsrc_ptr>();

  set_geom(GeometryPtr(new Geometry()));

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_degradation(0, deg_rate_);
  update_vec_hist(0);
  //update_conc_hist(0);
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
  set_deg_rate(deg_rate);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* DegRateNuclide::copy(NuclideModel* src){
  DegRateNuclide* src_ptr = dynamic_cast<DegRateNuclide*>(src);

  set_deg_rate(src_ptr->deg_rate_);
  tot_deg_ = 0;
  last_degraded_ = TI->time();
  wastes_ = deque<mat_rsrc_ptr>();

  // copy the geometry AND the centroid. It should be reset later.
  set_geom(GeometryPtr(new Geometry()));
  geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid());

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_degradation(TI->time(), deg_rate_);
  update_vec_hist(TI->time());
  update_conc_hist(TI->time());

  return (NuclideModel*)this;
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
void DegRateNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  mat_rsrc_ptr left_over = mat_rsrc_ptr(new Material(comp_to_rem));
  left_over->setQuantity(0);
  while (!wastes_.empty()){
    left_over->absorb(wastes_.back());
    wastes_.pop_back();
  }
  left_over->extract(comp_to_rem, kg_to_rem);
  wastes_.push_back(left_over);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  update_degradation(the_time, deg_rate_);
  update_vec_hist(the_time);
  update_conc_hist(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_deg_rate(double deg_rate){
  if( deg_rate < 0 || deg_rate > 1 ) {
    stringstream msg_ss;
    msg_ss << "The DegRateNuclide degradation rate range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << deg_rate;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    deg_rate_ = deg_rate;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(int the_time){ 
  return vec_hist(the_time).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(){
  return contained_mass(last_degraded_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::source_term_bc(){
  return make_pair(contained_vec(last_degraded_), 
      tot_deg()*contained_mass(last_degraded_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::dirichlet_bc(){
  IsoConcMap dirichlet, whole_vol;
  whole_vol = conc_hist(last_degraded_);
  IsoConcMap::const_iterator it;
  for( it=whole_vol.begin(); it!=whole_vol.end(); ++it){
    dirichlet[(*it).first] = tot_deg()*(*it).second ;
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
IsoConcMap DegRateNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_degraded_ <= the_time);

  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair; 
  sum_pair = vec_hist(the_time);

  if(sum_pair.second != 0 && geom_->volume() != numeric_limits<double>::infinity()) { 
    double scale = sum_pair.second/geom_->volume();
    CompMapPtr curr_comp = sum_pair.first.comp();
    CompMap::const_iterator it;
    for(it = (*curr_comp).begin(); it != (*curr_comp).end(); ++it) {
      to_ret[ (*it).first ] = ((*it).second)*scale ;
    }
  } else {
    to_ret[ 92235 ] = 0; 
  }
  conc_hist_[the_time] = to_ret ;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::sum_mats(deque<mat_rsrc_ptr> mats){
  IsoVector vec;
  CompMapPtr sum_comp = CompMapPtr(new CompMap(MASS));
  double kg = 0;
  double mass_to_add;

  if( mats.size() != 0 ){ 
    CompMapPtr comp_to_add;
    deque<mat_rsrc_ptr>::iterator mat;
    int iso;
    CompMap::const_iterator comp;

    for(mat = mats.begin(); mat != mats.end(); ++mat){ 
      kg += (*mat)->mass(MassUnit(KG));
      comp_to_add = (*mat)->isoVector().comp();
      comp_to_add->massify();
      for(comp = (*comp_to_add).begin(); comp != (*comp_to_add).end(); ++comp) {
        iso = comp->first;
        if(sum_comp->count(iso)!=0) {
          (*sum_comp)[iso] = (*sum_comp)[iso] + (comp->second)*kg;
        } else { 
          (*sum_comp)[iso] = (comp->second)*kg;
        }
      }
    }
  } else {
    (*sum_comp)[92235] = 0;
  }
  vec = IsoVector(sum_comp);
  return make_pair(vec, kg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::update_degradation(int the_time, double deg_rate){
  assert(last_degraded_ <= the_time);
  assert(deg_rate<=1.0 && deg_rate >= 0.0);
  double total = 0;
  total = tot_deg() + deg_rate*(the_time - last_degraded_);
  tot_deg_ = min(1.0, total);
  last_degraded_ = the_time;

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::tot_deg(){
  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::update_vec_hist(int the_time){
  // CompMapPtr one_comp = CompMapPtr(new CompMap(MASS));
  // (*one_comp)[92235] = 1;
  // vec_hist_[ the_time ] = make_pair(IsoVector(one_comp), 10*(1-tot_deg()));
  vec_hist_[ the_time ] = sum_mats(wastes_) ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector DegRateNuclide::contained_vec(int the_time){
  IsoVector to_ret = vec_hist(the_time).first;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::vec_hist(int the_time){
  pair<IsoVector, double> to_ret;
  VecHist::const_iterator it;
  if( !vec_hist_.empty() ) {
    it = vec_hist_.find(the_time);
    if( it != vec_hist_.end() ){
      to_ret = (*it).second;
      assert(to_ret.second < 1000 );
    } 
  } else { 
    CompMapPtr zero_comp = CompMapPtr(new CompMap(MASS));
    (*zero_comp)[92235] = 0;
    to_ret = make_pair(IsoVector(zero_comp),0);
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::conc_hist(int the_time){
  IsoConcMap to_ret;
  ConcHist::iterator it;
  it = conc_hist_.find(the_time);
  if( the_time == last_degraded_ && it != conc_hist_.end() ){
    to_ret = (*it).second;
  } else {
    to_ret[92235] = 0 ; // zero
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Concentration DegRateNuclide::conc_hist(int the_time, Iso tope){
  Concentration to_ret;
  IsoConcMap conc_map = conc_hist(the_time);
  IsoConcMap::iterator it;
  it = conc_map.find(tope);
  if(it != conc_map.end()){
    to_ret = (*it).second;
  }else{
    to_ret = 0;
  }
  return to_ret;
}


