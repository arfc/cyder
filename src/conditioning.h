/// Taken from cyclus/cycamore storage.h file 

#ifndef CYCLUS_CONDITIONING_CONDITIONING_H_
#define CYCLUS_CONDITIONING_CONDITIONING_H_

#include <string>
#include <list>
#include <vector>

#include "cyclus.h"
#include "cyder_version.h"

// forward declaration
namespace conditioning {
class Conditioning;
} // namespace conditioning


namespace conditioning {
/// @class Conditioning
///
/// This Facility is intended to hold materials for a user specified
/// amount of time in order to model a conditioning facility with a certain
/// residence time or holdup time.
/// The Conditioning class inherits from the Facility class and is
/// dynamically loaded by the Agent class when requested.
///
/// @section intro Introduction
/// This Agent was initially developed to support the fco code-to-code 
/// comparison.
/// It's very similar to the "NullFacility" of years 
/// past. Its purpose is to hold materials and release them only  
/// after some period of delay time.
///
/// @section agentparams Agent Parameters
/// in_commods is a vector of strings naming the commodities that this facility receives
/// out_commods is a string naming the commodity that in_commod is stocks into
/// residence_time is the minimum number of timesteps between receiving and offering
/// in_recipe (optional) describes the incoming resource by recipe
/// 
/// @section optionalparams Optional Parameters
/// max_inv_size is the maximum capacity of the inventory conditioning
/// throughput is the maximum processing capacity per timestep
///
/// @section detailed Detailed Behavior
/// 
/// Tick:
/// Nothing really happens on the tick. 
///
/// Tock:
/// On the tock, any material that has been waiting for long enough (delay 
/// time) is placed in the stocks buffer.
///
/// Any brand new inventory that was received in this timestep is placed into 
/// the processing queue to begin waiting. 
/// 
/// Making Requests:
/// This facility requests all of the in_commod that it can.
///
/// Receiving Resources:
/// Anything of the in_commod that is received by this facility goes into the 
/// inventory.
///
/// Making Offers:
/// Any stocks material in the stocks buffer is offered to the market.
///
/// Sending Resources:
/// Matched resources are sent immediately.
class Conditioning 
  : public cyclus::Facility,
    public cyclus::toolkit::CommodityProducer,
    public cyclus::toolkit::Position {
 public:  
  /// @param ctx the cyclus context for access to simulation-wide parameters
  Conditioning(cyclus::Context* ctx);
  
  #pragma cyclus decl

  #pragma cyclus note {"doc": "Conditioning is a simple facility which accepts any number of commodities " \
                              "and holds them for a user specified amount of time. The commodities accepted "\
                              "are chosen based on the specified preferences list. Once the desired amount of material "\
                              "has entered the facility it is passed into a 'processing' buffer where it is held until "\
                              "the residence time has passed. The material is then passed into a 'ready' buffer where it is "\
                              "queued for removal. Currently, all input commodities are lumped into a single output commodity. "\
                              "Conditioning also has the functionality to handle materials in discrete or continuous batches. Discrete "\
                              "mode, which is the default, does not split or combine material batches. Continuous mode, however, "\
                              "divides material batches if necessary in order to push materials through the facility as quickly "\
                              "as possible."}

  /// A verbose printer for the Conditioning Facility
  virtual std::string str();

  // --- Facility Members ---
  
  // --- Agent Members ---
  /// Sets up the Conditioning Facility's trade requests
  virtual void EnterNotify();

  /// The handleTick function specific to the Conditioning.
  virtual void Tick();

  /// The handleTick function specific to the Conditioning.
  virtual void Tock();

 protected:
  /// @brief Move all unprocessed inventory to processing
  void BeginProcessing_();

/// @brief move ready resources from processing to packaged after repackaging
<<<<<<< HEAD
  /// @param size_package (number of fuel bundles accepted in package), package_properties 
  void PackageMatl_(int size_package,std::map<std::string, std::map<std::string, double>> package_properties);
=======
  /// @param *** ADD HERE ***
  void PackageMatl_(int size_package, std::map<std::string, std::map<std::string, double>> package_properties);
>>>>>>> 64994879d5f1cfd95b8f922cb36ee9fb47c8389a

  /// @brief move ready resources from packaged to ready at a certain time
  /// @param time the time of interest
  void ReadyMatl_(int time);

  /// @brief Move as many ready resources as allowable into stocks
  /// @param cap current throughput capacity 
  void ProcessMat_(double cap);

    /* --- Conditioning Members --- */

  /// @brief current maximum amount that can be added to processing
  inline double current_capacity() const { 
    return (max_inv_size - processing.quantity() - stocks.quantity()); }

  /// @brief returns the time key for ready materials
  int ready_time(){ return context()->time() - residence_time; }

  /* --- Module Members --- */

