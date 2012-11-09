/*! \file LumpedNuclide.cpp
    \brief Implements the LumpedNuclide class used by the Generic Repository 
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
#include "LumpedNuclide.h"

using namespace std;
using boost::lexical_cast;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide() : 
  t_t_(0),
  eta_ratio_(0),
  P_D_(NULL),
  formulation_("")
{ 
  set_geom(GeometryPtr(new Geometry()));

  t_t_ = 0;
  v_ = 0;
  eta_ratio_ = NULL;
  P_D_ = NULL;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide(QueryEngine* qe):
  t_t_(0),
  eta_ratio_(0),
  P_D_(NULL),
  formulation_("")
{ 

  set_geom(GeometryPtr(new Geometry()));

  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
  this->initModuleMembers(qe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::~LumpedNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedNuclide::initModuleMembers(QueryEngine* qe){
  t_t_ = lexical_cast<double>(qe->getElementContent("transit_time"));
  v_ = lexical_cast<double>(qe->getElementContent("advective_velocity"));

  eta_ratio_=NULL;
  P_D_=NULL;

  list<string> choices;
  list<string>::iterator it;
  choices.push_back ("EM");
  choices.push_back ("PFM");
  choices.push_back ("EPM");
  choices.push_back ("DM");

  QueryEngine* formulation_qe = qe->queryElement("formulation");
  for( it=choices.begin(); it!=choices.end(); it++){
    if (formulation_qe->nElementsMatchingQuery(*it) > 0){
      formulation_=(*it);
    }
  }
  QueryEngine* ptr = formulation_qe->queryElement(formulation_);
  if(formulation_=="EPM") {
    eta_ratio_ = lexical_cast<double>(ptr->getElementContent("eta_ratio"));
  } else if(formulation_=="DM") {
    P_D_ = lexical_cast<double>(ptr->getElementContent("dispersion_coeff"));
  } else if( formulation_ != "EM" && formulation_ != "PFM"){
    string err = "The formulation type '"; 
    err += formulation_;
    err += "' is not supported.";
    throw CycException(err);
    LOG(LEV_ERROR,"GRLNuc") << err;
  }

  LOG(LEV_DEBUG2,"GRLNuc") << "The LumpedNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* LumpedNuclide::copy(NuclideModel* src){
  LumpedNuclide* toRet = new LumpedNuclide();
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::print(){
    LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::absorb(mat_rsrc_ptr matToAdd) {
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide is absorbing material: ";
  matToAdd->print();
  wastes_.push_back(matToAdd);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedNuclide::extract(const CompMapPtr comp_to_rem, double kg_to_rem) {
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide" << "is extracting composition: ";
  comp_to_rem->print() ;
  MatTools::extract(comp_to_rem, kg_to_rem, wastes_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::transportNuclides(int time){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The LumpedNuclide class should transport all nuclides

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
pair<IsoVector, double> LumpedNuclide::source_term_bc(){
  /// @TODO This is just a placeholder
  pair<IsoVector, double> to_ret;
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder should call actual outer radius
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ConcGradMap  LumpedNuclide::neumann_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoFluxMap LumpedNuclide::cauchy_bc(IsoConcMap c_ext, Radius r_ext){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time());
}
