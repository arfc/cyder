/** \file Component.h
 * \brief Declares the Component class used by the Generic Repository 
 * \author Kathryn D. Huff
 */
#if !defined(_COMPONENT_H)
#define _COMPONENT_H

#include <vector>
#include <map>
#include <string>

#include "Material.h"
#include "MaterialDB.h"
#include "ThermalModel.h"
#include "NuclideModel.h"
#include "Geometry.h"

/*!
A map for storing the composition history of a material.

@TODO IsoVector should be used instead of this
*/
typedef std::map<int, std::map<int, double> > CompHistory;

/*!
A map for storing the mass history of a material.

@TODO IsoVector should be used instead of this
*/
typedef std::map<int, std::map<int, double> > MassHistory;

/// type definition for Concentrations in kg/m^3
typedef double Concentration;

/// type definition for ConcMap 
typedef std::map<int, Concentration> ConcMap;

/// type definition for Temperature in Kelvin 
typedef double Temp;

/// type definition for Power in Watts
typedef double Power;

/// Enum for type of engineered barrier component.
enum ComponentType {BUFFER, ENV, FF, NF, WF, WP, LAST_EBS};

/// A shared pointer for the component object
class Component;
typedef boost::shared_ptr<Component> ComponentPtr;

/** 
   @brief Defines interface for subcomponents of the GenericRepository
   
   Components such as the Waste Form, Waste Package, Buffer, Near Field,
   Far Field, and Envrionment will share a universal interface so that 
   information passing concerning fluxes and other boundary conditions 
   can be passed in and out of them.
 */
class Component : public boost::enable_shared_from_this<Component> {

public:
  /**
     Default constructor for the component class. Creates an empty component.
   */
  Component();

  /** 
     Default destructor does nothing.
   */
  ~Component();

  /**
     initializes the model parameters from an QueryEngine object
     and calls the explicit init(ID, name, ...) function. 
     
     @param qe is the QueryEngine object containing intialization info
   */
  void initModuleMembers(QueryEngine* qe); 

  /**
     initializes the model parameters from an QueryEngine object
     
     @param name  the name_ data member, a string
     @param type the type_ data member, a ComponentType enum value
     @param mat the name to initialize the mat_table_ data member, a string
     @param inner_radius the inner_radius_ data member, in meters
     @param outer_radius the outer_radius_ data member, in meters 
     @param thermal_model the thermal_model_ data member, a pointer
     @param nuclide_model the nuclide_model_ data member, a pointer
   */
  void init(std::string name, ComponentType type, std::string mat, Radius inner_radius,
      Radius outer_radius, ThermalModelPtr thermal_model, NuclideModelPtr nuclide_model); 

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  void copy(const ComponentPtr& src); 

  /**
     standard verbose printer includes current temp and concentrations
   */
  void print(); 

  /**
     Absorbs the contents of the given Material into this Component.
     
     @param mat_to_add the Material to be absorbed
   */
  void absorb(mat_rsrc_ptr mat_to_add);

  /**
     Extracts the contents of the given composition from this Component. Use this 
     function for decrementing a Component's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this Component
     @param kg_to_rem the mass in kg to decrement against this Component
   */
  void extract(CompMapPtr comp_to_rem, double kg_to_rem);

  /**
     Transports heat from the inner boundary to the outer boundary in this 
     component
     
     @param time the timestep at which to transport the heat
   */
  void transportHeat(int time);

  /**
     Transports nuclides from the inner boundary to the outer boundary in this 
     component

     @param time the timestep at which to transport the nuclides
   */
  void transportNuclides(int time);

  /** 
     Loads this component with another component.
     
     @param type the ComponentType of this component
     @param to_load the Component to load into this component
   */
  ComponentPtr load(ComponentType type, ComponentPtr to_load);

  /**
     Reports true if the component is full of whatever goes inside it.
     
     @return TRUE if the component is full and FALSE if there is no more room 
   */
  bool isFull() ;

