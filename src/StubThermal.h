/*! \file StubThermal.h
  \brief Declares the StubThermal class that gives a concrete ThermalModel example 
  \author Kathryn D. Huff
 */
#if !defined(_STUBTHERMAL_H)
#define _STUBTHERMAL_H

#include <iostream>
#include "Logger.h"
#include <string>

#include "ThermalModel.h"

/// A shared pointer for the StubThermal object
class StubThermal;
typedef boost::shared_ptr<StubThermal> StubThermalPtr;

/** 
   @brief StubThermal is a skeleton component model that does nothing.
   
   This disposal system component will do nothing. This Component is 
   intended as a skeleton to guide the implementation of new Components. 
   
   The StubThermal model can be used to represent components of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class StubThermal : public ThermalModel {
public:
  
  /**
     Default constructor for the component class. Creates an empty component.
   */
  StubThermal(){}; 

  /**
     primary constructor reads input from XML node
     
     @param qe is the QueryEngine object containing intialization info
   */
  StubThermal(QueryEngine* qe){};

  /** 
     Default destructor does nothing.
   */
  ~StubThermal() {};

  /**
     A constructor for the Lumped Nuclide Model that returns a shared pointer.
    */
  static ThermalModelPtr create (){ return ThermalModelPtr(new StubThermal()); };

  /**
     A constructor for the Lumped Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static ThermalModelPtr create (QueryEngine* qe){ return ThermalModelPtr(new StubThermal(qe)); };

  /**
     initializes the model parameters from an xmlNodePtr
     
     @param qe is the QueryEngine object containing intialization info
   */
  virtual void initModuleMembers(QueryEngine* qe); 

  /**
     A function that copies deeply.
   */
  StubThermalPtr deepCopy();

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual void copy(ThermalModelPtr src); 

  /**
     standard verbose printer includes current temp and concentrations
   */
  virtual void print(); 

  /**
     transports the heat

     @param time the timestep at which to transport the heat
   */
  virtual void transportHeat(int time);

  /**
     return the thermal model implementation type
     
     @return impl_name_ the name of the ThermalModel implementation 
   */
  const virtual ThermalModelType type(){return STUB_THERMAL;}; 

  /**
     return the thermal model implementation type
     
     @return impl_name_ the name of the ThermalModel implementation 
   */
  virtual std::string name(){return "STUB_THERMAL";}; 

  /**
     This function says whether or not the material is acceptable to 
     this repository at r_lim for the limit t_lim

     @param mat the material whose whose contribution to query 
     @param r_lim the limiting radius
     @param t_lim the limiting temperature  

     @return mat_acceptable (true when acceptable, false otherwise)  
    */
  virtual bool mat_acceptable(mat_rsrc_ptr mat, Radius r_lim, Temp t_lim);
  
  /**
     gets the peak temperature that this component will experience on the 
     boundary

     @TODO this is a placeholder!!! deal with it.
   */
  virtual Temp peak_temp();

  /**
     gets the temperature average in the component
     
   */
  virtual Temp temp();

};
#endif
