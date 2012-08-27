/*! \file StubNuclide.h
    \brief Declares the StubNuclide class that gives an example of a concrete NuclideModel. 
    \author Kathryn D. Huff
 */
#if !defined(_STUBNUCLIDE_H)
#define _STUBNUCLIDE_H

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
   @brief StubNuclide is a skeleton nuclide model model that does nothing.
   
   This disposal system nuclide model will do nothing. This NuclideModel is 
   intended as a skeleton to guide the implementation of new NuclideModels. 
   
   The StubNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class StubNuclide : public NuclideModel {
public:
  
  /**
     Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  StubNuclide(); 

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  StubNuclide(xmlNodePtr cur){};

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~StubNuclide();

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
     Absorbs the contents of the given Material into this StubNuclide.
     
     @param matToAdd the StubNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this StubNuclide. Use this 
     function for decrementing a StubNuclide's mass balance after transferring 
     through a link. 
     
     @param matToRem the Material whose composition we want to decrement 
     against this StubNuclide
   */
  virtual void extract(mat_rsrc_ptr matToRem) ;

  /**
     Transports nuclides from the inner to the outer boundary 
   */
  virtual void transportNuclides();

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return STUB_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "STUB_NUCLIDE";};

  /**
     returns the peak Toxicity this object will experience during the 
     simulation.
     
     @return peak_toxicity
     @TODO issue #36
   */
  const virtual Tox peak_tox(){return NULL;};

  /**
     returns the concentration map for this component at the time specified
     
     @param time the time to query the concentration map
     @TODO issue #36
   */
  virtual ConcProfile conc_profile(int time){return conc_hist_.at(time);};

  /**
     returns the available material source term at the outer boundary of the 
     component
   
     @return m_ij the available source term outer boundary condition 
   */
  virtual mat_rsrc_ptr source_term_bc();

  /**
     returns the prescribed concentration at the boundary, the dirichlet bc
     in kg/m^3
   
     @return C the concentration at the boundary in kg/m^3
   */
  virtual IsoConcMap dirichlet_bc();

  /**
     returns the concentration gradient at the boundary, the Neumann bc
   
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual IsoConcMap neumann_bc();

  /**
     returns the flux at the boundary, the Neumann bc
   
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoConcMap cauchy_bc();

};
#endif
