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
  EPM, 
  PFM, 
  LAST_FORMULATION_TYPE};

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
     primary constructor reads input from QueryEngine
     
     @param qe is the QueryEngine object containing intialization info
   */
  LumpedNuclide(QueryEngine* qe);

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
     Extracts the contents of the given Material from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this LumpedNuclide
     @param comp_to_rem the mass in kg to decrement against this LumpedNuclide
   */
  virtual void extract(CompMapPtr comp_to_rem, double kg_to_rem);

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

  /// Returns the formulation of the concentration relationship
  FormulationType formulation(){return formulation_;};

  /** Returns the FormulationType corresponding to the string
   * 
   * @param the name of the formulation type (EM, PFM, DM, etc.)
   * @return the formulation type corresponding to the string
   */
  FormulationType enumerateFormulation(std::string formulation);

  /// Sets the formulation of the concentration relationship
  void set_formulation(std::string formulation){formulation_ = enumerateFormulation(formulation);};

  /// Sets the formulation of the concentration relationship
  void set_formulation(FormulationType formulation){formulation_ = formulation;};

  /// Returns the transit time of the radioactive tracer through the cell
  double transit_time(){return t_t_;};

  /**
    The advective velocity through this component. [m/s] 
   */
  double v(){return v_;};

protected:
  /**
    The advective velocity through this component [m/s]
   */
  double v_;
  /**
   * The name of the lumped parameter model formulation. This can be 
   * the Exponential Model (EM), Piston Flow Model (PFM), Combined Exponential and 
   * Piston Flow Model (EPM), or the Dispersion Model(DM).
   */
  FormulationType formulation_;

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
