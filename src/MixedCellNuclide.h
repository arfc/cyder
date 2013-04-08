/*! \file MixedCellNuclide.h
  \brief Declares the MixedCellNuclide class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_MIXEDCELLNUCLIDE_H)
#define _MIXEDCELLNUCLIDE_H

#include <iostream>
#include "Logger.h"
#include <deque>
#include <vector>
#include <map>
#include <string>

#include "NuclideModel.h"

/// A shared pointer for the MixedCellNuclide object
class MixedCellNuclide;
typedef boost::shared_ptr<MixedCellNuclide> MixedCellNuclidePtr;

/** 
   @brief MixedCellNuclide is a nuclide model that releases material congruently
   with degradation of the engineered barrier material.
   
   This disposal system nuclide model will release any contained contaminants 
   at a rate corresponding solely to its degradation rate. That is, if the 
   component degrades at a rate of 15% per year, then 15% of the contaminant will
   be made available at the boundaries. This NuclideModel will follow J. Ahn's 
   congruent release model. 
   
   The MixedCellNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, and Near Field. 
   However, since the Far Field and the Envrionment do not degrade, these are 
   not well represented by the MixedCellNuclide model.
 */
class MixedCellNuclide : public NuclideModel {
  /*----------------------------*/
  /* All NuclideModel classes   */
  /* have the following members */
  /*----------------------------*/
private:
  
  /**
     Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  MixedCellNuclide(); 

  /**
     primary constructor reads input from the QueryEngine
     
     @param qe is the QueryEngine object containing intialization info
   */
  MixedCellNuclide(QueryEngine* qe);

public:

  /**
     A constructor for the Mixed Cell Nuclide Model that returns a shared pointer.
    */
  static MixedCellNuclidePtr create (){ return MixedCellNuclidePtr(new MixedCellNuclide()); };

  /**
     A constructor for the Mixed Cell Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static MixedCellNuclidePtr create (QueryEngine* qe){ return MixedCellNuclidePtr(new MixedCellNuclide(qe)); };

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~MixedCellNuclide();

  /**
     initializes the model parameters from a QueryEngine object
     
     @param qe is the QueryEngine object containing intialization info
   */
  virtual void initModuleMembers(QueryEngine* qe); 

  /**
     copies a nuclide model and its parameters from another
     
     @param src is the nuclide model being copied
   */
  virtual NuclideModelPtr copy(const NuclideModel& src); 

  /**
     standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
     Absorbs the contents of the given Material into this MixedCellNuclide.
     
     @param matToAdd the MixedCellNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this MixedCellNuclide. Use this 
     function for decrementing a MixedCellNuclide's mass balance after transferring 
     through a link. 

     @param comp_to_rem the composition to decrement against this MixedCellNuclide
     @param kg_to_rem the amount in kg to decrement against this MixedCellNuclide

     @return the material extracted
   */
  virtual mat_rsrc_ptr extract(CompMapPtr comp_to_rem, double kg_to_rem );

  /**
     Transports nuclides from the inner to the outer boundary 

     @param time the timestep at which to transport the nuclides
   */
  virtual void transportNuclides(int time);

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return MIXEDCELL_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "MIXEDCELL_NUCLIDE";};

  /**
     Updates all the hists

     @param the_time the time at which to update the history
   */
  virtual void update(int the_time);

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
  virtual ConcGradMap neumann_bc(IsoConcMap c_ext, Radius r_ext);

  /**
     returns the flux at the boundary, the Neumann bc
   *
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoFluxMap cauchy_bc(IsoConcMap c_ext, Radius r_ext);

  /**
     Updates the NuclideParams table by adding appropriate rows to describe the 
     parameters initializing this NuclideModel.
     */
  virtual void updateNuclideParamsTable();


  /*----------------------------*/
  /* This NuclideModel class    */
  /* has the following members  */
  /*----------------------------*/
