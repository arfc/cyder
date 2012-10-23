/*! \file LumpedThermal.h
  \brief Declares the LumpedThermal class that gives a concrete ThermalModel example 
  \author Kathryn D. Huff
 */
#if !defined(_LUMPEDTHERMAL_H)
#define _LUMPEDTHERMAL_H

#include <iostream>
#include "Logger.h"
#include <string>


#include "ThermalModel.h"

/** 
   @brief LumpedThermal is a skeleton component model that does nothing.
   
   This disposal system component will do nothing. This Component is 
   intended as a skeleton to guide the implementation of new Components. 
   
   The LumpedThermal model can be used to represent components of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class LumpedThermal : public ThermalModel {
public:
  
  /**
     Default constructor for the component class. Creates an empty component.
   */
  LumpedThermal(){}; 

  /**
     primary constructor reads input from XML node
     
     @param qe is the QueryEngine object containing intialization info
   */
  LumpedThermal(QueryEngine* qe){};

  /** 
     Default destructor does nothing.
   */
  ~LumpedThermal() {};

  /**
     initializes the model parameters from an QueryEngine object
     
     @param qe is the QueryEngine object containing intialization info
   */
  virtual void initModuleMembers(QueryEngine* qe); 

  /**
     A function that copies deeply.
   */
  LumpedThermal* deepCopy();

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual void copy(ThermalModel* src); 

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
  const virtual ThermalModelType type(){return LUMPED_THERMAL;}; 

  /**
     return the thermal model implementation type
     
     @return impl_name_ the name of the ThermalModel implementation 
   */
  virtual std::string name(){return "LUMPED_THERMAL";}; 
  
  /**
     gets the peak temperature that this component will experience on the 
     boundary
     
   */
  virtual Temp peak_temp();

  /**
     gets the temperature average in the component
     
   */
  virtual Temp temp();

};
#endif
