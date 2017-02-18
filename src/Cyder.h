/*! \file Cyder.h
  \brief Declares the Cyder class, the central class of Cyder 
  \author Kathryn D. Huff
 */
#if !defined(_CYDER_H)
#define _CYDER_H
#include "Logger.h"
#include <queue>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>

#include "Facility.h"
#include "Component.h"

/**
   type definition for waste stream objects
 */
typedef std::pair<mat_rsrc_ptr, std::string> WasteStream;

/*! Cyder
    This Facility seeks to provide a generic disposal system model
   
   The Cyder class inherits from the Facility class and is 
   dynamically loaded by the Model class when requested.
   
   \section intro Introduction 
   
   The Cyder is a facility type in *Cyclus* which represents a generic 
   disposal system. It continually requests wastes of all incoming commodities that
   it allows until the capacity has been reached. While there are new waste streams 
   in the stocks, they are conditioned into a waste form, packaged into a waste 
   package, and emplaced in a buffer.
   
   \section modelparams Model Parameters 
   
   Cyder behavior is comprehensively defined by the following 
   parameters:
   - double capacity : The production capacity of the facility (units vary, but 
   typically kg/month). *Question:* Do we want to allow this to be infinite?  
   - int startOpYear : The year in which the facility begins to operate .
   - int startOpMonth : The month in which the facility begins to operate .
   - int lifeTime : The length of time that the facility operates (months).
   - std::string inCommod : One or more types of commodity that this facility accepts.
   - Component`*` component : One or more types of component that facility contains
   - Inst`*` inst : The institution responsible for this facility.  
   - double area : The square  meters of topographic area that the repository may occupy.
   
   \section optionalparams Optional Parameters  
   
   Cyder behavior may also be specified with the following optional 
   parameters which have default values listed here...  
   
   \section detailed Detailed Behavior 
   
   The Cyder is under development at this time. 
   
   In general, it starts operation when the simulation reaches the month specified 
   as the startDate. Each month, the Cyder makes a request for the 
   inCommod commodity types at a rate corresponding to the calculated capacity less 
   the amount it currently has in its stocks (which begins the simulation empty). 
   
   If a request is matched with an offer, the Cyder receives that 
   order from the supplier and adds the quantity to its stocks. 
   
   At the end of the month, it then proceeds to condition, package, and load the 
   material into the repository according to thermal and nuclide constraints. 
   
   During each month, nuclide and heat transport calculations are completed within 
   the repository in order to determine useful fuel cycle metrics.
   
   When the simulation time equals the start date plus the lifetime, the facility 
   ceases to operate. 
   
 */
class Cyder : public cyclus::Facility  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /// Default constructor for the Cyder class.
  Cyder(cyclus::Context* ctx);

  /// Destructor for the Cyder class. 
  virtual ~Cyder();

  #pragma cyclus note { \
  "doc": \
  "The Cyder is a facility type in *Cyclus* which represents a generic\n"\
  "disposal system. It continually requests wastes of all incoming commodities that\n"\
  "it allows until the capacity has been reached. While there are new waste streams \n"\
  "in the stocks, they are conditioned into a waste form, packaged into a waste \n"\
  "package, and emplaced in a buffer\n"."\
  }
  
  /// initialize an object from QueryEngine input
  virtual void initModuleMembers(QueryEngine* qe);

  /// initialize an object by cloning module members from another
  virtual void cloneModuleMembersFrom(Facility* src);

  /**
     @brief deep copy method 
     
     This drills down the dependency tree to initialize all relevant 
     parameters/containers.
     
     Note that this function must be defined only in the specific 
     model in question and not in any inherited models preceding it.
     
     @param src the pointer to the original model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /// Print information about this model
  virtual std::string str();

  /**
     Transacted resources are received through this method
     
     @param trans the transaction to which these resource objects belong
     @param manifest is the set of resources being received
   */ 
  virtual void addResource(Transaction trans, std::vector<rsrc_ptr> manifest);
/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

 public:
  /**
     @brief When the facility receives a message, execute any transaction therein
   */
  virtual void receiveMessage(msg_ptr msg);

/* -------------------- */
 

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

