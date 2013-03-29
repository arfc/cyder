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

#include "NuclideModel.h"

enum FormulationType{
  DM, 
  EM, 
  PFM, 
  LAST_FORMULATION_TYPE};

/// A shared pointer for the LumpedNuclide object
class LumpedNuclide;
typedef boost::shared_ptr<LumpedNuclide> LumpedNuclidePtr;

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
private: 
  
  /**
     Default constructor for the lumped parameter nuclide model class.
     Creates an empty lumped parameter volume with contaminant concentration 
     zero unless an initial contaminant concentration is specified.
   */
  LumpedNuclide(); 

  /**
     primary constructor reads input from QueryEngine
     
     @param qe is the QueryEngine object containing intialization info
   */
  LumpedNuclide(QueryEngine* qe);

public:
  /**
     A constructor for the Lumped Nuclide Model that returns a shared pointer.
    */
  static LumpedNuclidePtr create (){ LumpedNuclidePtr to_ret = LumpedNuclidePtr(new LumpedNuclide()); to_ret->set_formulation(DM); return to_ret;};

  /**
     A constructor for the Lumped Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static LumpedNuclidePtr create (QueryEngine* qe){ return LumpedNuclidePtr(new LumpedNuclide(qe)); };

  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~LumpedNuclide();

  /**
     initializes the model parameters from an QueryEngine object
     
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
     Absorbs the contents of the given Material into this LumpedNuclide
     model.
     
     @param matToAdd the LumpedNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this LumpedNuclide
     @param comp_to_rem the mass in kg to decrement against this LumpedNuclide

     @return the material extracted
   */
  virtual mat_rsrc_ptr extract(CompMapPtr comp_to_rem, double kg_to_rem);

  /**
     Transports nuclides from the inner to the outer boundary 

     @param time the timestep at which to transport the nuclides
   */
  virtual void transportNuclides(int time);

  /**
     Returns the nuclide model type
   */
  virtual NuclideModelType type(){return LUMPED_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "LUMPED_NUCLIDE";};

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
   * returns the flux at the boundary, the Neumann bc
   *
   * @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoFluxMap cauchy_bc(IsoConcMap c_ext, Radius r_ext);

  /// Returns the inner boundary condition 
  const IsoConcMap C_0() const {return C_0_;};

  /// Returns the formulation of the concentration relationship
  const FormulationType formulation() const {return formulation_;};

  /** Returns the FormulationType corresponding to the string
   * 
   * @param the name of the formulation type (EM, PFM, DM, etc.)
   * @return the formulation type corresponding to the string
   */
  FormulationType enumerateFormulation(std::string formulation);

  /// Sets the internal boundary condition
  void set_C_0(IsoConcMap C_0){C_0_ = C_0;};

  /// Sets the formulation of the concentration relationship
  void set_formulation(std::string formulation){formulation_ = enumerateFormulation(formulation);};

  /// Sets the formulation of the concentration relationship
  void set_formulation(FormulationType formulation){formulation_ = formulation;};

  /// Sets the porosity_ variable, the percent of the permeable porous medium.
  void set_porosity(double porosity);

  /// Sets the peclet_ variable, the ratio of advective to diffusive transport.
  void set_Pe(double Pe);

  /// Returns the peclet number of the component [-]
  const double Pe() const {return Pe_;};

  /**
    The advective velocity through this component. [m/s] 
   */
  const double v() const {return v_;};

  /// The porosity of the permeable porous mediuam of this component. [%]
  const double porosity() const {return porosity_;};

  /// Gets the total volume
  double V_T();

  /// Gets the free fluid volume. Same as V_f (no sorption or solubility)
  virtual double V_ff();

  /// Gets the fluid volume, based on porosity
  double V_f();

  /// Gets the fluid volume, based on porosity
  double V_s();

  /** 
     DM model concentration calculator
     @param C_0 the incoming concentration map
     @param the_time the length of the timestep over which to calculate
     @return C_f the final concentration at the end of the timestep
     */
  IsoConcMap C_DM(IsoConcMap C_0, int the_time);

  /**
     EM model concentration calculator

     @param C_0 the incoming concentration map
     @param the_time the length of the timestep over which to calculate
     @return C_f the final concentration at the end of the timestep
     */
  IsoConcMap C_EM(IsoConcMap C_0, int the_time);

  /** 
    PFM model concentration calculator

     @param C_0 the incoming concentration map
     @param the_time the length of the timestep over which to calculate
     @return C_f the final concentration at the end of the timestep
    */
  IsoConcMap C_PFM(IsoConcMap C_0, int the_time);

  /** 
     Determines what IsoVector to remove from the daughter nuclide models

     @param time the timestep at which the nuclides should be transported
     @param daughter nuclide_model of an internal component. there may be many.
     
    */
  void update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters); 

  /**
     Extracts one timestep's mass from the daughter nuclidemodel.

     @param daughter the nuclide model from which to extract mass
     @param dt the timestep length over which to integrate

     @return the material that has been extracted from daughter
     */
  mat_rsrc_ptr extractIntegratedMass(NuclideModelPtr daughter, 
      double dt);

  /** 
     Updates the contained vector

     @param the_time the time at which to update the vector
    */
  void update_vec_hist(int the_time);

  /** 
     Updates the available concentration using the wastes_ as mats

     @param the_time the time at which to update the IsoConcMap
    */
  void update_conc_hist(int the_time);

  /** 
     Updates the available concentration

     @param the_time the time at which to update the IsoConcMap
     @param mat the material that is part of the available concentration
    */
  void update_conc_hist(int the_time, mat_rsrc_ptr mat);

  /** 
    concentration calculator for this->formulation_ model

     @param C_0 the incoming concentration map
     @param the_time the length of the timestep over which to calculate
     @return C_f the final concentration at the end of the timestep
    */
  IsoConcMap C_t(IsoConcMap C_0, int the_time);


protected:
  /**
    The advective velocity through this component [m/s]
   */
  double v_;
  /**
   * The name of the lumped parameter model formulation. This can be 
   * the Exponential Model (EM), Piston Flow Model (PFM), 
   * or the Dispersion Model(DM).
   */
  FormulationType formulation_;

  /// The transit time of a radioactive tracer through the cell
  double t_t_;

  /// The dimensionless mass diffusion Peclet number of the medium [-].
  double Pe_;

  /// The porosity of the permeable porous medium
  double porosity_;

  /// the current conc map at the inner boundary
  IsoConcMap C_0_;

};


#endif
