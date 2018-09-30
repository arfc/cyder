// taken from cyclus/cycamore storage.cc
// conditioning.cc
// Implements the Conditioning class
#include "conditioning.h"

namespace conditioning {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Conditioning::Conditioning(cyclus::Context* ctx) 
    : cyclus::Facility(ctx),
      latitude(0.0),
      longitude(0.0),
      coordinates(latitude, longitude) {
  cyclus::Warn<cyclus::EXPERIMENTAL_WARNING>(
      "The Conditioning Facility is experimental.");};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// pragmas

#pragma cyclus def schema conditioning::Conditioning

#pragma cyclus def annotations conditioning::Conditioning

#pragma cyclus def initinv conditioning::Conditioning

#pragma cyclus def snapshotinv conditioning::Conditioning

#pragma cyclus def infiletodb conditioning::Conditioning

#pragma cyclus def snapshot conditioning::Conditioning

#pragma cyclus def clone conditioning::Conditioning

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::InitFrom(Conditioning* m) {
#pragma cyclus impl initfromcopy conditioning::Conditioning
  cyclus::toolkit::CommodityProducer::Copy(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::InitFrom(cyclus::QueryableBackend* b) {
#pragma cyclus impl initfromdb conditioning::Conditioning

  using cyclus::toolkit::Commodity;
  Commodity commod = Commodity(out_commods.front());
  cyclus::toolkit::CommodityProducer::Add(commod);
  cyclus::toolkit::CommodityProducer::SetCapacity(commod, throughput);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::EnterNotify() {
  cyclus::Facility::EnterNotify();
  buy_policy.Init(this, &inventory, std::string("inventory"));

  // dummy comp, use in_recipe if provided
  cyclus::CompMap v;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  if (in_recipe != "") {
    comp = context()->GetRecipe(in_recipe);
  }

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

  for (int i = 0; i != in_commods.size(); ++i) {
    buy_policy.Set(in_commods[i], comp, in_commod_prefs[i]);
  }
  buy_policy.Start();

  if (out_commods.size() == 1) {
    sell_policy.Init(this, &stocks, std::string("stocks"))
        .Set(out_commods.front())
        .Start();
  } else {
    std::stringstream ss;
    ss << "out_commods has " << out_commods.size() << " values, expected 1.";
    throw cyclus::ValueError(ss.str());
  }
  RecordPosition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Conditioning::str() {
  std::stringstream ss;
  std::string ans, out_str;
  if (out_commods.size() == 1) {
    out_str = out_commods.front();
  } else {
    out_str = "";
  }
  if (cyclus::toolkit::CommodityProducer::Produces(
          cyclus::toolkit::Commodity(out_str))) {
    ans = "yes";
  } else {
    ans = "no";
  }
  ss << cyclus::Facility::str();
  ss << " has facility parameters {"
     << "\n"
     << "     Output Commodity = " << out_str << ",\n"
     << "     Residence Time = " << residence_time << ",\n"
     << "     Throughput = " << throughput << ",\n"
     << " commod producer members: "
     << " produces " << out_str << "?:" << ans << "'}";
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::Tick() {
  // Set available capacity for Buy Policy
  inventory.capacity(current_capacity());

  LOG(cyclus::LEV_INFO3, "ComCnv") << prototype() << " is ticking {";

  if (current_capacity() > cyclus::eps_rsrc()) {
    LOG(cyclus::LEV_INFO4, "ComCnv")
        << " has capacity for " << current_capacity() << " kg of material.";
  }
  LOG(cyclus::LEV_INFO3, "ComCnv") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::Tock() {
  LOG(cyclus::LEV_INFO3, "ComCnv") << prototype() << " is tocking {";

  BeginProcessing_();  // place unprocessed inventory into processing
  PackageMatl_(package_size,package_properties);

  if (ready_time() >= 0 || residence_time == 0 && !inventory.empty()) {
    ReadyMatl_(ready_time());  // place processing into ready
  }

  ProcessMat_(throughput);  // place ready into stocks

  LOG(cyclus::LEV_INFO3, "ComCnv") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::BeginProcessing_() {
  while (inventory.count() > 0) {
    try {
      processing.Push(inventory.Pop());
      entry_times.push_back(context()->time());
      std::cout << "began processing" << std::endl;

      LOG(cyclus::LEV_DEBUG2, "ComCnv")
          << "Conditioning " << prototype()
          << " added resources to processing at t= " << context()->time();
    } catch (cyclus::Error& e) {
      e.msg(Agent::InformErrorMsg(e.msg()));
      throw e;
    }
  }
}

typedef std::map<std::string, std::map<std::string, int>> package_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::PackageMatl_(int pack_size, package_ package_prop) { // add package state variable, how to use fancy typedef 
  while (processing.count() >= pack_size) {
      // try a for loop 
    cyclus::PackagedMaterial::matstream temp_stream;
    double assem_quantity = 0; 
    for (int a = 1; a = pack_size; a = a + 1) {
      // pop a bunch of assemblies from processing to our temp stream 
      temp_stream.push_back(processing.Pop());
      assem_quantity += (processing.Peek()->quantity());
	// pop all entry times except the youngest material object 
	if (a = pack_size) {
	  pm_entry_times.push_back(context()->time());
	} 
      }
    // place that temp stream into our package_prop 
    cyclus::PackagedMaterial::package temp_package (temp_stream,package_prop);
    // somehow make sure that assem quantities are added together 

    // create a new packagedmaterial
    cyclus::PackagedMaterial::Ptr pm; 
    pm = cyclus::PackagedMaterial::Create(this, assem_quantity,temp_package);
    // add packagedmaterial into packaged resbuf 
    packaged.Push(pm);

  }
  std::cout << "packaged" << std::endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::ReadyMatl_(int time) {
  using cyclus::toolkit::ResBuf;

  int to_ready = 0;

  while (!pm_entry_times.empty() && pm_entry_times.front() <= time) {
    pm_entry_times.pop_front();
    ++to_ready;
  }

  ready.Push(packaged.PopN(to_ready));
  std::cout << "readyed" << std::endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Conditioning::ProcessMat_(double cap) {
// move ready to stocks based on throughput 

  if (!ready.empty()) {
    try {
      double count_num = ready.count(); 
      double max_pop = std::min(cap, count_num);
      int cap_pop = 0; 
      while (cap_pop < max_pop){
        stocks.Push(ready.Pop());
        cap_pop = cap_pop+1;
      }

      LOG(cyclus::LEV_INFO1, "ComCnv") << "Conditioning " << prototype()
                                       << " moved resources"
                                       << " from ready to stocks"
                                       << " at t= " << context()->time();
    } catch (cyclus::Error& e) {
      e.msg(Agent::InformErrorMsg(e.msg()));
      throw e;
    }
  }
  std::cout << "processed" << std::endl;

}


void Conditioning::RecordPosition() {
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


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Agent* ConstructConditioning(cyclus::Context* ctx) {
  return new Conditioning(ctx);
}

}  // namespace conditioning
