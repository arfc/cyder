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


/** 
   @brief OneDimPPMNuclide is a nuclide transport model that treats the volume 
   as a homogeneously mixed cell.
   
   This disposal system nuclide model will receive material diffusively. That is, at
   the internal boundary, if the concentration from an adjacent cell is
   higher than the concentration within the mixed cell, this cell will accept the 
   appropriate flux accross that boundary. So too if the concentration is lower in the 
   adjacent cell, a corresponding contaminant flux will leave this cell accross that boundary. 
   
   When accepting a flux of Material, this cell immediately incorporates it into 
   the mixed volume. The mixed volume will then make the corresponding homogeneous 
   concentration available at all boundaries.  
   
   The OneDimPPMNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class OneDimPPMNuclide : public NuclideModel {
public:
  
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
  OneDimPPMNuclide(QueryEngine* qe){};

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
  virtual NuclideModel* copy(NuclideModel* src); 

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
  virtual NuclideModelType type(){return ONEDIMPPM_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "ONEDIMPPM_NUCLIDE";};

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
  virtual IsoConcMap cauchy_bc();

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
