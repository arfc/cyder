/*! \file DegRateNuclide.cpp
    \brief Implements the DegRateNuclide class used by the Generic Repository 
    \author Kathryn D. Huff
 */
#include <iostream>
#include "Logger.h"
#include "Timer.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "InputXML.h"
#include "DegRateNuclide.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DegRateNuclide::DegRateNuclide(){
  deg_rate_=0;
  contained_mass_ = vector<double>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/DegRateNuclide");
  double deg_rate = strtod(XMLinput->get_xpath_content(cur, "degradation"), NULL);
  init(deg_rate);
  LOG(LEV_DEBUG2,"GRDRNuc") << "The DegRateNuclide Class init(cur) function has been called";;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::init(double deg_rate) {
  deg_rate_ = deg_rate;
  if (deg_rate_ < 0 | deg_rate_ > 1) {
    string err = "Expected a fractional degradation rate. The value provided: ";
    err += deg_rate_ ;
    err += ", is not between 0 and 1 (inclusive).";
    LOG(LEV_ERROR,"GRDRNuc") << err ;;
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* DegRateNuclide::copy(NuclideModel* src){
  DegRateNuclide* toRet = new DegRateNuclide();
  deg_rate_ = dynamic_cast<DegRateNuclide*>(src)->deg_rate_;
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::print(){
    LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::absorb(mat_rsrc_ptr matToAdd)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void DegRateNuclide::extract(mat_rsrc_ptr matToRem)
{
  // Get the given DegRateNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRDRNuc") << "DegRateNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::transportNuclides(){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  //
  // Sometimes it's easier to use the dissolution rate as a source term in the 
  // transport equations
  //
  // Dissolution rate : m_dot[i,t] = C[i,t]*Q;
  // if S is the crossectional area perpendicular to water flow and v is water 
  // velocity : f[i,z,t] = C[i,t] * v * d(z)  = volumetric source term

}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void DegRateNuclide::set_deg_rate(double deg_rate){
  if( deg_rate < 0 || deg_rate > 1 ) {
    string msg = "The DegRateNuclide degradation rate range is 0 to 1, inclusive.";
    msg += " The value provided was ";
    msg += deg_rate;
    msg += ".";
    throw CycRangeException(msg);
  } else {
    deg_rate_ = deg_rate;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(int time){
  return contained_mass_.at(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::contained_mass(){
  return contained_mass(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
mat_rsrc_ptr DegRateNuclide::source_term_bc(){
  mat_rsrc_ptr m_ij = mat_rsrc_ptr(new Material());
  return m_ij;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::dirichlet_bc(){
  /// @TODO This is just a placeholder
  return contained_mass(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::neumann_bc(){
  /// @TODO This is just a placeholder
  return contained_mass(TI->time());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double DegRateNuclide::cauchy_bc(){
  /// @TODO This is just a placeholder
  return contained_mass(TI->time());
}
