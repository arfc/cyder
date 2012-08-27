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
#include "Geometry.h"

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
   type definition for radiotoxicity in Sv
  */
typedef double Tox;

/**
   type definition for Concentrations in kg/m^3
 */
typedef double Concentration;

/**
   type definition for a map from isotopes to concentrations
   The keys are the isotope identifiers Z*1000 + A
   The values are the Concentrations of each isotope
  */
typedef std::map<int, Concentration> IsoConcMap;

/**
   type definition for a map from radial positions to IsoConcMaps 
   The keys are the radial positions in the component
   The values are the IsoConcMap maps that exist at that position
 */
typedef std::map<double, IsoConcMap> ConcProfile;

/** 
   type definition fro a map from times to ConcProfiles
   The keys are timesteps, in the unit of the timesteps in the simulation.
   The values are the ConcProfile maps at those timesteps
  */
typedef std::map<int, ConcProfile> ConcHist;

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
     A virtual destructor
    */
  virtual ~NuclideModel() {};

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
     returns the concentration map for this component at the time specified
     
     @param time
   */
  virtual ConcProfile conc_profile(int time) = 0;

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
  virtual IsoConcMap dirichlet_bc() = 0;
  Concentration dirichlet_bc(Iso tope) {return this->dirichlet_bc().at(tope);};
  
  /**
     returns the concentration gradient at the boundary, the Neumann bc
    
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual IsoConcMap neumann_bc() = 0;
  Concentration neumann_bc(Iso tope) {return this->neumann_bc().at(tope);}; 

  /**
     returns the flux at the boundary, the Neumann bc
    
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoConcMap cauchy_bc() = 0;
  Concentration cauchy_bc(Iso tope) {return this->cauchy_bc().at(tope);};

protected:
  /// The map of times to isotopes to concentrations, in kg/m^3
  ConcHist conc_hist_;


  /// The vector of wastes contained by this component
  std::vector<mat_rsrc_ptr> wastes_;

};
#endif
