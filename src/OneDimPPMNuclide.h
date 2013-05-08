/*! \file OneDimPPMNuclide.h
    \brief Declares the OneDimPPM class, a one dimensional permeable porous 
    medium nuclide transport model. 
    \author Kathryn D. Huff
 */
#if !defined(_ONEDIMPPMNUCLIDE_H)
#define _ONEDIMPPMNUCLIDE_H

#include <iostream>
#include "Logger.h"
#include <vector>
#include <map>
#include <string>

#include "NuclideModel.h"

/// A shared pointer for the OneDimPPMNuclide object
class OneDimPPMNuclide;
typedef boost::shared_ptr<OneDimPPMNuclide> OneDimPPMNuclidePtr;


/** 
   @brief OneDimPPMNuclide is a nuclide transport model that treats the volume
   as a one dimensional system with unidirectional flow and a semi-infinite
   boundary condition in the positive flow direction. 

   The solution is given in :
   F. J. Leij, T. H. Skaggs, and M. T. Van Genuchten, ``Analytical solutions for
   solute transport in three-dimensional semi-infinite porous media,'' Water
   resources research, vol. 27, no. 10, p. 27192733, 1991. 
   
   This disposal system nuclide model will receive material both diffusively and 
   advectively. That is, at the internal boundary a Cauchy type boundary 
   condition is applied. 
   
    For the boundary and initial conditions, 
    \f{eqnarray*}{
      -D \frac{\partial C}{\partial z}\big|_{z=0} + v_zc &=& v_zC_0\mbox{ when }\left( 0<t<t_0 \right)\\
      -D \frac{\partial C}{\partial z}\big|_{z=0} + v_zc &=& 0\mbox{ when }\left( t>t_0 \right)\\
      \frac{\partial C}{\partial z}\big|_{z=\infty} &=& 0\\
      C(z,0) &=& C_i,
      \label{1dinfBC}
    \f}
      
   the solution is given as 

   \f{eqnarray*}{
      C(z,r,t) = \frac{C_0}{2}\Bigg[&\mbox{erfc}\left[{\frac{r-v_zt}{2\sqrt{D_Lt}}}\right] 
      + \frac{1}{2} \left(\frac{v_z^2t}{\pi D_L}\right)^{1/2}e^{\frac{-( r - 
      v_zt)^2}{4D_Lt}}\nonumber\\
      &- \frac{1}{2}\left( 
      1+\frac{v_zr}{D_L}+\frac{v_z^2t}{D_L}\right)e^\frac{v_zr}{D_L}\mbox{erfc}\left[\frac{r-V_zt}{2\sqrt{D_Lt}}\right]
      \Bigg].
    \f}

   If the concentration from an adjacent cell is higher 
   than the concentration within the mixed cell, this cell will accept the 
   appropriate diffusive flux accross that boundary. So too if the concentration 
   is lower in the adjacent cell, a corresponding contaminant flux will leave 
   this cell across that boundary. Similarly, the advective velocity 
   (unidirectional) at the internal boundary of this cell will cause material to 
   enter the cell advectively. 

   When accepting a flux of Material, this cell incorporates it into the list of 
   wastes that it contains.. The volume will then make the corresponding 
   boundary conditions available at all boundaries.  
   
   The OneDimPPMNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field, 
   Far Field, and Envrionment. However, is is best used to represent either the 
   Buffer or the Environment components, due to its long running time and lack 
   of degradation.
   */
class OneDimPPMNuclide : public NuclideModel {
private:
  
  /**
     Default constructor for the mixed cell nuclide model class.
     Creates an empty mixed cell with contaminant concentration zero unless 
     an initial contaminant concentration is specified.
   */
  OneDimPPMNuclide(); 

  /**
     primary constructor reads input from QueryEngine
     
     @param qe is the QueryEngine object containing intialization info
   */
  OneDimPPMNuclide(QueryEngine* qe);

public:

  /**
     A constructor for the OneDimPPM Nuclide Model that returns a shared pointer.
    */
  static OneDimPPMNuclidePtr create (){ return OneDimPPMNuclidePtr(new OneDimPPMNuclide()); };

  /**
     A constructor for the OneDimPPM Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static OneDimPPMNuclidePtr create (QueryEngine* qe){ return OneDimPPMNuclidePtr(new OneDimPPMNuclide(qe)); };

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~OneDimPPMNuclide();

  /**
     initializes the model parameters from an xmlNodePtr
     
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
     Absorbs the contents of the given Material into this OneDimPPMNuclide.
     
     @param matToAdd the OneDimPPMNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this OneDimPPMNuclide
     @param comp_to_rem the mass in kg to decrement against this OneDimPPMNuclide

     @return the material extracted
   */
  virtual mat_rsrc_ptr extract(CompMapPtr comp_to_rem, double kg_to_rem);


