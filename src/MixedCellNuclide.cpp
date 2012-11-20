/*! \file MixedCellNuclide.cpp
    \brief Implements the MixedCellNuclide class used by the Generic Repository 
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
#include "MixedCellNuclide.h"
#include "Material.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide():
  deg_rate_(0),
  v_(0),
  D_(0),
  tot_deg_(0),
  last_degraded_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();

  set_geom(GeometryPtr(new Geometry()));

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide(QueryEngine* qe):
  deg_rate_(0),
  v_(0),
  D_(0),
  tot_deg_(0),
  last_degraded_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();

  set_geom(GeometryPtr(new Geometry()));

  this->initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::~MixedCellNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::initModuleMembers(QueryEngine* qe){
  set_v(lexical_cast<double>(qe->getElementContent("advective_velocity")));
  set_deg_rate(lexical_cast<double>(qe->getElementContent("degradation")));
  set_D(lexical_cast<double>(qe->getElementContent("diffusion_coeff")));
  LOG(LEV_DEBUG2,"GRDRNuc") << "The MixedCellNuclide Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* MixedCellNuclide::copy(NuclideModel* src){
  MixedCellNuclide* src_ptr = dynamic_cast<MixedCellNuclide*>(src);

  set_v(src_ptr->v());
  set_deg_rate(src_ptr->deg_rate());
  set_D(src_ptr->D());
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

  return (NuclideModel*)this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::print(){
    LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "MixedCellNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  MatTools::extract(comp_to_rem, kg_to_rem, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::transportNuclides(int the_time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  update_degradation(the_time, this->deg_rate());
  update_vec_hist(the_time);
  update_conc_hist(the_time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::set_deg_rate(double cur_rate){
  if( cur_rate < 0 || cur_rate > 1 ) {
    stringstream msg_ss;
    msg_ss << "The MixedCellNuclide degradation rate range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << cur_rate;
    msg_ss <<  ".";
    LOG(LEV_ERROR,"GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  } else {
    this->deg_rate_ = cur_rate;
  }
  assert((cur_rate >=0) && (cur_rate <= 1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::contained_mass(){
  return dynamic_cast<NuclideModel*>(this)->contained_mass(last_degraded());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> MixedCellNuclide::source_term_bc(){
  return make_pair(contained_vec(last_degraded()), 
      tot_deg()*dynamic_cast<NuclideModel*>(this)->contained_mass(last_degraded()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::dirichlet_bc(){
  IsoConcMap dirichlet, whole_vol;
  whole_vol = conc_hist(last_degraded());
  IsoConcMap::const_iterator it;
  for( it=whole_vol.begin(); it!=whole_vol.end(); ++it){
    dirichlet[(*it).first] = tot_deg()*(*it).second ;
  }
  return dirichlet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap MixedCellNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
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
IsoFluxMap MixedCellNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  // -D dC/dx + v_xC = v_x C
  IsoFluxMap to_ret;
  ConcGradMap neumann = neumann_bc(c_ext, r_ext);
  ConcGradMap::iterator it;
  Iso iso;
  for( it = neumann.begin(); it != neumann.end(); ++it){
    iso = (*it).first;
    to_ret.insert(make_pair(iso, -D()*(*it).second + v()*dynamic_cast<NuclideModel*>(this)->dirichlet_bc(iso)));
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double MixedCellNuclide::update_degradation(int the_time, double cur_rate){
  assert(last_degraded() <= the_time);
  if(cur_rate != this->deg_rate()){
    set_deg_rate(cur_rate);
  };
  double total = this->tot_deg() + this->deg_rate()*(the_time - last_degraded());
  set_tot_deg(min(1.0, total));
  set_last_degraded(the_time);

  return tot_deg_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::update_vec_hist(int the_time){
  vec_hist_[ the_time ] = MatTools::sum_mats(wastes_) ;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::update_conc_hist(int the_time){
  return update_conc_hist(the_time, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::update_conc_hist(int the_time, deque<mat_rsrc_ptr> mats){
  assert(last_degraded() <= the_time);

  IsoConcMap to_ret;

  pair<IsoVector, double> sum_pair; 
  sum_pair = dynamic_cast<NuclideModel*>(this)->vec_hist(the_time);

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

