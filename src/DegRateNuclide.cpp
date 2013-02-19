/*! \file DegRateNuclide.cpp
    \brief Implements the DegRateNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "DegRateNuclide.h"
#include "Material.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::DegRateNuclide():
  deg_rate_(0),
  v_(0),
  tot_deg_(0),
  last_degraded_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();

  set_geom(GeometryPtr(new Geometry()));

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::DegRateNuclide(QueryEngine* qe):
  deg_rate_(0),
  v_(0),
  tot_deg_(0),
  last_degraded_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();

  set_geom(GeometryPtr(new Geometry()));

  initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::~DegRateNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::initModuleMembers(QueryEngine* qe){
  set_v(lexical_cast<double>(qe->getElementContent("advective_velocity")));
  set_deg_rate(lexical_cast<double>(qe->getElementContent("degradation")));
  LOG(LEV_DEBUG2,"GRDRNuc") << "The DegRateNuclide Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr DegRateNuclide::copy(const NuclideModel& src){
  const DegRateNuclide* src_ptr = dynamic_cast<const DegRateNuclide*>(&src);

  set_v(src_ptr->v());
  set_deg_rate(src_ptr->deg_rate());
  set_tot_deg(0);
  set_last_degraded(TI->time());

  // copy the geometry AND the centroid. It should be reset later.
  set_geom(GeometryPtr(new Geometry()));
  geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid());

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_vec_hist(TI->time());
  update_conc_hist(TI->time());

  return shared_from_this();
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
mat_rsrc_ptr DegRateNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  mat_rsrc_ptr to_ret = mat_rsrc_ptr(MatTools::extract(comp_to_rem, kg_to_rem, wastes_));
  update(TI->time());
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  update_degradation(the_time, deg_rate());
  update_vec_hist(the_time);
  update_conc_hist(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_deg_rate(double cur_rate){
  if( cur_rate < 0 || cur_rate > 1 ) {
    stringstream msg_ss;
    msg_ss << "The DegRateNuclide degradation rate range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << cur_rate;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    deg_rate_ = cur_rate;
  }
  assert((cur_rate >=0) && (cur_rate <= 1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(){
  return shared_from_this()->contained_mass(last_degraded());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> DegRateNuclide::source_term_bc(){
  return make_pair(contained_vec(last_degraded()), 
      tot_deg()*shared_from_this()->contained_mass(last_degraded()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::dirichlet_bc(){
  IsoConcMap dirichlet, whole_vol;
  whole_vol = conc_hist(last_degraded());
  IsoConcMap::const_iterator it;
  for( it=whole_vol.begin(); it!=whole_vol.end(); ++it){
    dirichlet[(*it).first] = tot_deg()*(*it).second ;
  }
  return dirichlet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap DegRateNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  ConcGradMap to_ret;

  IsoConcMap c_int = conc_hist(last_degraded());
  Radius r_int = geom_->radial_midpoint();
  
  int iso;
  IsoConcMap::iterator it;
  for( it=c_int.begin(); it != c_int.end(); ++it){
    iso = (*it).first;
    if( c_ext.count(iso) != 0) {  
      // in both
      to_ret[iso] = calc_conc_grad(c_ext[iso], c_int[iso]*tot_deg(), r_ext, r_int);
    } else {  
      // in c_int_only
      to_ret[iso] = calc_conc_grad(0, c_int[iso]*tot_deg(), r_ext, r_int);
    }
  }
  for( it=c_ext.begin(); it != c_ext.end(); ++it){
    iso = (*it).first;
    if( c_int.count(iso) == 0) { 
      // in c_ext only
      to_ret[iso] = calc_conc_grad(c_ext[iso], 0, r_ext, r_int);
    }
  }

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap DegRateNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  // -D dC/dx + v_xC = v_x C
  IsoFluxMap to_ret;
  ConcGradMap neumann = neumann_bc(c_ext, r_ext);
  ConcGradMap::iterator it;
  Iso iso;
  Elem elem;
  for( it = neumann.begin(); it != neumann.end(); ++it){
    iso = (*it).first;
    elem = iso/1000;
    to_ret.insert(make_pair(iso, -mat_table_->D(elem)*(*it).second + v()*shared_from_this()->dirichlet_bc(iso)));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap DegRateNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_degraded() <= the_time);

  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair; 
  sum_pair = vec_hist(the_time);

  int iso;
  double conc;
  if(sum_pair.second != 0 && geom_->volume() != numeric_limits<double>::infinity()) { 
    double scale = sum_pair.second/geom_->volume();
    CompMapPtr curr_comp = sum_pair.first.comp();
    CompMap::const_iterator it;
    it=(*curr_comp).begin();
    while(it != (*curr_comp).end() ) {
      iso = (*it).first;
      conc = (*it).second;
      to_ret.insert(make_pair(iso, conc*scale));
      ++it;
    }
  } else {
    to_ret[ 92235 ] = 0; 
  }
  conc_hist_[the_time] = to_ret ;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::update_degradation(int the_time, double cur_rate){
  assert(last_degraded() <= the_time);
  if(cur_rate != deg_rate()){
    set_deg_rate(cur_rate);
  };
  double total = tot_deg() + deg_rate()*(the_time - last_degraded());
  set_tot_deg(min(1.0, total));
  set_last_degraded(the_time);

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::update_vec_hist(int the_time){
  vec_hist_[ the_time ] = MatTools::sum_mats(wastes_) ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters){
  std::map<NuclideModelPtr, std::pair<IsoVector,double> > to_ret;
  std::vector<NuclideModelPtr>::iterator daughter;
  std::pair<IsoVector, double> source_term;
  for( daughter = daughters.begin(); daughter!=daughters.end(); ++daughter){
    source_term = (*daughter)->source_term_bc();
    if( source_term.second > 0 ){
      absorb((*daughter)->extract(source_term.first.comp(), source_term.second));
    }
  }
}