  /**
     Returns the ComponentType of this component (WF, WP, etc.)
     
     @return type_ the ComponentType of this component
   */
  ComponentType type();

  /**
     set the ComponentType
   */
  void set_type(ComponentType type){type_=type;};

  /**
     Enumerates a string if it is one of the named ComponentTypes
     
     @param type the name of the ComponentType (i.e. FF)
     @return the ComponentType enum associated with this string by the 
     component_type_names_ list 
   */
  ComponentType componentEnum(std::string type);
  
  /**
     Enumerates a string if it is one of the named ThermalModelTypes
     
     @param type the name of the ThermalModelType (i.e. StubThermal)
   */
  ThermalModelType thermalEnum(std::string type);

  /** 
     Enumerates a string if it is one of the named NuclideModelTypes
     
     @param type the name of the NuclideModelType (i.e. StubNuclide)
   */
  NuclideModelType nuclideEnum(std::string type);

  /** 
     Returns a new thermal model of the string type QueryEngine object
     
     @param qe is the QueryEngine object containing intialization info
     @return thermal_model_ a pointer to the ThermalModel that was created
   */
  ThermalModelPtr thermal_model(QueryEngine* qe);

  /** 
     Returns a new nuclide model of the string type and QueryEngine object
     
     @param qe is the QueryEngine object containing intialization info
     @return nuclide_model_ a pointer to the NuclideModel that was created
   */
  NuclideModelPtr nuclide_model(QueryEngine* qe);

  /** 
     Returns a new thermal model that is a copy of the src model
     
     @param src a pointer to the thermal model to copy
     @return a pointer to the ThermalModel that was created
   */
  ThermalModelPtr copyThermalModel(ThermalModelPtr src);

  /** 
     Returns atd::s new nuclide model that is a copy of the src model
   */
  NuclideModelPtr copyNuclideModel(NuclideModelPtr src);

  /**
     get the ID
     
     @return ID_
   */
  const int ID();

  /**
     set the Name

     @param the new name
   */
  void set_name(std::string name){name_=name;};

  /**
     get the Name
     
     @return name_
   */
  const std::string name();

  /**
     set the material type that this component is made of (clay, salt, glass, etc.)
     and retrieves the appropriate MatDataTablePtr

     @param mat the name of the material type (clay, salt, glass, etc.)
   */
  void set_mat_table(std::string mat);

  /**
     set the material type that this component is made of (clay, salt, glass, etc.)
     and retrieves the appropriate MatDataTablePtr

     @param mat_table the MatDataTablePtr for the material type (clay, salt, glass, etc.)
   */
  void set_mat_table(MatDataTablePtr mat_table);

  /**
     get the material table that this component is made of (clay, salt, glass, etc.)
     
     @return mat_table
   */
  const MatDataTablePtr mat_table();
 
  /**
     get the list of daughter components 
     
     @return components
   */
  const std::vector<ComponentPtr> daughters();

  /**
     get the parent component 
     
     @return component
   */
  ComponentPtr parent();

  /**
     get the list of waste objects 
     
     @return wastes
   */
  const std::vector<mat_rsrc_ptr> wastes();

  /**
     get the maximum Temperature this object allows at its boundaries 
     
     @return temp_lim_
   */
  const Temp temp_lim();

  /**
     get the maximum Toxicity this object allows at its boundaries 
     
     @return tox_lim_
   */
  const Tox tox_lim();

  /**
     get the peak Temperature this object will experience during the simulation
     
     @param type indicates whether to return the inner or outer peak temp
     
     @return peak_temp_
   */
  const Temp peak_temp(BoundaryType type);

  /**
     get the Temperature
     
     @return temp_
   */
  const Temp temp();

  /**
     get the inner radius of the object
     
     @return inner_radius_ in [meters]
   */
  const Radius inner_radius();

  /**
     get the inner radius of the object

     @return outer_radius_ in [meters]
   */
  const Radius outer_radius();

