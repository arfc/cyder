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
  n_(0),
  D_(0),
  rho_(0),
  Kd_(0),
  R_(0)
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
  n_(0),
  D_(0),
  rho_(0),
  Kd_(0),
  R_(0)
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

  // adv_vel.
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));

  // rock parameters
  n_ = lexical_cast<double>(qe->getElementContent("porosity"));
  D_ = lexical_cast<double>(qe->getElementContent("diffusion_coeff"));
  rho_ = lexical_cast<double>(qe->getElementContent("bulk_density"));
  Kd_ = lexical_cast<double>(qe->getElementContent("partition_coeff"));

  // retardation
  R_= 1 + (rho_*Kd_)/n_;

  LOG(LEV_DEBUG2,"GR1DNuc") << "The OneDimPPMNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr OneDimPPMNuclide::copy(NuclideModelPtr src){
  OneDimPPMNuclidePtr toRet = OneDimPPMNuclidePtr(new OneDimPPMNuclide());
  return (NuclideModelPtr)toRet;
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
