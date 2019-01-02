// Implements the Sink class
#include <algorithm>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "pmsink.h"

namespace cyder {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PmSink::PmSink(cyclus::Context* ctx)
    : cyclus::Facility(ctx),
      capacity(std::numeric_limits<double>::max()),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {
  SetMaxInventorySize(std::numeric_limits<double>::max());}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PmSink::~PmSink() {}

#pragma cyclus def schema cyder::PmSink

#pragma cyclus def annotations cyder::PmSink

#pragma cyclus def infiletodb cyder::PmSink

#pragma cyclus def snapshot cyder::PmSink

#pragma cyclus def snapshotinv cyder::PmSink

#pragma cyclus def initinv cyder::PmSink

#pragma cyclus def clone cyder::PmSink

#pragma cyclus def initfromdb cyder::PmSink

#pragma cyclus def initfromcopy cyder::PmSink

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::EnterNotify() {
  cyclus::Facility::EnterNotify();

  if (in_commod_prefs.size() == 0) {
    for (int i = 0; i < in_commods.size(); ++i) {
      in_commod_prefs.push_back(cyclus::kDefaultPref);
    }
  } else if (in_commod_prefs.size() != in_commods.size()) {
    std::stringstream ss;
    ss << "in_commod_prefs has " << in_commod_prefs.size()
       << " values, expected " << in_commods.size();
    throw cyclus::ValueError(ss.str());
  }
  RecordPosition();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string PmSink::str() {
  using std::string;
  using std::vector;
  std::stringstream ss;
  ss << cyclus::Facility::str();

  string msg = "";
  msg += "accepts commodities ";
  for (vector<string>::iterator commod = in_commods.begin();
       commod != in_commods.end();
       commod++) {
    msg += (commod == in_commods.begin() ? "{" : ", ");
    msg += (*commod);
  }
  msg += "} until its inventory is full at ";
  ss << msg << inventory.capacity() << " kg.";
  return "" + ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
PmSink::GetMatlRequests() {
  using cyclus::Material;
  using cyclus::RequestPortfolio;
  using cyclus::Request;
  using cyclus::Composition;
  
  std::cout << "material requests" << std::endl;

  std::set<RequestPortfolio<Material>::Ptr> ports;
  RequestPortfolio<Material>::Ptr port(new RequestPortfolio<Material>());
  double amt = RequestAmt();
  Material::Ptr mat;

  if (recipe_name.empty()) {
    mat = cyclus::NewBlankMaterial(amt);
  } else {
    Composition::Ptr rec = this->context()->GetRecipe(recipe_name);
    mat = cyclus::Material::CreateUntracked(amt, rec); 
  } 

  if (amt > cyclus::eps()) {
    std::vector<Request<Material>*> mutuals;
    for (int i = 0; i < in_commods.size(); i++) {
      mutuals.push_back(port->AddRequest(mat, this, in_commods[i], in_commod_prefs[i]));
      
    }
    port->AddMutualReqs(mutuals);
    ports.insert(port);
  }  // if amt > eps
  return ports;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::Product>::Ptr>
PmSink::GetProductRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::Product;
  using cyclus::RequestPortfolio;
  using cyclus::Request;

  std::cout << "product requests" << std::endl;
  
  std::set<RequestPortfolio<Product>::Ptr> ports;
  RequestPortfolio<Product>::Ptr
      port(new RequestPortfolio<Product>());
  double amt = RequestAmt();

  if (amt > cyclus::eps()) {
    CapacityConstraint<Product> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods.begin(); it != in_commods.end(); ++it) {
      std::string quality = "";  // not clear what this should be..
      Product::Ptr rsrc = Product::CreateUntracked(amt, quality);
      port->AddRequest(rsrc, this, *it);
    }

    ports.insert(port);
  }  // if amt > eps
  return ports;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::set<cyclus::RequestPortfolio<cyclus::PackagedMaterial>::Ptr>
PmSink::GetPackagedMatlRequests() {
  using cyclus::CapacityConstraint;
  using cyclus::PackagedMaterial;
  using cyclus::RequestPortfolio;
  using cyclus::Request;
  
  std::cout << "packaged material requests" << std::endl;

  std::set<RequestPortfolio<PackagedMaterial>::Ptr> ports;
  RequestPortfolio<PackagedMaterial>::Ptr
      port(new RequestPortfolio<PackagedMaterial>());
  double amt = RequestAmt();

  if (amt > cyclus::eps()) {
    CapacityConstraint<PackagedMaterial> cc(amt);
    port->AddConstraint(cc);

    std::vector<std::string>::const_iterator it;
    for (it = in_commods.begin(); it != in_commods.end(); ++it) {
      cyclus::PackagedMaterial::package quality; 
      PackagedMaterial::Ptr rsrc = PackagedMaterial::CreateUntracked(amt, quality);
      port->AddRequest(rsrc, this, *it);
    }

    ports.insert(port);
  }  // if amt > eps
  return ports;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::AcceptMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Material>,
                                 cyclus::Material::Ptr> >& responses) {
  std::cout << "matl trades" << std::endl;
  std::vector< std::pair<cyclus::Trade<cyclus::Material>,
                         cyclus::Material::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::AcceptProductTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::Product>,
                                 cyclus::Product::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::Product>,
                         cyclus::Product::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::AcceptPackagedMatlTrades(
    const std::vector< std::pair<cyclus::Trade<cyclus::PackagedMaterial>,
                                 cyclus::PackagedMaterial::Ptr> >& responses) {
  std::vector< std::pair<cyclus::Trade<cyclus::PackagedMaterial>,
                         cyclus::PackagedMaterial::Ptr> >::const_iterator it;
  for (it = responses.begin(); it != responses.end(); ++it) {
    inventory.Push(it->second);
  }
  std::cout << "packaged material trades" << std::endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::Tick() {
  using std::string;
  using std::vector;
  LOG(cyclus::LEV_INFO3, "SnkFac") << prototype() << " is ticking {";

  std::cout << "pm sink tick" << std::endl;
  std::cout << inventory.quantity() << std::endl;

  double requestAmt = RequestAmt();
  // inform the simulation about what the sink facility will be requesting
  if (requestAmt > cyclus::eps()) {
    for (vector<string>::iterator commod = in_commods.begin();
         commod != in_commods.end();
         commod++) {
      LOG(cyclus::LEV_INFO4, "SnkFac") << " will request " << requestAmt
                                       << " kg of " << *commod << ".";
      cyclus::toolkit::RecordTimeSeries<double>("demand"+*commod, this, 
                                            requestAmt);
    }
  }
  LOG(cyclus::LEV_INFO3, "SnkFac") << "}";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void PmSink::Tock() {
  LOG(cyclus::LEV_INFO3, "SnkFac") << prototype() << " is tocking {";

  //std::cout << "pm sink tock" << std::endl;
  
  // On the tock, the sink facility doesn't really do much.
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(cyclus::LEV_INFO4, "SnkFac") << "PmSink " << this->id()
                                   << " is holding " << inventory.quantity()
                                   << " units of material at the close of month "
                                   << context()->time() << ".";
  LOG(cyclus::LEV_INFO3, "SnkFac") << "}";
}

void PmSink::RecordPosition() {
  std::string specification = this->spec();
  context()
      ->NewDatum("AgentPosition")
      ->AddVal("Spec", specification)
      ->AddVal("Prototype", this->prototype())
      ->AddVal("AgentId", id())
      ->AddVal("Latitude", latitude)
      ->AddVal("Longitude", longitude)
      ->Record();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructPmSink(cyclus::Context* ctx) {
  return new PmSink(ctx);
}

}  // namespace cyder
