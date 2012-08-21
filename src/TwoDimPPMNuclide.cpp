/*! \file TwoDimPPMNuclide.cpp
    \brief Implements the TwoDimPPMNuclide class used by the Generic Repository 
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
#include "TwoDimPPMNuclide.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TwoDimPPMNuclide::TwoDimPPMNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TwoDimPPMNuclide::~TwoDimPPMNuclide(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TwoDimPPMNuclide::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/TwoDimPPMNuclide");

  // v_ is the advective velocity of water through the packages
  v_ = strtod(XMLinput->get_xpath_content(cur,"advective_velocity"), NULL);
  // C(x,0)=C_i
  Ci_ = strtod(XMLinput->get_xpath_content(cur,"initial_concentration"), NULL); 
  // -D{\frac{\partial C}{\partial x}}|_{x=0} + vC = vC_0, for t<t_0
  Co_ = strtod(XMLinput->get_xpath_content(cur,"source_concentration"), NULL); 

  // rock parameters
  D_ = strtod(XMLinput->get_xpath_content(cur,"diffusion_coeff"), NULL);
  n_ = strtod(XMLinput->get_xpath_content(cur,"porosity"), NULL);
  rho_ = strtod(XMLinput->get_xpath_content(cur,"bulk_density"), NULL);
  Kd_ = strtod(XMLinput->get_xpath_content(cur,"partition_coeff"), NULL);

  // retardation
  R_= 1 + (rho_*Kd_)/n_;

  LOG(LEV_DEBUG2,"GR2DNuc") << "The TwoDimPPMNuclide Class init(cur) function has been called";;
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
void TwoDimPPMNuclide::extract(mat_rsrc_ptr matToRem)
{
  // Get the given TwoDimPPMNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GR2DNuc") << "TwoDimPPMNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void TwoDimPPMNuclide::transportNuclides(){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The TwoDimPPMNuclide class should transport all nuclides

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr TwoDimPPMNuclide::source_term_bc(){
  mat_rsrc_ptr m_ij = mat_rsrc_ptr(new Material());
  return m_ij;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double TwoDimPPMNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return conc_map(TI->time())[OUTER];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double TwoDimPPMNuclide::neumann_bc(){
  /// @TODO This is just a placeholder
  return conc_map(TI->time())[OUTER];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double TwoDimPPMNuclide::cauchy_bc(){
  /// @TODO This is just a placeholder
  return conc_map(TI->time())[OUTER];
}
