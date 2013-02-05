/*! \file StubNuclide.cpp
    \brief Implements the StubNuclide class that gives an example of a concrete NuclideModel 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "Logger.h"
#include "Timer.h"
#include "StubNuclide.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubNuclide::StubNuclide(){
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubNuclide::StubNuclide(QueryEngine* qe){
  wastes_ = deque<mat_rsrc_ptr>();
  set_geom(GeometryPtr(new Geometry()));
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  initModuleMembers(qe);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubNuclide::~StubNuclide(){};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::initModuleMembers(QueryEngine* qe){
  // for now, just say you've done it... 
  LOG(LEV_DEBUG2,"GRSNuc") << "The StubNuclide Class initModuleMembers(qe) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr StubNuclide::copy(const NuclideModel& src){
  StubNuclidePtr toRet = StubNuclidePtr(new StubNuclide());
  return (NuclideModelPtr)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::print(){
    LOG(LEV_DEBUG2,"GRSNuc") << "StubNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRSNuc") << "StubNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRSNuc") << "StubNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The StubNuclide class should transport all nuclides

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::vector<IsoVector> StubNuclide::update_wastes(int the_time, std::vector<NuclideModelPtr> daughters){
  std::vector<IsoVector> to_ret;
  std::vector<NuclideModelPtr>::iterator daughter;
  for( daughter = daughters.begin(); daughters.end(); ++daughter){
    to_ret.push_back(daughter->source_term_bc(the_time));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> StubNuclide::source_term_bc(){
  /// @TODO This is just a placeholder
  pair<IsoVector, double> to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap StubNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap StubNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap StubNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}
