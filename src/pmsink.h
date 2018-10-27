#ifndef CYDER_SRC_SINK_H_
#define CYDER_SRC_SINK_H_

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "cyclus.h"
#include "cyder_version.h"

namespace cyder {

class Context;

/// This facility acts as a pmsink of materials and products with a fixed
/// throughput (per time step) capacity and a lifetime capacity defined by a
/// total inventory size.  The inventory size and throughput capacity both
/// default to infinite. If a recipe is provided, it will request material with
/// that recipe. Requests are made for any number of specified commodities.
class PmSink 
  : public cyclus::Facility,
    public cyclus::toolkit::Position  {
 public:
  PmSink(cyclus::Context* ctx);

  virtual ~PmSink();

  virtual std::string version() { return CYDER_VERSION; }

  #pragma cyclus note { \
    "doc": \
    " A pmsink facility that accepts materials and products with a fixed\n"\
    " throughput (per time step) capacity and a lifetime capacity defined by\n"\
    " a total inventory size. The inventory size and throughput capacity\n"\
    " both default to infinite. If a recipe is provided, it will request\n"\
    " material with that recipe. Requests are made for any number of\n"\
    " specified commodities.\n" \
    }

  #pragma cyclus decl

  virtual std::string str();

  virtual void EnterNotify();
  
  virtual void Tick();

  virtual void Tock();

  /// @brief PmSinkFacilities request Materials of their given commodity. Note
  /// that it is assumed the PmSink operates on a single resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests();

  /// @brief PmSinkFacilities request Products of their given
  /// commodity. Note that it is assumed the PmSink operates on a single
  /// resource type!
  virtual std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
      GetGenRsrcRequests();

  /// @brief PmSinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptMatlTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
      cyclus::Material::Ptr> >& responses);

  /// @brief PmSinkFacilities place accepted trade Materials in their Inventory
  virtual void AcceptGenRsrcTrades(
      const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
      cyclus::Product::Ptr> >& responses);

  ///  add a commodity to the set of input commodities
  ///  @param name the commodity name
  inline void AddCommodity(std::string name) { in_commods.push_back(name); }

  ///  sets the size of the storage inventory for received material
  ///  @param size the storage size
  inline void SetMaxInventorySize(double size) {
    max_inv_size = size;
    inventory.capacity(size);
  }

  /// @return the maximum inventory storage size
  inline double MaxInventorySize() const { return inventory.capacity(); }

  /// @return the current inventory storage size
  inline double InventorySize() const { return inventory.quantity(); }

  /// determines the amount to request
  inline double RequestAmt() const {
    return std::min(capacity, std::max(0.0, inventory.space()));
  }

  /// sets the capacity of a material generated at any given time step
  /// @param capacity the reception capacity
  inline void Capacity(double cap) { capacity = cap; }

  /// @return the reception capacity at any given time step
  inline double Capacity() const { return capacity; }

  /// @return the input commodities
  inline const std::vector<std::string>&
      input_commodities() const { return in_commods; }

  /// @return the input commodities preferences
  inline const std::vector<double>&
      input_commodity_preferences() const { return in_commod_prefs; }

 private:
  /// all facilities must have at least one input commodity
  #pragma cyclus var {"tooltip": "input commodities", \
                      "doc": "commodities that the pmsink facility accepts", \
                      "uilabel": "List of Input Commodities", \
                      "uitype": ["oneormore", "incommodity"]}
  std::vector<std::string> in_commods;

  #pragma cyclus var {"default": [],\
                      "doc":"preferences for each of the given commodities, in the same order."\
                      "Defauts to 1 if unspecified",\
                      "uilabel":"In Commody Preferences", \
                      "range": [None, [1e-299, 1e299]], \
                      "uitype":["oneormore", "range"]}
  std::vector<double> in_commod_prefs;

  #pragma cyclus var {"default": "", \
                      "tooltip": "requested composition", \
                      "doc": "name of recipe to use for material requests, " \
                             "where the default (empty string) is to accept " \
                             "everything", \
                      "uilabel": "Input Recipe", \
                      "uitype": "inrecipe"}
  std::string recipe_name;

  /// max inventory size
  #pragma cyclus var {"default": 1e299, \
                      "tooltip": "pmsink maximum inventory size", \
                      "uilabel": "Maximum Inventory", \
                      "uitype": "range", \
                      "range": [0.0, 1e299], \
                      "doc": "total maximum inventory size of pmsink facility"}
  double max_inv_size;

  /// monthly acceptance capacity
  #pragma cyclus var {"default": 1e299, \
                      "tooltip": "pmsink capacity", \
                      "uilabel": "Maximum Throughput", \
                      "uitype": "range", \
                      "range": [0.0, 1e299], \
                      "doc": "capacity the pmsink facility can " \
                             "accept at each time step"}
  double capacity;

  /// this facility holds material in storage.
  #pragma cyclus var {'capacity': 'max_inv_size'}
  cyclus::toolkit::ResBuf<cyclus::Resource> inventory;

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
};

}  // namespace cyder

#endif  // CYDER_SRC_SINK_H_