#pragma cyclus 

   virtual void EnterNotify();
    /**
       The handleTick function specific to the Cyder.
       At each tick, it requests as much raw inCommod as it can 
       process this * month and offers as much outCommod as it 
       will have in its inventory by the end of the month.
       
       
     */
    virtual void Tick();

    /**
       The handleTick function specific to the Cyder.
       At each tock, it processes material and handles orders, and 
       records this month's actions.
       
       
     */
    virtual void Tock();

   
    

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

public:

protected:
    /**
     * This map holds the input name, a variable reference, and the
     * database datatype of each of the simple datamembers.
     */
     std::map<const char*, boost::any> member_refs_ ;

    /**
       The Cyder has many input commodities
     */

    #pragma cyclus var {"tooltip": "input commodities", \
                        "doc": "commodities that the sink facility accepts", \
                        "uilabel": "List of Input Commodities", \
                        "uitype": "incommodity"}
    std::deque<std::string> in_commods_;

    /**
       This thermal model determines the acceptability of a material
       at radius r_lim_ due to temperature limit t_lim_
     */

    #pragma cyclus var {"tooltip": "thermal model", \
                        "doc": "Choose from 'LumpedThermal', 'STCThermal', 'StubThermal'", \
                        "uilabel": "Thermal Model", \
                        "uitype": "none"}
    ThermalModelPtr thermal_model_;

    /**
       A limit to how quickly the Cyder can accept waste.
       Units vary. It will be in the commodity unit per month.
     */
  #pragma cyclus var {"default": 1e299, \
                      "tooltip": "repository emplacement rate", \
                      "uilabel": "Maximum Throughput", \
                      "uitype": "range", \
                      "range": [0.0, 1e299], \
                      "doc": "amount the repository can " \
                             "accept at each time step"}
    double capacity_;

    /**
       The x, y, and z dimensional extents of the repository in kilometers.
     */
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "x dimension of repository in kilometers", \
                        "uilabel": "X Dimension Length [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "x dimension of the repository in kilometers"}
    double x_;

    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "y dimension of repository in kilometers", \
                        "uilabel": "Y Dimension Length [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "y dimension of the repository in kilometers"}
    double y_;

    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "z dimension of repository in kilometers", \
                        "uilabel": "Z Dimension Length [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "z dimension of the repository in kilometers"}
    double z_;

    /**
       The x, y, and z dimensional spacing of the source centers in kilometers.
     */

    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "x dimension spacing of centers in kilometers", \
                        "uilabel": "X Dimension Spacing [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "x dimension of the spacing of source centers in kilometers"}
    double dx_;
    
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "y dimension spacing of centers in kilometers", \
                        "uilabel": "Y Dimension Spacing [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "y dimension of the spacing of source centers in kilometers"}
    double dy_;

    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "z dimension spacing of centers in kilometers", \
                        "uilabel": "Z Dimension Spacing [km]", \
                        "units": "km", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "z dimension of the spacing of source centers in kilometers"}
    double dz_;

    /// The advective velocity (@TODO decide direction? +x or +z?) [m/yr] 
    // NOTE!!! NOT SI!! [m/yr]
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "advective velocity in meters per year", \
                        "uilabel": "Advective Velocity", \
                        "units": "m/yr", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "advective velocity in meters per year"}
    double adv_vel_;

    /**
       The stocks of pre-emplacement waste materials.
     */
    #pragma cyclus var {"tooltip": "stocks of pre-emplaced waste material", \
                        "uilabel": "PreEmplaced Stock", \
                        "uitype": "none", \
                        "doc": "stocks of pre-emplaced waste material"}
    std::deque<WasteStream> stocks_;

    /**
       The inventory of emplaced materials.
     */
    #pragma cyclus var {"tooltip": "inventory of emplaced materials", \
                        "uilabel": "Emplaced Inventory", \
                        "uitype": "none", \
                        "doc": "inventory of emplaced materials"}
    std::deque<WasteStream> inventory_;

    /**
       The maximum size to which the inventory may grow..
       The Cyder must stop processing the material in its stocks 
       when its inventory is full. (YMRLegislative = 70,000tHM) 
     */

    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "repository maximum inventory size", \
                        "uilabel": "Maximum Inventory", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "total maximum inventory size of repository"}
    double inventory_size_;

    /**
       The number of months that a facility stays operational.
       hopefully, this repository is forever, but just in case... 
     */
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "repository lifetime in months", \
                        "units": "month", \
                        "uilabel": "Repository Lifetime", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "repository lifetime in months"}
    int lifetime_;


    /**
       The radius at which the thermally limiting temperature takes effect [m]
     */
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "radius of thermal limiting temperature into effect", \
                        "units": "m", \
                        "uilabel": "Limit Radius", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "radius of thermal limiting temperature into effect"}
    Radius r_lim_;

    /**
       The limiting temperature at the limiting radius [K]
     */
    #pragma cyclus var {"default": 1e299, \
                        "tooltip": "limiting temperature at limiting radius", \
                        "units": "K", \
                        "uilabel": "Limiting Temperature", \
                        "uitype": "range", \
                        "range": [0.0, 1e299], \
                        "doc": "lmiting temperature at the limiting radius"}
     Temp t_lim_;

    /**
       Reports true if the repository has reached capacity, false otherwise
     */
    #pragma cyclus var {"tooltip": "true if the repository has reached capacity", \
                        "uilabel": "True If Repository Full", \
                        "uitype": "none", \
                        "doc": "if the repository has reached capacity"}
    bool is_full_;

    /**
       The Far Field component
     */

    #pragma cyclus var {"tooltip": "the far field component", \
                        "uilabel": "Far Field Comp", \
                        "uitype": "none", \
                        "doc": "the far field component."}
    ComponentPtr far_field_;

    /**
       The buffer template before initialization.
       This will be copied and initialized before use.
     */
    #pragma cyclus var {"tooltip": "the buffer template before initialization", \
                        "uilabel": "Buffer Template", \
                        "uitype": "none", \
                        "doc": "the buffer template before initialization"}
    ComponentPtr buffer_template_;

    /**
       The waste package component templates before initialization.
       These will be copied and initialized before use.
     */
    #pragma cyclus var {"tooltip": "the waste package template before initialization", \
                        "uilabel": "The Waste Package Template", \
                        "uitype": "none", \
                        "doc": "he waste package template before initialization"}
    std::deque<ComponentPtr> wp_templates_;

    /**
       The waste form templates before initialization.
       These will be copied and initialized before use.
     */

    #pragma cyclus var {"tooltip": "the waste form template before initialization", \
                        "uilabel": "TheWaste Form Template", \
                        "uitype": "none", \
                        "doc": "he waste form template before initialization"}
    std::deque<ComponentPtr> wf_templates_;

    /**
       The waste package components current being dealt with
     */
    #pragma cyclus var {"tooltip": "the current waste package components", \
                        "uilabel": "Current Waste Package", \
                        "uitype": "none", \
                        "doc": "the current waste package components"}
    std::deque<ComponentPtr> current_waste_packages_;

    /**
       The waste package components that have been emplaced
     */
    #pragma cyclus var {"tooltip": "the emplaced waste package components", \
                        "uilabel": "Emplaced Waste Package", \
                        "uitype": "none", \
                        "doc": "the emplaced waste package components"}
    std::deque<ComponentPtr> emplaced_waste_packages_;

    /**
       The waste form components
     */
    #pragma cyclus var {"tooltip": "the current waste form components", \
                        "uilabel": "Current Waste Form", \
                        "uitype": "none", \
                        "doc": "the current waste form components"}
    std::deque<ComponentPtr> current_waste_forms_;

    /**
       The buffer components
     */
    #pragma cyclus var {"tooltip": "the buffer components", \
                        "uilabel": "Buffer Components", \
                        "uitype": "none", \
                        "doc": "the buffer components"}
    std::deque<ComponentPtr> buffers_;

    /**
       The waste package component
     */
    #pragma cyclus var {"tooltip": "the waste pacakge component", \
                        "uilabel": "Waste Package Comp", \
                        "uitype": "none", \
                        "doc": "the waste pacakge component"}
    std::deque<ComponentPtr> waste_packages_;

    /**
       The waste form components
     */
    #pragma cyclus var {"tooltip": "the waste form component", \
                        "uilabel": "Waste Form Comp", \
                        "uitype": "none", \
                        "doc": "the waste form component"}
    std::deque<ComponentPtr> waste_forms_;

    /**
       Each commodity is associated with a waste form.
     */
    #pragma cyclus var {"tooltip": "commodity association with waste form", \
                        "uilabel": "Waste Form Map", \
                        "uitype": "none", \
                        "doc": "commodity association with waste form"}
    std::map<std::string, ComponentPtr> commod_wf_map_;

    /**
       Each waste form associated with a waste package.
     */
    #pragma cyclus var {"tooltip": "commodity association with waste package", \
                        "uilabel": "Waste Package Map", \
                        "uitype": "none", \
                        "doc": "commodity association with waste package"}
    std::map<std::string, ComponentPtr> wf_wp_map_;

    /**
       Make requests based on capacity
       
       @param time the time at which to make the requests. 
     */
    void makeRequests(int time);

    /**
       Emplace the waste
     */
    void emplaceWaste() ;
    
    /**
       Condition the waste
       
       @param waste_stream is the WasteStream object to be conditioned
       @return the form that has been loaded with the waste stream
     */
    ComponentPtr conditionWaste(WasteStream waste_stream) ;

    /**
       Package the waste
       
       @param waste_form is the loaded waste form to be packaged
       @return the package that has been loaded with the waste form
     */
    ComponentPtr packageWaste(ComponentPtr waste_form) ;

    /**
       Load the buffer with the waste
       
       @param waste_package is the package to load into the buffer
       @return the buffer that has been loaded with the waste package
     */
    ComponentPtr loadBuffer(ComponentPtr waste_package) ;

    /**
       Set the placement for the component within the repository
       
       @param comp the component to be placed
       @return comp the component once it has been placed
     */
    ComponentPtr setPlacement(ComponentPtr comp);

    /**
       Initializes the name and model type of the component
       
       @param qe is the QueryEngine object containing intialization info
       
       @return the initialized component
     */
    ComponentPtr initComponent(QueryEngine* qe) ;

    /**
       Do heat transport calculations. 

       @param time the timestep at which to transport the heat
     */
    void transportHeat(int time) ;

    /**
       Do nuclide transport calculations for each component, radially outward

       @param the_time the timestep at which to transport the nuclides
     */
    void transportNuclides(int the_time) ;

    /**
       Record the state of each component, radially outward

       @param the_time the timestep at which to record all states
       */
    void updateContaminantTable(int the_time) ;