  /// get the centroid position vector of the object
  const point_t centroid();

  /// get the x component of the centroid position vector of the object
  const double x();

  /// get the y component of the centroid position vector of the object
  const double y();

  /// get the z component of the centroid position vector of the object
  const double z();

  /// set the pointer to the geometry object
  void set_geom(GeometryPtr geom) {geom_ = geom;};

  /// get the pointer to the geometry object
  const GeometryPtr geom() {return geom_;};

  /**
     gets the pointer to the nuclide model being used in this component
     
     @return nuclide_model_
   */
  NuclideModelPtr nuclide_model();

  /**
     sets the nuclide model to the src nuclide model
   */
  void set_nuclide_model(const NuclideModelPtr& src){ nuclide_model_ = NuclideModelPtr(src);};

  /**
     gets the pointer to the thermal model being used in this component
     
     @return thermal_model_
   */
  ThermalModelPtr thermal_model();

  /**
     sets the thermal model to the src thermal model
   */
  void set_thermal_model(const ThermalModelPtr& src){ thermal_model_ = ThermalModelPtr(src);};

  /**
     set the parent component 
     
     @param parent is the component that should be set as the parent
   */
  void setParent(ComponentPtr parent){parent_ = parent;};

  /**
     set the placement of the object
     
     @param centroid is the centroid position vector
   */
  void setPlacement(point_t centroid){
    geom_->set_centroid(centroid);};

protected:
  /** 
     The serial number for this Component.
   */
  int ID_;

  /**
     Stores the next available component ID
   */
  static int nextID_;

  /// ThermalModleType names list
  static std::string thermal_type_names_[LAST_THERMAL];

  /// NuclideModelType names list
  static std::string nuclide_type_names_[LAST_NUCLIDE];

  /**
     The composition history of this Component, in the form of a map whose
     keys are integers representing the time at which this Component had a 
     particular composition and whose values are the corresponding 
     compositions. A composition is a map of isotopes and their 
     corresponding number of atoms.
   */
  CompHistory comp_hist_;

  /**
     The mass history of this Component, in the form of a map whose
     keys are integers representing the time at which this Component had a 
     particular composition and whose values are the corresponding mass 
     compositions. A composition is a map of isotopes and the corresponding
     masses.
   */
  MassHistory mass_hist_;

  /**
     The type of thermal model implemented by this component
   */
  std::string mat_;

  /**
     The type of thermal model implemented by this component
   */
  ThermalModelPtr thermal_model_;

  /**
     The type of nuclide model implemented by this component
   */
  NuclideModelPtr nuclide_model_;

  /**
     The immediate parent component of this component.
   */
  ComponentPtr parent_;

  /**
     The immediate daughter components of this component.
   */
  std::vector<ComponentPtr> daughters_;

  /**
     The contained contaminants, a list of material objects..
   */
  std::vector<mat_rsrc_ptr> wastes_;

  /**
     The name of this component, a string
   */
  std::string name_;

  /**
     A pointer to the MatDataTable representing this Component's material 
    */
  MatDataTablePtr mat_table_;

  /**
     The geometry of the cylindrical component, a shared pointer
   */
  GeometryPtr geom_;

  /**
     The type of component that this component represents (ff, buffer, wp, etc.) 
   */
  ComponentType type_;

  /**
     The temp limit of this component 
   */
  Temp temp_lim_;

  /**
     The toxlimit of this component 
   */
  Tox tox_lim_;

  /**
     The peak temp achieved at the outer boundary 
   */
  Temp peak_outer_temp_;

  /**
     The peak temp achieved at the inner boundary 
   */
  Temp peak_inner_temp_;

  /**
     The peak tox achieved  
   */
  Tox peak_tox_;

  /**
     The temp taken to be the homogeneous temp of the whole 
     component.
   */
  Temp temp_;

  /**
     The concentrations of contaminant isotopes in the component
   */
  ConcMap concentrations_;


};



#endif
