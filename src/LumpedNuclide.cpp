/*! \file LumpedNuclide.cpp
    \brief Implements the LumpedNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "InputXML.h"
#include "Logger.h"
#include "Timer.h"
#include "LumpedNuclide.h"

using namespace std;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::LumpedNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LumpedNuclide::~LumpedNuclide(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedNuclide::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/LumpedNuclide");

  t_t_ = strtod(XMLinput->get_xpath_content(cur,"transit_time"),NULL);
  eta_ratio_=NULL;
  P_D_=NULL;

  formulation_ = XMLinput->get_xpath_name(cur,"formulation/*");
  string nodepath = "formulation/"+formulation_;
  xmlNodePtr ptr = XMLinput->get_xpath_element(cur,nodepath.c_str());
  if(formulation_=="EM"){
  } else if(formulation_=="PFM") {
  } else if(formulation_=="EPM") {
    eta_ratio_ = strtod(XMLinput->get_xpath_content(ptr,"eta_ratio"), NULL);
  } else if(formulation_=="DM") {
    P_D_ = strtod(XMLinput->get_xpath_content(ptr,"dispersion_coeff"),NULL);
  } else {
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
void LumpedNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void LumpedNuclide::extract(mat_rsrc_ptr matToRem)
{
  // Get the given LumpedNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRLNuc") << "LumpedNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void LumpedNuclide::transportNuclides(){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The LumpedNuclide class should transport all nuclides

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr LumpedNuclide::source_term_bc(){
  mat_rsrc_ptr m_ij = mat_rsrc_ptr(new Material());
  return m_ij;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder should call actual outer radius
  return conc_hist_.at(TI->time()).at(OUTER);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap  LumpedNuclide::neumann_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time()).at(OUTER);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoConcMap LumpedNuclide::cauchy_bc(){
  /// @TODO This is just a placeholder
  return conc_hist_.at(TI->time()).at(OUTER);
}
