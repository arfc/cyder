// MixedCellNuclide.cpp
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "InputXML.h"
#include "MixedCellNuclide.h"

using namespace std;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MixedCellNuclide::MixedCellNuclide(){ }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MixedCellNuclide::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/MixedCellNuclide");
  porosity_ = strtod(XMLinput->get_xpath_content(cur,"porosity"),NULL);
  deg_rate_ = strtod(XMLinput->get_xpath_content(cur,"degradation"),NULL);

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
void MixedCellNuclide::extract(mat_rsrc_ptr matToRem)
{
  // Get the given MixedCellNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2,"GRMCNuc") << "MixedCellNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::transportNuclides(){
  // This should transport the nuclides through the component.
  // It will likely rely on the internal flux and will produce an external flux. 
  // The convention will be that flux is positive in the radial direction
  // If these fluxes are negative, nuclides aphysically flow toward the waste package 
  // It will send the adjacent components information?
  // The MixedCellNuclide class should transport all nuclides

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MixedCellNuclide::mixCell(){}

