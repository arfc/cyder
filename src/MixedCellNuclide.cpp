/*! \file MixedCellNuclide.cpp
    \brief Implements the MixedCellNuclide class used by the Generic Repository 
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
#include "MixedCellNuclide.h"

using namespace std;
using boost::lexical_cast;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide():
  deg_rate_(0),
  v_(0)
{ 
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide(QueryEngine* qe):
  deg_rate_(0),
  v_(0)
{ 
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  this->initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::~MixedCellNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::initModuleMembers(QueryEngine* qe){
  // move the xml pointer to the current model
  set_porosity(lexical_cast<double>(qe->getElementContent("porosity")));
  set_deg_rate(lexical_cast<double>(qe->getElementContent("degradation")));
  set_v(lexical_cast<double>(qe->getElementContent("advective_velocity")));

  LOG(LEV_DEBUG2,"GRMCNuc") << "The MixedCellNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* MixedCellNuclide::copy(NuclideModel* src){
  MixedCellNuclide* toRet = new MixedCellNuclide();
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::print(){
    LOG(LEV_DEBUG2,"GRMCNuc") << "MixedCellNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRMCNuc") << "MixedCellNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRMCNuc") << "MixedCellNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The MixedCellNuclide class should transport all nuclides
  mixCell();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::mixCell(){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> MixedCellNuclide::source_term_bc(){
  /// @TODO This is just a placeholder
  pair<IsoVector, double> to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap MixedCellNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap MixedCellNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap MixedCellNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}
