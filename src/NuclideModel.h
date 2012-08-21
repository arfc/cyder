/*! \file NuclideModel.h
  \brief Declares the virtual NuclideModel class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_NUCLIDEMODEL_H)
#define _NUCLIDEMODEL_H

#include <vector>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"

/**
   enumerated list of types of nuclide transport model
 */
enum NuclideModelType { 
  DEGRATE_NUCLIDE, 
  LUMPED_NUCLIDE, 
  MIXEDCELL_NUCLIDE, 
  ONEDIMPPM_NUCLIDE, 
  STUB_NUCLIDE, 
  TWODIMPPM_NUCLIDE, 
  LAST_NUCLIDE};

/**
   type definition for Toxicity in units of Sv 
 */
typedef double Tox;

/**
   type definition for Concentrations in kg/m^3
 */
typedef double Concentration;

/**
   type definition for ConcMap 
 */
typedef std::map<int, Concentration> ConcMap;


/** 
   @brief Abstract interface for GenericRepository nuclide transport 
   
   NuclideModels such as MixedCell, OneDimPPM, etc., will share a 
   virtual interface defined here so that information passing concerning 
   hydrogeologic nuclide transport fluxes and other boundary conditions 
   can be conducted between components within the repository.
 */
class NuclideModel {

public:
  /**
     initializes the model parameters from an xmlNodePtr
     
     @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur)=0; 

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual NuclideModel* copy(NuclideModel* src)=0; 

  /**
     standard verbose printer explains the model
   */
  virtual void print()=0; 

  /**
     Absorbs the contents of the given Material into this NuclideModel.
     
     @param matToAdd the NuclideModel to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) = 0;

  /**
     Extracts the contents of the given Material from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param matToRem the Material whose composition we want to decrement 
     against this NuclideModel
   */
  virtual void extract(mat_rsrc_ptr matToRem) = 0 ;

  /**
     Transports nuclides from the inner boundary to the outer boundary in this 
     component
   */
  virtual void transportNuclides() = 0 ;

  /** 
     returns the NuclideModelType of the model
   */
  virtual NuclideModelType type() = 0;

  /** 
     returns the name of the NuclideModelType of the model
   */
  virtual std::string name() = 0;

  /**
     get the peak Toxicity this object will experience during the simulation
     
     @return peak_toxicity_
   */
  const virtual Tox peak_tox()=0;

  /**
     returns the concentration map for this component at the time specified
     
     @param time
   */
  virtual ConcMap conc_map(int time) = 0;

  /**
     returns the available material source term at the outer boundary of the 
     component
    
     @return m_ij the available source term outer boundary condition 
   */
  virtual mat_rsrc_ptr source_term_bc()=0;

  /**
     returns the prescribed concentration at the boundary, the dirichlet bc
     in kg/m^3
    
     @return C the concentration at the boundary in kg/m^3
   */
  virtual double dirichlet_bc() = 0;

  /**
     returns the concentration gradient at the boundary, the Neumann bc
    
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual double neumann_bc() = 0;

  /**
     returns the flux at the boundary, the Neumann bc
    
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual double cauchy_bc() = 0;

protected:
  /// The map of times to isotops to concentrations, in kg/m^3
  std::map<int, ConcMap> conc_hist_;

};

#endif