public:

    /**
       places the known variable names and types into member_types_ and member_refs_
       maps

       @param name is the name given to this variable in the input grammar
       @param val is the val for the appropraite member variable
      */
    void mapVars(const char* name, boost::any val);

    /**
       Returns a boolean confirming whether the mat can be accepted at this time.

       @param mat the material whose acceptability is in question.

       @return mat_acceptable (true if acceptable, false otherwise.)
      */
    bool mat_acceptable(mat_rsrc_ptr mat);

    /**
       Sets t_lim_ the thermal limiting temperature [K]

       @param t_lim the temperature limit [K]
      */
    void set_t_lim(Temp t_lim);

    /**
       Returns the thermally limiting temperature [K]

       @return t_lim_ the temperature limit [K]
      */
     Temp t_lim(){return t_lim_;};

    /**
       Sets r_lim_ [m], the limiting radius at which t_lim_ is the limiting temp

       @param r_lim the limiting radius [m]
      */
    void set_r_lim(Radius r_lim);
 
    /**
       Returns the thermally limiting temperature [K]

       @return r_lim_ the limiting radius [m]
      */
    Radius r_lim(){return r_lim_;};

    /**
      This adds a row that uniquely defines this repository model
      */
    void addRowToParamsTable();

    /**
       get the commodity-specific capacity of the Cyder
       
       @param commod the commodity
     */
    double getCapacity(std::string commod) ;

    /**
       get the total mass of the stuff in the inventory
       
       @return the total mass of the processed materials in storage
     */
    double checkInventory();

    /**
       get the total mass of the stuff in the stocks
       
       @return the total mass of the raw materials in storage
     */
    double checkStocks();

    /**
      get the advective velocity [m/s] of water movement in the repository
     */
    double adv_vel(){return adv_vel_;};

/* ------------------- */ 

};

#endif

