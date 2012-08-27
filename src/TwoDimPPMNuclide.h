/*! \file TwoDimPPM.h
  \brief Declares the TwoDimPPM class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_TWODIMPPMNUCLIDE_H)
#define _TWODIMPPMNUCLIDE_H

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
   @brief TwoDimPPMNuclide is a nuclide transport model that treats the volume 
   as a homogeneously mixed cell.
   
   This disposal system nuclide model will receive material diffusively. That is, at
   the internal boundary, if the concentration from an adjacent cell is
   higher than the concentration within the mixed cell, this cell will accept the 
   appropriate flux accross that boundary. So too if the concentration is lower in the 
   adjacent cell, a corresponding contaminant flux will leave this cell accross that boundary. 
   
   When accepting a flux of Material, this cell immediately incorporates it into 
   the mixed volume. The mixed volume will then make the corresponding homogeneous 
   concentration available at all boundaries.  
   
   The TwoDimPPMNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class TwoDimPPMNuclide : public NuclideModel {
public:
  
  /**
     Default constructor for the mixed cell nuclide model class.
     Creates an empty mixed cell with contaminant concentration zero unless 
     an initial contaminant concentration is specified.
   */
  TwoDimPPMNuclide(); 

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  TwoDimPPMNuclide(xmlNodePtr cur){};

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~TwoDimPPMNuclide() ;

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
     Absorbs the contents of the given Material into this TwoDimPPMNuclide.
     
     @param matToAdd the TwoDimPPMNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this TwoDimPPMNuclide. Use this 
     function for decrementing a TwoDimPPMNuclide's mass balance after transferring 
     through a link. 
     
     @param matToRem the Material whose composition we want to decrement 
     against this TwoDimPPMNuclide
   */
  virtual void extract(mat_rsrc_ptr matToRem) ;

  /**
     Transports nuclides from the inner to the outer boundary 
   */
  virtual void transportNuclides();

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return TWODIMPPM_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "TWODIMPPM_NUCLIDE";};

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
    The advective velocity through the waste packages in units of m/s.
  */
  double v_;

  /// The initial contaminant concentration, C(x,0), in g/cm^3
  // @TODO make this an isovector, maybe with a recipe
  double Ci_;

  /// The contaminant concentration constantly at the source until t_0, in g/cm^3
  // @TODO make this an isovector, maybe with a recipe
  double Co_;

  /// Porosity of the component matrix, a fraction between 0 and 1, inclusive.
  double n_;

  /// Diffusion coefficient (which kind?) (what units?)
  double D_;

  /// The bulk (dry) density of the component matrix, in g/cm^3.
  double rho_;

  /**
    Partition coefficient, the ratio of absorbed adsorbate per mass 
    of solid to the amount of the adsorbate remaining in solution at equilibrium. 
   */
  double Kd_;

  /// Retardation coefficient, R = 1 + rho_*Kd_/n 
  double R_;

};


#endif
