/*! \file ThermalModel.h
  \brief Declares the ThermalModel virtual class defining the heat transport model interface. 
  \author Kathryn D. Huff
 */
#if !defined(_THERMALMODEL_H)
#define _THERMALMODEL_H

#include <map>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "Material.h"
#include "Geometry.h"

/**  
   type definition for Temperature in Kelvin
 */
typedef double Temp;

/**
   type definition for the history of the component temperature. Time, Temp.
  */
typedef std::map<int, Temp> TempHist;

/**
   type definition for Power in Watts
 */
typedef double Power;

/**
   enumerator for the component models available to the repo
 */
enum ThermalModelType{
  LUMPED_THERMAL,
  STUB_THERMAL, 
  LAST_THERMAL};  

/** 
   @brief Abstract interface for heat transport models to be used in the GenericRepository
   
   ThermalModels such as Lumped, Stub etc,
   will share this virtual interface so that they can be interchanged within the  
   GenericRepository.
 */
class ThermalModel {

public:

  /** 
     A virtual destructor
    */
  virtual ~ThermalModel() {};

  /**
     initializes the model parameters from an xmlNodePtr
     
     @param cur is the current xmlNodePtr
   */
  virtual void init(xmlNodePtr cur)=0; 

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual void copy(ThermalModel* src)=0; 

  /**
     standard verbose printer should include current temp and concentrations
   */
  virtual void print()=0; 

  /**
     transports the heat

     @param time the timestep at which to transport the heat
   */
  virtual void transportHeat(int time) =0;

  /**
     get the thermal model implementation type
   */
  const virtual ThermalModelType type()=0;

  /**
     get the name of the thermal model implementation type
   */
  virtual std::string name()=0;

  /**
     get the peak Temperature this object will experience during the simulation
     
     
   */
  virtual Temp peak_temp() = 0;

  /**
     get the Temperature
     
     @return temperature_
   */
  virtual Temp temp()=0;

  /**
     set the geometry shared pointer

     @param geom the geometry of the component, a shared pointer
    */
  void set_geom(GeometryPtr geom){geom_ = geom;};

protected:
  /// The temperature history of this component
  TempHist temp_hist_;

  /// The temperature of this component, on average
  Temp temperature_;

  /// A shared pointer to the geometry of this component
  GeometryPtr geom_;

};


#endif