  /**
     Transports nuclides from the inner to the outer boundary 

     @param time the timestep at which to transport the nuclides
   */
  virtual void transportNuclides(int time);

  /** 
     Determines what IsoVector to remove from the daughter nuclide models

     @param time the timestep at which the nuclides should be transported
     @param daughter nuclide_model of an internal component. there may be many.
     */
  void update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters); 

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return ONEDIMPPM_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "ONEDIMPPM_NUCLIDE";};

  /**
     Updates all the hists

     @param the_time the time at which to update the history
   */
  virtual void update(int the_time);

  /**
     returns the available material source term at the outer boundary of the 
     component
   
     @return the IsoVector and mass defining the outer boundary condition 
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
     update the istopic contaminant vector history, vec_hist_ based on wastes_

     @param the_time the time at which to update the vec_hist_ [timestep]
    */
  void update_vec_hist(int the_time);

  /**
     return porosity
    */
  const double porosity() const {return porosity_;};

  /** 
     Updates the available concentration using the wastes_ as mats

     @param the_time the time at which to update the IsoConcMap
    */
  void update_conc_hist(int the_time);

  /** 
     Updates the available concentration

     @param the_time the time at which to update the IsoConcMap
     @param mats the materials that are part of the available concentration
    */
  void update_conc_hist(int the_time, std::deque<mat_rsrc_ptr> mats);

  /**
     Calculates the concentration of each isotope a certain time and radius.
     
     @param C_0 the initial concentration at that radius?
     @param r_calc the radius at which to calculate the IsoConcMap [m]
     @param dt the change in time since C_0 was calculated [timestep]
    */
  IsoConcMap conc_profile(IsoConcMap C_0, double r_calc, int dt);


  /**
     Calculates the concentration of a single isotope due to C_0 after dt at 
     radius r_calc


     /// describe van genuchten et. al eqn. here... 
     
     @param C_0 the initial concentration at that radius?
     @param r_calc the radius at which to calculate the IsoConcMap [m]
     @param iso the isotope whose concentration is being queried [-]
     @param dt the change in time since C_0 was calculated [timestep]
    */
  double calculate_conc(IsoConcMap C_0, IsoConcMap C_i, double r_calc, int iso, int t0, int t);

  /**
     Calculates the concentration of an isoconcmap due to C_0 after dt at 
     radius r_calc


     /// describe van genuchten et. al eqn. here... 
     
     @param C_0 the initial concentration at that radius?
     @param r_calc the radius at which to calculate the IsoConcMap [m]
     @param dt the change in time since C_0 was calculated [timestep]
    */
  IsoConcMap calculate_conc(IsoConcMap C_0, IsoConcMap C_i, double r_calc, int t0, int t);

  /// @TODO describe
  IsoConcMap trap_rule(double a, double b, int n, std::map<double, IsoConcMap> fmap);

  /// sets the porosity_ variable, the percent void of the medium 
  void set_porosity(double porosity);

  /**
     return initial concentration in the cell for this timestep
    */
  IsoConcMap Ci() ;

  /// sets the Ci_ variable, the initial concentration.
  void set_Ci(IsoConcMap Ci);

  /**
     return Co, the source concentration at the inner boundary
    */
  IsoConcMap Co(const NuclideModelPtr& daughter);

  /**
     return bulk density
    */
  const double rho() const {return rho_;};

  /// sets the rho_ variable, the dry bulk density of the medium [kg/m^3] 
  void set_rho(double rho);

  /**
    The advective velocity through this component. [m/s] 
   */
  const double v() const {return v_;};

  /// sets the v_ variable, the advective velocity through this component. 
  void set_v(double v);

  /// Gets the total fluid volume
  double V_T();

  /// Gets the fluid volume, based on porosity
  double V_f();

  /// Gets the fluid volume, based on porosity
  virtual double V_ff(){return V_f();};

  /**
     Updates the NuclideParams table by adding appropriate rows to describe the 
     parameters initializing this NuclideModel.
     */
  virtual void updateNuclideParamsTable();


protected:
  /**
    The advective velocity through the waste packages in units of m/s.
  */
  double v_;

  /// The initial contaminant concentration, C(x,0), in g/cm^3
  IsoConcMap Ci_;

  /// The contaminant concentration constantly at the source until t_0, in g/cm^3
  IsoConcMap Co_;

  /// Porosity of the component matrix, a fraction between 0 and 1, inclusive.
  double porosity_;

  /// The bulk (dry) density of the component matrix, in g/cm^3.
  double rho_;

};


#endif
