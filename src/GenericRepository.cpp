/*! \file GenericRepository.cpp
    \brief Implements the GenericRepository class, the central class of Cyder 
    \author Kathryn D. Huff
 */

#include <string>
#include <deque>
#include <vector>

#include "GenericResource.h"
#include "CycException.h"
#include "Timer.h"
#include "Logger.h"
#include "GenericRepository.h"


/**
 * The GenericRepository class inherits from the FacilityModel class and is 
 * dynamically loaded by the Model class when requested.
 * 
 * This facility model is intended to calculate nuclide and heat metrics over
 * time in the repository. It will make appropriate requests for spent fuel 
 * which derive from heat- and perhaps dose- limited space availability. 
 *
 * BEGINNING OF SIMULATION
 * At the beginning of the simulation, this facility model loads the components 
 * within it, arranges them, and figures out its initial capacity for each 
 * heat or dose generating waste type it expects to accept. 
 *
 * TICK
 * Examining the stocks, materials recieved last month are emplaced.
 * The repository determines its current capacity for the first of the 
 * incommodities (waste classifications?) and requests as much of the 
 * incommodities that it can fit. The next incommodity is on the docket for next 
 * month. 
 *
 * TOCK
 * The repository passes the Tock radially outward through its components.
 *
 * (r = 0) -> -> -> -> -> -> -> ( r = R ) mat -> form -> package -> buffer -> 
 * barrier -> near -> far
 *
 * addResource
 * Only accept material.
 * Put the material in the stocks
 *
 * RECEIVE MESSAGE
 * reject it, I don't do messages.
 *
 *
 */

using boost::lexical_cast;

