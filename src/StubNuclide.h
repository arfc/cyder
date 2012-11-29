/*! \file StubNuclide.h
    \brief Declares the StubNuclide class that gives an example of a concrete NuclideModel. 
    \author Kathryn D. Huff
 */
#if !defined(_STUBNUCLIDE_H)
#define _STUBNUCLIDE_H

#include <iostream>
#include "Logger.h"
#include <vector>
#include <map>
#include <string>

#include "NuclideModel.h"


/// A shared pointer for the StubNuclide object
class StubNuclide;
typedef boost::shared_ptr<StubNuclide> StubNuclidePtr;

/** 
   @brief StubNuclide is a skeleton nuclide model model that does nothing.
   
   This disposal system nuclide model will do nothing. This NuclideModel is 
   intended as a skeleton to guide the implementation of new NuclideModels. 
   
   The StubNuclide model can be used to represent nuclide models of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class StubNuclide : public NuclideModel {
private:
  
  /**
     Default constructor for the nuclide model class. Creates an empty nuclide model.
   */
  StubNuclide(); 

  /**
     primary constructor reads input from QueryEngine
     
     @param qe is the QueryEngine object containing intialization info
   */
  StubNuclide(QueryEngine* qe);

public:
  /**
     Virtual destructor deletes datamembers that are object pointers.
    */
  virtual ~StubNuclide();

  /**
     A constructor for the Stub Nuclide Model that returns a shared pointer.
    */
  static StubNuclidePtr create (){ return StubNuclidePtr(new StubNuclide()); };

  /**
     A constructor for the Stub Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static StubNuclidePtr create (QueryEngine* qe){ return StubNuclidePtr(new StubNuclide(qe)); };

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
     Absorbs the contents of the given Material into this StubNuclide.
     
     @param matToAdd the StubNuclide to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) ;

  /**
     Extracts the contents of the given Material from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this StubNuclide
     @param comp_to_rem the mass in kg to decrement against this StubNuclide
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
  virtual NuclideModelType type(){return STUB_NUCLIDE;};

  /**
     Returns the nuclide model type name
   */
  virtual std::string name(){return "STUB_NUCLIDE";}; 

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


};
#endif
