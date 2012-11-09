/*! \file TwoDimPPMNuclide.cpp
    \brief Implements the TwoDimPPMNuclide class used by the Generic Repository 
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
#include "TwoDimPPMNuclide.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TwoDimPPMNuclide::TwoDimPPMNuclide():
  Ci_(0),
  Co_(0),
  v_(0),
  D_(0),
  n_(0),
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
TwoDimPPMNuclide::TwoDimPPMNuclide(QueryEngine* qe):
  Ci_(0),
  Co_(0),
  v_(0),
  D_(0),
  n_(0),
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
TwoDimPPMNuclide::~TwoDimPPMNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TwoDimPPMNuclide::initModuleMembers(QueryEngine* qe){
  // C(x,0)=C_i
  Ci_ = lexical_cast<double>(qe->getElementContent("initial_concentration"));
  // -D{\frac{\partial C}{\partial x}}|_{x=0} + vC = vC_0, for t<t_0
  Co_ = lexical_cast<double>(qe->getElementContent("source_concentration"));
  // adv_vel
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));
  // rock parameters
  D_ = lexical_cast<double>(qe->getElementContent("diffusion_coeff"));
  n_ = lexical_cast<double>(qe->getElementContent("porosity"));
  rho_ = lexical_cast<double>(qe->getElementContent("bulk_density"));
  Kd_ = lexical_cast<double>(qe->getElementContent("partition_coeff"));

  // retardation
  R_= 1 + (rho_*Kd_)/n_;

  LOG(LEV_DEBUG2,"GR2DNuc") << "The TwoDimPPMNuclide Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* TwoDimPPMNuclide::copy(NuclideModel* src){
  TwoDimPPMNuclide* toRet = new TwoDimPPMNuclide();
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void TwoDimPPMNuclide::print(){
    LOG(LEV_DEBUG2,"GR2DNuc") << "TwoDimPPMNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void TwoDimPPMNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given TwoDimPPMNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR2DNuc") << "TwoDimPPMNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TwoDimPPMNuclide::extract(CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given TwoDimPPMNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR2DNuc") << "TwoDimPPMNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void TwoDimPPMNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The TwoDimPPMNuclide class should transport all nuclides

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> TwoDimPPMNuclide::source_term_bc(){
  /// @TODO This is just a placeholder
  pair<IsoVector, double> to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap TwoDimPPMNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap TwoDimPPMNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap TwoDimPPMNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}