table_ptr GenericRepository::gr_params_table = new Table(
    "GenericRepositoryParams");

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericRepository::GenericRepository() {
  // initialize things that don't depend on the input
  stocks_ = std::deque< WasteStream >();
  inventory_ = std::deque< WasteStream >();
  waste_packages_ = std::deque< ComponentPtr >();
  waste_forms_ = std::deque< ComponentPtr >();
  far_field_ = ComponentPtr(new Component());

  is_full_ = false;
  mapVars("x", "FLOAT", &x_);
  mapVars("y", "FLOAT", &y_);
  mapVars("z", "FLOAT", &z_);
  mapVars("dx", "FLOAT", &dx_);
  mapVars("dy", "FLOAT", &dy_);
  mapVars("dz", "FLOAT", &dz_);
  mapVars("advective_velocity", "FLOAT", &adv_vel_);
  mapVars("capacity", "FLOAT", &capacity_);
  mapVars("inventorysize", "FLOAT", &inventory_size_);
  mapVars("lifetime", "FLOAT", &lifetime_);
  mapVars("startOperYear", "INTEGER", &start_op_yr_);
  mapVars("startOperMonth", "INTEGER", &start_op_mo_);
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::initModuleMembers(QueryEngine* qe) { 
  // initialize ordinary objects
  std::map<std::string, std::string>::iterator item;
  for (item = member_types_.begin(); item != member_types_.end(); item++) {
    if (item->second =="INTEGER"){
      *(static_cast<int*>(member_refs_[item->first])) = lexical_cast<int>(qe->getElementContent(item->first.c_str()));
    } else if (item->second == "FLOAT") {
      *(static_cast<double*>(member_refs_[item->first])) = lexical_cast<double>(qe->getElementContent(item->first.c_str()));
    } else {
      std::string err = "The ";
      err += item->second;
      err += " data type for variable: ";
      err += item->first;
      err += " is not yet supported by the GenericRepository.";
      LOG(LEV_ERROR,"GenRepoFac")<<err;;
      throw CycException(err);
    }
  }

  // The repository accepts any commodities designated waste.
  // This will be a list
  int n_incommodities = qe->nElementsMatchingQuery("incommodity");
  for (int i = 0; i < n_incommodities; i++) {
    in_commods_.push_back(qe->getElementContent("incommodity",i));
  }

  // get components
  int n_components = qe->nElementsMatchingQuery("component");
  QueryEngine* component_input;
  for (int i = 0; i < n_components; i++) {
    component_input = qe->queryElement("component",i);
    initComponent(component_input);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentPtr GenericRepository::initComponent(QueryEngine* qe){
  ComponentPtr toRet = ComponentPtr(new Component());
  // the component class initialization function will pass down the queryengine pointer
  toRet->initModuleMembers(qe);
  // all components have a name and a type
  ComponentType comp_type = toRet->type();

  // they will have allowed subcomponents (think russian doll)
  int n_sub_components;
  QueryEngine* allowed_commod;
  QueryEngine* allowed_wf;
  std::string allowed_commod_name;
  std::string allowed_wf_name; 

  switch(comp_type) {
    case BUFFER:
      buffer_template_ = toRet;
      // do the buffers have allowed waste package types?
      break;
    case FF:
      far_field_ = toRet;
      // does the far field have allowed buffer types?
      break;
    case WF:
      // get allowed waste commodities
      n_sub_components = qe->nElementsMatchingQuery("allowedcommod");
      for (int i=0; i<n_sub_components; i++) {
        allowed_commod_name = qe->getElementContent("allowedcommod",i);
        commod_wf_map_.insert(std::make_pair(allowed_commod_name, toRet));
      }
      wf_templates_.push_back(toRet);
      break;
    case WP:
      wp_templates_.push_back(toRet);
      // // get allowed waste forms
      n_sub_components = qe->nElementsMatchingQuery("allowedwf");
      for (int i=0; i<n_sub_components; i++) {
        allowed_wf_name = qe->getElementContent("allowedwf",i);
        //iterate through wf_templates_
        //for each wf_template
        for (std::deque< ComponentPtr >::iterator iter = wf_templates_.begin(); iter != 
            wf_templates_.end(); iter ++){
          if ((*iter)->name() == allowed_wf_name){
            wf_wp_map_.insert(std::make_pair(allowed_wf_name, wp_templates_.back()));
          }
        }
      }
      break;
    default:
      throw CycException("Unknown ComponentType enum value encountered."); }

  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::cloneModuleMembersFrom(FacilityModel* source)
{

  GenericRepository* src = dynamic_cast<GenericRepository*>(source);
  // copy variables specific to this model
  x_= src->x_;
  y_= src->y_;
  z_= src->z_;
  dx_= src->dx_;
  dy_= src->dy_;
  dz_= src->dz_;
  adv_vel_ = src->adv_vel_;
  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  in_commods_ = src->in_commods_;
  far_field_->copy(src->far_field_);
  buffer_template_ = src->buffer_template_;
  wp_templates_ = src->wp_templates_;
  wf_templates_ = src->wf_templates_;
  wf_wp_map_ = src->wf_wp_map_;
  commod_wf_map_ = src->commod_wf_map_;
  buffers_.push_front(ComponentPtr(new Component()));
  buffers_.front()->copy(buffer_template_);
  setPlacement(buffers_.front());

  // don't copy things that should start out empty
  // initialize empty structures instead
  stocks_ = std::deque< WasteStream >();
  inventory_ = std::deque< WasteStream >();
  waste_packages_ = std::deque< ComponentPtr >();
  waste_forms_ = std::deque< ComponentPtr >();
  is_full_ = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::copyFreshModel(Model* src)
{
  copy(dynamic_cast<GenericRepository*>(src));
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string GenericRepository::str() {
 
  // this should ultimately print all of the components loaded into this repository.
  std::stringstream fac_model_ss;
  fac_model_ss << FacilityModel::str(); 
  std::string gen_repo_msg;

  gen_repo_msg += "}, wf {";
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  gen_repo_msg += "}, wp {";
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  gen_repo_msg += "}, buffer {";
  for ( std::deque< ComponentPtr >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  if (NULL != far_field_){
    gen_repo_msg += "with far_field_ {" +  far_field_->name();
  }
  fac_model_ss << gen_repo_msg;
  return fac_model_ss.str();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::receiveMessage(msg_ptr msg)
{
  throw CycException("GenericRepository doesn't know what to do with a msg.");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::addResource(Transaction trans, std::vector<rsrc_ptr> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (std::vector<rsrc_ptr>::iterator this_rsrc=manifest.begin();
       this_rsrc != manifest.end();
       this_rsrc++)
  {
    LOG(LEV_DEBUG2, "GenRepoFac") <<"GenericRepository " << ID() << " is receiving material with mass "
        << (*this_rsrc)->quantity();
    if ((*this_rsrc)->type()==MATERIAL_RES){
      stocks_.push_front(std::make_pair(boost::dynamic_pointer_cast<Material>(*this_rsrc), trans.commod()));
    } else {
      std::string err = "The GenericRepository only accepts Material-type Resources.";
      throw CycException(err);
      LOG(LEV_ERROR, "GenRepoFac")<< err ;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::handleTick(int time)
{
  LOG(LEV_INFO3, "GenRepoFac") << facName() << " is ticking {";
  // if this is the first timestep, register the far field
  if (time==0){
    setPlacement(far_field_);
    addComponentToTable(far_field_);
  }

  // make requests
  makeRequests(time);
  LOG(LEV_INFO3, "GenRepoFac") << "}";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::handleTock(int time) {

  // emplace the waste that's ready
  emplaceWaste();

  // calculate the heat
  transportHeat(time);
  
  // calculate the nuclide transport
  transportNuclides(time);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::makeRequests(int time){

  // should this model make requests for all of the commodities it accepts?
  // there should be a section of the repository for each accepted commodity
 
  // right now it picks one commodity per month and asks for that.
  // It chooses the next incommodity in the preference lineup
  std::string in_commod;
  if(!in_commods_.empty()) {
    in_commod = in_commods_.front();

    // It then moves that commodity from the front to the back of the preference 
    // lineup
    in_commods_.push_back(in_commod);
    in_commods_.pop_front();
  
    // It can accept amounts however small
    double minAmt = 0;
    // this will be a request for free stuff
    double commod_price = 0;
    // It will need to figure out its capacity
    double requestAmt;
    // Perform the task of figuring out the capacity for this commod
    requestAmt = getCapacity(in_commod);
    
    // make requests
    if (requestAmt == 0){
      // don't request anything
    } else {
      MarketModel* market = MarketModel::marketForCommod(in_commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);
  
      // create a generic resource
      gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource("kg",in_commod,requestAmt));
  
      // build the transaction and message
      Transaction trans(this, REQUEST);
      trans.setCommod(in_commod);
      trans.setMinFrac(minAmt/requestAmt);
      trans.setPrice(commod_price);
      trans.setResource(request_res); 
  
      msg_ptr request = new Message(this, recipient, trans); 
      request->sendOn();
      LOG(LEV_INFO3, "GenRepoFac") << " requests " << requestAmt << " kg of " << in_commod << ".";
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GenericRepository::getCapacity(std::string commod){
  double toRet=0;
  // if the overall repo has a legislative limit, report it
  // eventually, this will report the commodity dependent capacity
  // The GenericRepository should ask for material unless it's full
  double inv = this->checkInventory();
  // including how much is already in its stocks
  double sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  double space = inventory_size_- inv - sto;
  // if empty space is less than monthly acceptance capacity
  if (space <= capacity_){
    toRet = space;
    // otherwise
  } else if (space >= capacity_){
    // the upper bound is the monthly acceptance capacity
    toRet = capacity_;
  } 
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GenericRepository::checkInventory(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.
  for (std::deque< WasteStream >::iterator iter = inventory_.begin(); iter != 
      inventory_.end(); iter ++){
    total += iter->first->quantity();
  }

  return total;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double GenericRepository::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if (!stocks_.empty()){
    for (std::deque< WasteStream >::iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++) {
        total += iter->first->quantity();
    };
  };
  return total;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::emplaceWaste(){
  // if there's anything in the stocks, try to emplace it
  if (!stocks_.empty()) {
    // for each waste stream in the stocks
    for (std::deque< WasteStream >::const_iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++) {
      // -- put the waste stream in the waste form
      // -- associate the waste stream with the waste form
      conditionWaste((*iter));
    }
    for (std::deque< ComponentPtr >::const_iterator iter = 
        current_waste_forms_.begin(); iter != current_waste_forms_.end(); iter 
        ++){
      // -- put the waste form in a waste package
      // -- associate the waste form with the waste package
      packageWaste((*iter));
    }
    int nwf = current_waste_forms_.size();
    for (int i=0; i < nwf; i++){
      waste_forms_.push_back(current_waste_forms_.front());
      current_waste_forms_.pop_front();
    }
    int nwp = current_waste_packages_.size();
    for (int i=0; i < nwp; i++){
      ComponentPtr iter = current_waste_packages_.front();
      // try to load each package in the current buffer 
      ComponentPtr current_buffer = buffers_.front();
      if (NULL == current_buffer) {
        std::string err_msg = "Buffers not yet loaded into Generic Repository.";
        throw CycException(err_msg);
      }
      // if the package is full
      if ( iter->isFull()
          // and not too hot
          //&& (*iter)->peak_temp(OUTER) <= current_buffer->temp_lim() or 
          //too toxic
          //&& (*iter)->peak_tox() <= current_buffer->tox_lim()
          ) {
        // emplace it in the buffer
        loadBuffer(iter);
        if ( current_buffer->isFull() ) {
          buffers_.push_back(buffers_.front());
          buffers_.pop_front();
          if ( buffers_.front()->isFull()){
            // all buffers are now full, capacity reached
            is_full_ = true;
          } else {
            setPlacement(buffers_.front());
          }
        }
        // take the waste package out of the current packagess
        waste_packages_.push_back(current_waste_packages_.front());
        current_waste_packages_.pop_front();
      }
      // if the waste package was either too hot or not full
      // push it back on the stack
      current_waste_packages_.push_back(current_waste_packages_.front());
      current_waste_packages_.pop_front();
      inventory_.push_back(stocks_.front());
      stocks_.pop_front();
      // once the waste is emplaced, is there anything else to do?
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentPtr GenericRepository::conditionWaste(WasteStream waste_stream){
  // figure out what waste form to put the waste stream in
  ComponentPtr chosen_wf_template;
  chosen_wf_template = commod_wf_map_[waste_stream.second];
  if (chosen_wf_template){
    std::string err_msg = "The commodity '";
    err_msg += waste_stream.second;
    err_msg +="' does not have a matching WF in the GenericRepository.";
    throw CycException(err_msg);
  }
  // if there doesn't already exist a partially full one
  // @todo check for partially full wf's before creating new one (katyhuff)
  // create that waste form
  current_waste_forms_.push_back(ComponentPtr(new Component()));
  current_waste_forms_.back()->copy(chosen_wf_template);
  // and load in the waste stream
  current_waste_forms_.back()->absorb(waste_stream.first);
  return current_waste_forms_.back();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentPtr GenericRepository::packageWaste(ComponentPtr waste_form){
  // figure out what waste package to put the waste form in
  bool loaded = false;
  ComponentPtr chosen_wp_template;
  std::string name = waste_form->name();
  chosen_wp_template = wf_wp_map_[name];
  if (chosen_wp_template == NULL){
    std::string err_msg = "The waste form '";
    err_msg += (waste_form)->name();
    err_msg +="' does not have a matching WP in the GenericRepository.";
    throw CycException(err_msg);
  }
  ComponentPtr toRet;
  // until the waste form has been loaded into a package
  while (!loaded){
    // look through the current waste packages 
    for (std::deque<ComponentPtr>::const_iterator iter= 
        current_waste_packages_.begin();
        iter != current_waste_packages_.end();
        iter++){
      // if there already exists an only partially full one of the right kind
      if ( !(*iter)->isFull() && (*iter)->name() == 
          chosen_wp_template->name()){
        // fill it
        (*iter)->load(WP, waste_form);
        toRet = (*iter);
        loaded = true;
      } }
    // if no currently unfilled waste packages match, create a new waste package
    current_waste_packages_.push_back(ComponentPtr( new Component() ));
    current_waste_packages_.back()->copy(chosen_wp_template);
    // and load in the waste form
    toRet = current_waste_packages_.back()->load(WP, waste_form); 
    loaded = true;
  }
  return toRet;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentPtr GenericRepository::loadBuffer(ComponentPtr waste_package){
  // figure out what buffer to put the waste package in
  ComponentPtr chosen_buffer = buffers_.front();
  // and load in the waste package
  buffers_.front()->load(BUFFER, waste_package);
  // put this on the stack of waste packages that have been emplaced
  emplaced_waste_packages_.push_back(waste_package);
  // set the location of the waste package 
  setPlacement(waste_package);
  addComponentToTable(waste_package);
  // set the location of the waste forms within the waste package
  std::vector<ComponentPtr> daughters = waste_package->daughters();
  for (std::vector<ComponentPtr>::iterator iter = daughters.begin();  
      iter != daughters.end(); 
      iter ++){
    setPlacement(*iter);
    addComponentToTable((*iter));
  }
  return buffers_.front();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentPtr GenericRepository::setPlacement(ComponentPtr comp){
  double x,y,z;
  // figure out what type of component it is
  switch(comp->type()) 
  {
    case FF :
      x = x_/2;
      y = y_/2;
      z = z_/2;
      break;
    case BUFFER :
      x = (buffers_.size()- .5)*dx_ ;
      y = y_/2 ; 
      z = dz_ ; 
      break;
    case WP :
      x = (comp->parent())->x();
      y = (emplaced_waste_packages_.size()*dy_ - dy_/2) ; 
      z = dz_ ; 
      break;
    case WF :
      x = (comp->parent())->x();
      y = (comp->parent())->y();
      z = (comp->parent())->z();
      break;
    default :
      std::string err = "ComponentType, '";
      err += comp->type();
      err +="' is not a valid type for Component ";
      err += comp->name();
      err += ".";
      throw CycException(err);
  }
  // figure out what buffer to put the waste package in
  point_t point = {x,y,z};
  comp->setPlacement(point);
  return comp; 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::transportHeat(int time){
  // update the thermal BCs everywhere
  // pass the transport heat signal through the components, inner -> outer
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportHeat(time);
  }
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportHeat(time);
  }
  for ( std::deque< ComponentPtr >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportHeat(time);
  }
  if (NULL != far_field_){
    far_field_->transportHeat(time);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::transportNuclides(int time){
  // update the nuclide transport BCs everywhere
  // pass the transport nuclides signal through the components, inner -> outer
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportNuclides(time);
  }
  for ( std::deque< ComponentPtr >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportNuclides(time);
  }
  for ( std::deque< ComponentPtr >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportNuclides(time);
  }
  if (NULL != far_field_){
    far_field_->transportNuclides(time);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::mapVars(std::string name, std::string type, void* ref) {
  member_types_.insert(std::make_pair( name , type));
  member_refs_.insert(std::make_pair( name , ref ));
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::makeParamsTable(){
  // declare the table columns
  std::vector<column> columns;
  columns.push_back(std::make_pair("facID", "INTEGER"));
  std::map<std::string, std::string>::iterator item;
  for (item = member_types_.begin(); item != member_types_.end(); item++){
    columns.push_back(std::make_pair(item->first, item->second));
  }
  // declare the table's primary key
  primary_key pk;
  pk.push_back("facID");
  gr_params_table->defineTable(columns,pk);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::defineComponentsTable(){

}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericRepository::addComponentToTable(ComponentPtr comp){

}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructGenericRepository() {
    return new GenericRepository();
}

extern "C" void destructGenericRepository(Model* p) {
    delete p;
}

/* ------------------- */ 

