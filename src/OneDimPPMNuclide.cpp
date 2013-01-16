/*! \file OneDimPPMNuclide.cpp
    \brief Implements the OneDimPPMNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <deque>
#include <time.h>
#include <boost/lexical_cast.hpp>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "OneDimPPMNuclide.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::OneDimPPMNuclide():
  Ci_(0),
  Co_(0),
  v_(0),
  porosity_(0),
  rho_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::OneDimPPMNuclide(QueryEngine* qe):
  Ci_(0),
  Co_(0),
  v_(0),
  porosity_(0),
  rho_(0)
{
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  this->initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
OneDimPPMNuclide::~OneDimPPMNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OneDimPPMNuclide::initModuleMembers(QueryEngine* qe){
  // C(x,0)=C_i
  Ci_ = lexical_cast<double>(qe->getElementContent("initial_concentration"));
  // -D{\frac{\partial C}{\partial x}}|_{x=0} + vC = vC_0, for t<t_0
  Co_ = lexical_cast<double>(qe->getElementContent("source_concentration"));

  // advective velocity (hopefully the same as the whole system).
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));

  // rock parameters
  porosity_ = lexical_cast<double>(qe->getElementContent("porosity"));
  rho_ = lexical_cast<double>(qe->getElementContent("bulk_density"));

  LOG(LEV_DEBUG2,"GR1DNuc") << "The OneDimPPMNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr OneDimPPMNuclide::copy(const NuclideModel& src){
  const OneDimPPMNuclide* src_ptr = dynamic_cast<const OneDimPPMNuclide*>(&src);

  set_last_updated(TI->time());
  set_porosity(src_ptr->porosity());
  set_rho(src_ptr->rho());
  set_v(src_ptr->v());
  set_Ci(src_ptr->Ci());
  set_Co(src_ptr->Co());


  // copy the geometry AND the centroid. It should be reset later.
  set_geom(geom_->copy(src_ptr->geom(), src_ptr->geom()->centroid()));

  wastes_ = deque<mat_rsrc_ptr>();
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  update_vec_hist(TI->time());

  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::print(){
    LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given OneDimPPMNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void OneDimPPMNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given OneDimPPMNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR1DNuc") << "OneDimPPMNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The OneDimPPMNuclide class should transport all nuclides

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> OneDimPPMNuclide::source_term_bc(){
  /// @TODO This is just a placeholder
  pair<IsoVector, double> to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap OneDimPPMNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap OneDimPPMNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap OneDimPPMNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::update_vec_hist(int the_time){
  vec_hist_[the_time]=MatTools::sum_mats(wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_porosity(double porosity){
  try { 
    MatTools::validate_percent(porosity);
  } catch (CycRangeException e) {
    stringstream msg_ss;
    msg_ss << "The OneDimPPMNuclide porosity range is 0 to 1, inclusive.";
    msg_ss << " The value provided was ";
    msg_ss << porosity;
    msg_ss << ".";
    LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  }
  porosity_ = porosity;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_rho(double rho){
  try { 
    MatTools::validate_finite_pos(rho);
  } catch (CycRangeException e) {
    stringstream msg_ss;
    msg_ss << "The OneDimPPMNuclide bulk density (rho) range must be positive and finite";
    msg_ss << " The value provided was ";
    msg_ss << rho;
    msg_ss << ".";
    LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
    throw CycRangeException(msg_ss.str());
  }
  rho_ = rho;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_Co(double Co){
  Co_=Co;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_Ci(double Ci){
  Ci_=Ci;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void OneDimPPMNuclide::set_v(double v){
  v_=v;
}


