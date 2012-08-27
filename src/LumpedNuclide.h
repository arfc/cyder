/*! \file LumpedNuclide.h
  \brief Declares the LumpedNuclide class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_LUMPEDNUCLIDE_H)
#define _LUMPEDNUCLIDE_H

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
   @brief LumpedNuclide is a nuclide transport model that treats the volume 
   as a lumped parameter type nuclide transport model. This method is experimental, 
   and was not proposed in my prelim. This model may be influence by a paper by 
   Zuber and Maloszewski, as well as the inverse problem of that discussed in :

   Zuber, A., Maloszewski, P., 2001. Lumped-parameter models. Environmental Isotopes 
   in the Hydrological Cycle. Vol. 6: Modelling, UNESCO, Paris, Tech. Doc. in Hydrology 
   39, 5–35. 

   and :

   "Lumped Parameter models as a tool for determining the hydrological parameters of some 
   groundwater systems based on isotope data." Tracers and Modelling in Hydrogeology. 
   (Proceedings of the TraM 2000 Conference in Liege Belgium, May 2000). IAH Publ. no 262, 
   2000.
   
   This disposal system nuclide model will receive material blindly  at both borders.
   Received material will be incorporated and transferred to the opposite boundary according
   to lumped conductivity parameters.

   The parameters may be dependent on current contaiminant concentration.
   
   The LumpedNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class LumpedNuclide : public NuclideModel {
public:
  
  /**
     Default constructor for the lumped parameter nuclide model class.
     Creates an empty lumped parameter volume with contaminant concentration 
     zero unless an initial contaminant concentration is specified.
   */
  LumpedNuclide(); 

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  LumpedNuclide(xmlNodePtr cur){};

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~LumpedNuclide();

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
     Absorbs the contents of the given Material into this LumpedNuclide
     model.
     
     @param matToAdd the LumpedNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this LumpedNuclide model. 
     Use this function for decrementing a LumpedNuclide's mass balance after 
     transferring through a link. 
     
     @param matToRem the Material whose composition we want to decrement 
     against this LumpedNuclide
   */
  virtual void extract(mat_rsrc_ptr matToRem) ;

  /**
     Transports nuclides from the inner to the outer boundary 
   */
  virtual void transportNuclides();

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return LUMPED_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "LUMPED_NUCLIDE";};

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
  virtual ConcMap conc_map(int time){return conc_hist_.at(time);};

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
  virtual double dirichlet_bc();

  /**
     returns the concentration gradient at the boundary, the Neumann bc
   
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual double neumann_bc();

  /**
     returns the flux at the boundary, the Neumann bc
   
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual double cauchy_bc();

private:
  /**
   * The name of the lumped parameter model formulation. This can be 
   * the Exponential Model (EM), Piston Flow Model (PFM), Combined Exponential and 
   * Piston Flow Model (EPM), or the Dispersion Model(DM).
   */
  std::string formulation_;

  /// The transit time of a radioactive tracer through the cell
  double t_t_;
  
  /**
   * In the EPM model, the ratio of the total volume to the volue with the exponential 
   * distribution of transit times.
   */
  double eta_ratio_;

  /// The dispersion parameter (the inverse of the Peclet number).
  double P_D_;

};


#endif
