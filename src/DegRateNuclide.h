/*! \file DegRateNuclide.h
  \brief Declares the DegRateNuclide class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_DEGRATENUCLIDE_H)
#define _DEGRATENUCLIDE_H

#include <iostream>
#include "Logger.h"
#include <vector>
#include <map>
#include <string>

#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "NuclideModel.h"


/** 
   @brief DegRateNuclide is a nuclide model that releases material congruently
   with degradation of the engineered barrier material.
   
   This disposal system nuclide model will release any contained contaminants 
   at a rate corresponding solely to its degradation rate. That is, if the 
   component degrades at a rate of 15% per year, then 15% of the contaminant will
   be made available at the boundaries. This NuclideModel will follow J. Ahn's 
   congruent release model. 
   
   The DegRateNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, and Near Field. 
   However, since the Far Field and the Envrionment do not degrade, these are 
   not well represented by the DegRateNuclide model.
 */
class DegRateNuclide : public NuclideModel {
public:
  
  /**
     Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  DegRateNuclide(){ }; 

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  DegRateNuclide(xmlNodePtr cur){};

  /** 
     Default destructor does nothing.
   */
  ~DegRateNuclide() {};

  /**
     initializes the model parameters from an xmlNodePtr
     
     @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur); 

  /**
     copies a nuclide model and its parameters from another
     
     @param src is the nuclide model being copied
   */
  virtual NuclideModel* copy(NuclideModel* src); 

  /**
     standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
     Absorbs the contents of the given Material into this DegRateNuclide.
     
     @param matToAdd the DegRateNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this DegRateNuclide. Use this 
     function for decrementing a DegRateNuclide's mass balance after transferring 
     through a link. 
     
     @param matToRem the Material whose composition we want to decrement 
     against this DegRateNuclide
   */
  virtual void extract(mat_rsrc_ptr matToRem) ;

  /**
     Transports nuclides from the inner to the outer boundary 
   */
  virtual void transportNuclides();

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return DEGRATE_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "DEGRATE_NUCLIDE";};

  /**
     returns the peak Toxicity this object will experience during the 
     simulation.
     
     @return peak_tox_
   */
  const virtual Tox peak_tox(){};

  /**
     returns the concentration map for this component at the time specified
     
     @param time the time to query the concentration map
   */
  virtual ConcMap conc_map(int time){};

private:
  /**
    The degradation rate that defines this model, fraction per year.
   */
  double deg_rate_;
};
#endif
