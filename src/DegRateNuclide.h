/*! \file DegRateNuclide.h
  \brief Declares the DegRateNuclide class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_DEGRATENUCLIDE_H)
#define _DEGRATENUCLIDE_H

#include <iostream>
#include "Logger.h"
#include <deque>
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
  /*----------------------------*/
  /* All NuclideModel classes   */
  /* have the following members */
  /*----------------------------*/
public:
  
  /**
     Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  DegRateNuclide(); 

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~DegRateNuclide();

  /**
     primary constructor reads input from XML node
     
     @param cur input XML node
   */
  DegRateNuclide(xmlNodePtr cur){};

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

     @param comp_to_rem the composition to decrement against this DegRateNuclide
     @param kg_to_rem the amount in kg to decrement against this DegRateNuclide
   */
  virtual void extract(CompMapPtr comp_to_rem, double kg_to_rem );

  /**
     Transports nuclides from the inner to the outer boundary 

     @param time the timestep at which to transport the nuclides
   */
  virtual void transportNuclides(int time);

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return DEGRATE_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "DEGRATE_NUCLIDE";};

  /**
     returns the available material source term at the outer boundary of the 
     component
   *
     @return m_ij the available source term outer boundary condition 
   */
  virtual std::pair<IsoVector, double> source_term_bc();

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
  virtual ConcGradMap neumann_bc(ConcGrad c_ext, Radius r_ext);

  /**
     returns the flux at the boundary, the Neumann bc
   *
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoConcMap cauchy_bc();

  /*----------------------------*/
  /* This NuclideModel class    */
  /* has the following members  */
  /*----------------------------*/
public:
  /**
     initializes the model parameters from an xmlNodePtr
     
     @param deg_rate the degradation rate, fraction per yr (a fraction 0-1)
   */
  virtual void init(double deg_rate); 


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
     @TODO should put this in the NuclideModel class or something. 

     @return contained_mass_[now] throughout the component volume, in kg
   */
  double contained_mass();

  /** 
     returns the current contained contaminant mass, in kg, at time
     @TODO should put this in the NuclideModel class or something. 

     @param time the time to query the contained contaminant mass
     @return contained_mass_ throughout the component volume, in kg, at time
   */
  double contained_mass(int time);

  /**
     updates the contained concentration according to contained wastes_
    
     @param time the time at which to update the degradation
     @return the current isotopic concentration map at the outer border
    */
  IsoConcMap update_conc_hist(int time);

  /**
     updates the contained concentration with mats provided 
    
     @param time the time at which to update the degradation
     @param mats the vector of materials contained in the component
     @return the current isotopic concentration map at the outer border
    */
  IsoConcMap update_conc_hist(int time, std::deque<mat_rsrc_ptr> mats);

  /**
     updates the total degradation and makes time the last degraded time.
    
     @param time the time at which to update the degradation
     @param deg_rate is the degradation rate since the last degradation, fraction. 
    */
  double update_degradation(int time, double deg_rate);

  /** 
     The IsoVector representing the summed, normalized material in 
     the component.
     @TODO should put this in a toolkit or something. Doesn't belong here.

     @param time the time at which to retrieve the IsoVector

     @return vec_hist(time).first
    */
  IsoVector contained_vec(int time);

  /**
     Updates the isotopic vector history at the time

     @param time the time at which to update the vector history, according to wastes_
     last_degraded_ time.
     */
  void update_vec_hist(int time);

  /**
     Updates the isotopic vector history at the time

     @param time the time at which to update the vector history
     @param mats the deque of materials to include in the history, usually wastes_
     @throws an exception if the time provided is less than the 
     last_degraded_ time.
     */
  void update_vec_hist(int time, std::deque<mat_rsrc_ptr> mats);

  /**
     Returns the map of isotopes to concentrations at the time profided
     @TODO should put this in a toolkit or something. Doesn't belong here.

     @param time the time at which to query the concentration history
     @return conc_hist_.at(time). If not found an empty IsoConcMap is return
    */
  IsoConcMap conc_hist(int time);

  /**
     Returns the concetration of a certain isotope at a certain time

     @param time the time to query the concentration history
     @param tope the isotope to query
     @return conc_hist(time)[iso].second, or zero if not found
     */
  Concentration conc_hist(int time, Iso tope);

  /**
     Returns the IsoVector mass pair for a certain time

     @param time the time to query the isotopic history
     @return vec_hist_.at(time). If not found an empty pair is returned.
     */
  std::pair<IsoVector, double> vec_hist(int time);

  /// returns the total degradation of the component
  double tot_deg();


protected:
  /**
    The degradation rate that defines this model, fraction per year.
   */
  double deg_rate_;

  /// the time this nuclide model was initialized, the beginning of degradation
  double init_time_;

  /// the total fraction that this component has degraded
  double tot_deg_;

  int last_degraded_;


};
#endif