  #pragma cyclus var {"tooltip":"input commodity",\
                      "doc":"commodities accepted by this facility",\
                      "uilabel":"Input Commodities",\
                      "uitype":["oneormore","incommodity"]}
  std::vector<std::string> in_commods;

  #pragma cyclus var {"default": [],\
                      "doc":"preferences for each of the given commodities, in the same order."\
                      "Defauts to 1 if unspecified",\
                      "uilabel":"In Commody Preferences", \
                      "range": [None, [1e-299, 1e299]], \
                      "uitype":["oneormore", "range"]}
  std::vector<double> in_commod_prefs;

  #pragma cyclus var {"tooltip":"output commodity",\
                      "doc":"commodity produced by this facility. Multiple commodity tracking is"\
                      " currently not supported, one output commodity catches all input commodities.",\
                      "uilabel":"Output Commodities",\
                      "uitype":["oneormore","outcommodity"]}
  std::vector<std::string> out_commods;

  #pragma cyclus var {"default":"",\
                      "tooltip":"input recipe",\
                      "doc":"recipe accepted by this facility, if unspecified a dummy recipe is used",\
                      "uilabel":"Input Recipe",\
                      "uitype":"inrecipe"}
  std::string in_recipe;

  #pragma cyclus var {"default": 0,\
                      "tooltip":"residence time (timesteps)",\
                      "doc":"the minimum holding time for a received commodity (timesteps).",\
                      "units":"time steps",\
                      "uilabel":"Residence Time", \
                      "uitype": "range", \
                      "range": [0, 12000]}
  int residence_time;

  #pragma cyclus var {"default": 1e299,\
                     "tooltip":"throughput per timestep",\
                     "doc":"the max number of waste canisters that can be moved through the facility per timestep",\
                     "uilabel":"Throughput",\
                     "uitype": "range", \
                     "range": [0.0, 1e299], \
                     "units":""}
  double throughput;

  #pragma cyclus var {"default": 10,\
                     "tooltip":"no. of spent fuel bundles in each canister",\
                     "doc":"the number of spent fuel bundles accepted in each waste canister",\
                     "uilabel":"Package Size",\
                     "uitype": "range", \
                     "range": [0, 100], \
                     "units":""}
  int package_size;

  #pragma cyclus var {"default": 1e299,\
                      "tooltip":"maximum inventory size (kg)",\
                      "doc":"the maximum amount of material that can be in all conditioning buffer stages",\
                      "uilabel":"Maximum Inventory Size",\
                      "uitype": "range", \
                      "range": [0.0, 1e299], \
                      "units":"kg"}
  double max_inv_size; 

  #pragma cyclus var {\
                      "alias": ["packageproperties","layer",["properties","property","value"]],\
                      "uitype":["oneormore","string",["oneormore","string","double"]],\
                      "tooltip":"packaged material properties ",\
                      "doc":"packaged material properties such as ",\
                      "uilabel":"properties"}
  std::map<std::string, std::map<std::string, double>> package_properties;                    

  #pragma cyclus var {"tooltip":"Incoming material buffer"}
  cyclus::toolkit::ResBuf<cyclus::Material> inventory;

  #pragma cyclus var {"tooltip":"Output material buffer"}
  cyclus::toolkit::ResBuf<cyclus::PackagedMaterial> stocks;

  #pragma cyclus var {"tooltip":"Buffer for material held for required residence_time"}
  cyclus::toolkit::ResBuf<cyclus::PackagedMaterial> ready;

  //// list of input times for materials entering the processing buffer
  #pragma cyclus var{"default": [],\
                      "internal": True}
  std::list<int> entry_times;

  //// list of input times for youngest material in a packagedmaterial
  #pragma cyclus var{"default": [],\
                      "internal": True}
  std::list<int> pm_entry_times;

  #pragma cyclus var {"tooltip":"Buffer for material still waiting for required residence_time"}
  cyclus::toolkit::ResBuf<cyclus::Material> processing;

  #pragma cyclus var {"tooltip":"Buffer for material that just got packaged and are still waiting for required residence time "}
  cyclus::toolkit::ResBuf<cyclus::PackagedMaterial> packaged;

  //// A policy for requesting material
  cyclus::toolkit::MatlBuyPolicy buy_policy;

  //// A policy for sending material
  cyclus::toolkit::PackagedMatlSellPolicy sell_policy;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical latitude in degrees as a double", \
    "doc": "Latitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double latitude;

  #pragma cyclus var { \
    "default": 0.0, \
    "uilabel": "Geographical longitude in degrees as a double", \
    "doc": "Longitude of the agent's geographical position. The value should " \
           "be expressed in degrees as a double." \
  }
  double longitude;

  cyclus::toolkit::Position coordinates;

  void RecordPosition();

  friend class ConditioningTest;
};

}  // namespace conditioning

#endif  // CYCLUS_CONDITIONING_CONDITIONING_H_
