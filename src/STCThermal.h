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
#include "STCDB.h"

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
  STCThermal(); 

  /**
     primary constructor reads input from XML node
     
     @param qe is the QueryEngine object containing intialization info
   */
  STCThermal(QueryEngine* qe);

public:

  /**
     A constructor for the STC Nuclide Model that returns a shared pointer.
    */
  static STCThermalPtr create(){ return STCThermalPtr(new STCThermal()); };

  /**
     A constructor for the STC Nuclide Model that returns a shared pointer.

     @param qe is the QueryEngine object containing intialization info
    */
  static STCThermalPtr create(QueryEngine* qe){ return STCThermalPtr(new STCThermal(qe)); };

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
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual void copy(const ThermalModel& src); 

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
     Returns the maximum temperature change due to a material

     @param mat the material which is responsible for heating
     @return the time and temperature change in this medium due to mat [K]
   */
  std::pair<int,Temp> getMaxTempChange(mat_rsrc_ptr mat);

  /**
     Returns the whole temperature change projection due to a material over 
     time

     @param mat the material which is responsible for heating
     @return the <time,temperature change> vector in this medium due to mat [K]
   */
  std::map<int, Temp> getTempChange(mat_rsrc_ptr mat);

  /**
     Returns the temperature change due to a material

     @param tope the isotope for which to collect the STC
     @param the_time the timestep at which to report the temp change
     @return the temperature change in this medium due to mat [K]
   */
  Temp getTempChange(Iso tope, int the_time);

  /**
     Returns the temperature change due to a material

     @param mat the material which is responsible for heating
     @param the_time the timestep at which to report the temp change
     @return the temperature change in this medium due to mat [K]
   */
  Temp getTempChange(mat_rsrc_ptr mat, int the_time);

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
  void set_alpha_th(double alpha_th);
  /// sets k_th_, the thermal conductivity[]
  void set_k_th(double k_th);
  /// sets spacing_, the spacing between waste packages (uniform grid) [m]
  void set_spacing(double spacing);
  /// sets r_calc_, the radius at which the temperature is to be calculated [m]
  void set_r_calc(double r_calc);
  /// sets mat_, an optional name for the material through which to transport heat
  void set_mat(std::string mat){mat_=mat;};
  /// returns alpha_th_, the thermal diffusivity []
  const double alpha_th() const {return alpha_th_;};
  /// returns k_th_, the thermal conductivity []
  const double k_th() const {return k_th_;};
  /// returns spacing_, the spacing between waste packages (uniform grid) [m]
  const double spacing() const {return spacing_;};
  /// returns spacing_, the spacing between waste packages (uniform grid) [m]
  const double r_calc() const {return spacing_;};
  /// returns mat_, the mat_t material through which to transport heat
  const std::string mat() const {return mat_;};

protected:
  /**
     initializes the STC map from the STCDB.

     @param mat the mat_t struct defining the near field material:w
     */
  void initializeSTCTable();

  /**
    an STCDataTable containing a fully interpereted array of STC values indexed 
    by isotope and time for specifically this mat_t.
    */
  STCDataTablePtr table_;

  /// the thermal diffusivity []
  double alpha_th_;
  /// the thermal conductivity []
  double k_th_;
  /// the spacing between waste packages (uniform grid) [m]
  double spacing_;
  /// the r_calc radius at which the temperature is calculated [m]
  double r_calc_;
  /// This parameter is optional and sets default alpha, k, s, r for the mat_t
  std::string mat_;

};
#endif
