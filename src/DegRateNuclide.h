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
  DegRateNuclide(); 

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  DegRateNuclide(xmlNodePtr cur){};

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~DegRateNuclide();

  /**
     initializes the model parameters from an xmlNodePtr
     
     @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur); 

  /**
     initializes the model parameters from an xmlNodePtr
     
     @param deg_rate the degradation rate, fraction per yr (a fraction 0-1)
   */
  virtual void init(double deg_rate); 

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
     returns the degradation rate that characterizes this model
   *
     @return deg_rate_ fraction per year
   */
  double deg_rate() {return deg_rate_;};

  /** 
     returns the degradation rate that characterizes this model
   *
     @param deg_rate fraction per timestep, between 0 and 1
     @throws CycRangeException if deg_rate not between 0 and 1 inclusive 
   */
  void set_deg_rate(double deg_rate);

  /** 
     returns the current contained contaminant mass, in kg
   *
     @return contained_mass_[now] throughout the component volume, in kg
   */
  double contained_mass();

  /** 
     returns the current contained contaminant mass, in kg, at time
   *
     @param time the time to query the contained contaminant mass
     @return contained_mass_ throughout the component volume, in kg, at time
   */
  double contained_mass(int time);

  /**
     returns the available material source term at the outer boundary of the 
     component
   *
     @return m_ij the available source term outer boundary condition 
   */
  virtual mat_rsrc_ptr source_term_bc();

  /**
     returns the prescribed concentration at the boundary, the dirichlet bc
     in kg/m^3
   *
     @return C the concentration at the boundary in kg/m^3 for each isotope
   */
  virtual IsoConcMap dirichlet_bc();

  /**
     returns the concentration gradient at the boundary, the Neumann bc
   *
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual IsoConcMap neumann_bc();

  /**
     returns the flux at the boundary, the Neumann bc
   *
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoConcMap cauchy_bc();

protected:
  /**
     sets the boundary conditions of the 0th through 3rd kind

     @param conc_map the current map of concentrations at the boundary
    */
  void set_bcs(int time, IsoConcMap conc_map);

  /// sets the boundary condition of the 0th kind
  void set_source_term_bc(int time, IsoConcMap conc_map);

  /// sets the boundary condition of the 1st kind
  void set_dirichlet_bc(int time, IsoConcMap conc_map);
  
  /// sets the boundary condition of the 2nd kind
  void set_neumann_bc(int time, IsoConcMap conc_map);
  
  /// sets the boundary condition of the 3rd kind
  void set_cauchy_bc(int time, IsoConcMap conc_map);

  /**
     updates the current conc_hist_

     @param time is the time at which to update the history
     @returns the IsoConcMap added to conc_hist_ at that time
    */
  IsoConcMap update_hist(int time);

  /**
     total contaminant mass, in kg, throughout the volume, for each timestep.
   */
  std::map<int,double> contained_mass_;

  /**
    The degradation rate that defines this model, fraction per year.
   */
  double deg_rate_;

  /// the available source term in isovector form 
  IsoVector avail_iso_vec_;

  /// the available source term amount
  double avail_kg_;


};
#endif
