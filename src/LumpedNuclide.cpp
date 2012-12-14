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
  P_D_(0),
  formulation_(LAST_FORMULATION_TYPE)
{ 
  set_geom(GeometryPtr(new Geometry()));

  t_t_ = 0;
  v_ = 0;
  eta_ratio_ = 0;
  P_D_ = 0;
  vec_hist_ = VecHist();
  conc_hist_ = ConcHist();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide(QueryEngine* qe):
  t_t_(0),
  eta_ratio_(0),
  P_D_(0),
  formulation_(LAST_FORMULATION_TYPE)
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
  choices.push_back ("DM");
  choices.push_back ("EM");
  choices.push_back ("EPM");
  choices.push_back ("PFM");

  QueryEngine* formulation_qe = qe->queryElement("formulation");
  string formulation_string;
  for( it=choices.begin(); it!=choices.end(); it++){
    if (formulation_qe->nElementsMatchingQuery(*it) == 1){
      formulation_=enumerateFormulation(*it);
      formulation_string=(*it);
    }
  }
  QueryEngine* ptr = formulation_qe->queryElement(formulation_string);
  switch(formulation_){
    case DM :
      P_D_ = lexical_cast<double>(ptr->getElementContent("dispersion_coeff"));
      break;
    case EM :
      break;
    case EPM :
      eta_ratio_ = lexical_cast<double>(ptr->getElementContent("eta_ratio"));
      break;
    case PFM :
      break;
    default:
      string err = "The formulation type '"; 
      err += formulation_;
      err += "' is not supported.";
      throw CycException(err);
      LOG(LEV_ERROR,"GRLNuc") << err;
      break;
  }

  LOG(LEV_DEBUG2,"GRLNuc") << "The LumpedNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelPtr LumpedNuclide::copy(const NuclideModel& src){
  LumpedNuclidePtr toRet = LumpedNuclidePtr( new LumpedNuclide() );
  return (NuclideModelPtr)toRet;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
FormulationType LumpedNuclide::enumerateFormulation(string type_name) {
  FormulationType toRet = LAST_FORMULATION_TYPE;
  string formulation_type_names[] = {"DM", "EM", "EPM", "PFM", "LAST_FORMULATION_TYPE"};
  for(int type = 0; type < LAST_FORMULATION_TYPE; type++){
    if(formulation_type_names[type] == type_name){
      toRet = (FormulationType)type;
    } 
  }
  if (toRet == LAST_FORMULATION_TYPE){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid FormulationType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_FORMULATION_TYPE; name++){
      err_msg += formulation_type_names[name];
      err_msg += "\n";
    }
    throw CycException(err_msg);
  }
  return toRet;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_f(){
  return MatTools::V_f(V_T(),porosity());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_s(){
  return MatTools::V_s(V_T(),porosity());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double LumpedNuclide::V_T(){
  return geom_->volume();
}

