/*! \file STCThermal.h
  \brief Declares the STCThermal class that gives a concrete ThermalModel example 
  \author Kathryn D. Huff
 */
#if !defined(_STCTHERMAL_H)
#define _STCTHERMAL_H

#include <iostream>
#include "Logger.h"
#include <string>


#include "ThermalModel.h"

/// A shared pointer for the STCThermal object
class STCThermal;
typedef boost::shared_ptr<STCThermal> STCThermalPtr;


/** 
   @brief STCThermal is a skeleton component model that does nothing.
   
   This disposal system component will do nothing. This Component is 
   intended as a skeleton to guide the implementation of new Components. 
   
   The STCThermal model can be used to represent components of the 
   disposal system such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment.
 */
class STCThermal : public ThermalModel {
private:
  /**
     Default constructor for the component class. Creates an empty component.
   */
  STCThermal(){}; 

  /**
     primary constructor reads input from XML node
     
     @param qe is the QueryEngine object containing intialization info
   */
  STCThermal(QueryEngine* qe){};

public:

  /**
     A constructor for the STC Nuclide Model that returns a shared pointer.
    */
  static ThermalModelPtr create(){ return ThermalModelPtr(new STCThermal()); };

  /**
     A constructor for the STC Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static ThermalModelPtr create(QueryEngine* qe){ return ThermalModelPtr(new STCThermal(qe)); };

  /** 
     Default destructor does nothing.
   */
  ~STCThermal() {};

  /**
     initializes the model parameters from an QueryEngine object
     
     @param qe is the QueryEngine object containing intialization info
   */
  virtual void initModuleMembers(QueryEngine* qe); 

  /**
     A function that copies deeply.
   */
  STCThermalPtr deepCopy();

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
  const virtual ThermalModelType type(){return STC_THERMAL;}; 

  /**
     return the thermal model implementation type
     
     @return impl_name_ the name of the ThermalModel implementation 
   */
  virtual std::string name(){return "STC_THERMAL";}; 

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
   */
  virtual Temp peak_temp();

  /**
     gets the temperature average in the component
     
   */
  virtual Temp temp();

  /// sets alpha_th_, the thermal diffusivity []
  void set_alpha_th(double alpha_th){alpha_th_=alpha_th;};
  /// sets k_th_, the thermal conductivity[]
  void set_k_th(double k_th){k_th_=k_th;};
  /// sets spacing_, the spacing between waste packages (uniform grid) [m]
  void set_spacing(double spacing){spacing_=spacing;};
  /// sets mat_, the material through which to transport heat
  void set_mat(std::string mat){mat_=mat;};
  /// returns alpha_th_, the thermal diffusivity []
  double alpha_th(){return alpha_th_;};
  /// returns k_th_, the thermal conductivity []
  double k_th(){return k_th_;};
  /// returns spacing_, the spacing between waste packages (uniform grid) [m]
  double spacing(){return spacing_;};
  /// returns mat_, the material through which to transport heat
  double mat(){return mat_;};

protected:
  /// the thermal diffusivity []
  double alpha_th_;
  /// the thermal conductivity []
  double k_th_;
  /// the spacing between waste packages (uniform grid) [m]
  double spacing_;
  /// the material through which to transport heat
  double mat_;

};
#endif