public:
  /** 
     returns the degradation rate that characterizes this model
   *
     @return deg_rate_ fraction per year
   */
  const double deg_rate() const {return deg_rate_;};

  /** 
     returns the degradation rate that characterizes this model
   *
     @param cur_rate the current degradation rate, a fraction per timestep, between 0 and 1
     @throws CycRangeException if deg_rate not between 0 and 1 inclusive 
   */
  void set_deg_rate(double cur_rate);

  /** 
     returns the current contained contaminant mass, in kg
     @TODO should put this in the NuclideModel class or something. 

     @return contained_mass_[now] throughout the component volume, in kg
   */
  double contained_mass();

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
     @param cur_rate is the degradation rate since the last degradation, fraction. 
    */
  double update_degradation(int time, double cur_rate);

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
     Determines what IsoVector to remove from the daughter nuclide models

     @param time the timestep at which the nuclides should be transported
     @param daughter nuclide_model of an internal component. there may be many.
     
    */
  void update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters); 

  /**
     Update the isotopic vector history to incorporate sorption

     @param time the time at which to update the vector history
     @param iso the isotope to update
     @param mass the total mass of that isotope in the component

     @return the sorption limited mass
     */
  double sorb(int time, int iso, double mass);

  /**
     Update the isotopic vector history to incorporate solubility limitation

     @param time the time at which to update the vector history
     @param iso the isotope to update
     @param mass the total mass of that isotope in the component

     @return the solubility limited mass
     */
  double precipitate(int time, int iso, double mass);

  /// returns the total degradation of the component
  const double tot_deg() const {return tot_deg_;};

  /// sets the total degradation of the component
  void set_tot_deg(double tot_deg){tot_deg_=tot_deg;};

  /**
    Set the porosity (a fraction) of the material of this component. [%] 
   */
  void set_porosity(double porosity);

  /**
    The porosity (a fraction) of the material of this component. [%] 
   */
  const double porosity() const {return porosity_;};

  /**
    Set the advective velocity v_ through this component. [m/s] 
   */
  void set_v(double v){v_ = v;};

  /**
    The advective velocity through this component. [m/s] 
   */
  const double v() const {return v_;};

  /**
    Set the last_degraded_ time [integer timestamp]
   */
  void set_last_degraded(int last_degraded){last_degraded_ = last_degraded;};

  /**
    Returns the last timestamp at which this component was last degraded [integer timestamp]
   */
  const int last_degraded() const {return last_degraded_;};

  /// Sets boolean indicating whether to incorporate solubility limits
  void set_sol_limited(bool sol_limited){sol_limited_=sol_limited;}; 

  /// Gets boolean indicating whether to incorporate solubility limits
  const bool sol_limited() const {return sol_limited_;};

  /// Sets boolean indicating whether to incorporate sorption
  void set_kd_limited(bool kd_limited){kd_limited_=kd_limited;}; 

  /// Gets boolean indicating whether to incorporate sorption
  const bool kd_limited() const {return kd_limited_;};

  /// Gets the total volume
  double V_T();

  /// Gets the fluid volume, based on porosity
  double V_f();

  /// Gets the fluid volume, based on porosity
  double V_s();

  /// Gets the free fluid volume, based on porosity and degradation rate
  virtual double V_ff();

protected:
  /**
    The advective velocity through this component [m/s]
   */
  double v_;

  /**
    The degradation rate that defines this model, fraction per year.
   */
  double deg_rate_;

  /// the total fraction that this component has degraded
  double tot_deg_;

  /// the last timestamp at which this component was last degraded [integer timestamp]
  int last_degraded_;

  /// The porosity of the material in the component, a fraction [%] 
  double porosity_;

  /// Boolean indicates whether to incorporate solubility limits. (True = yes )
  bool sol_limited_;

  /// Boolean indicates whether to incorporate sorption. (True = yes )
  bool kd_limited_;

};
#endif
