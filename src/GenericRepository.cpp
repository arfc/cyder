/*! \file GenericRepository.cpp
    \brief Implements the GenericRepository class, the central class of Cyder 
    \author Kathryn D. Huff
 */
#include "Logger.h"

#include "GenericRepository.h"

#include <string>
#include <deque>
#include <vector>
#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"


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

table_ptr GenericRepository::gr_params_table = new Table("GenericRepositoryParams");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
GenericRepository::GenericRepository() {
  // initialize things that don't depend on the input
  stocks_ = std::deque< WasteStream >();
  inventory_ = std::deque< WasteStream >();
  waste_packages_ = std::deque< Component* >();
  waste_forms_ = std::deque< Component* >();
  far_field_ = new Component(); 
  is_full_ = false;
  mapVars("x","FLOAT", &x_) ;
  mapVars("y","FLOAT", &y_) ;
  mapVars("z","FLOAT", &z_) ;
  mapVars("dx","FLOAT", &dx_) ;
  mapVars("dy","FLOAT", &dy_) ;
  mapVars("dz","FLOAT", &dz_) ;
  mapVars("capacity","FLOAT", &capacity_) ;
  mapVars("inventorysize","FLOAT", &inventory_size_) ;
  mapVars("lifetime","FLOAT", &lifetime_) ;
  mapVars("startOperYear","INTEGER", &start_op_yr_) ;
  mapVars("startOperMonth","INTEGER", &start_op_mo_) ;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::init(xmlNodePtr cur)
{ FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/GenericRepository");

  // initialize ordinary objects
  std::map<std::string,std::string>::iterator item;
  for(item = member_types_.begin(); item != member_types_.end(); item++) {
    if(item->second =="INTEGER"){
      *(static_cast<int*>(member_refs_[item->first])) = strtol(XMLinput->get_xpath_content(cur,item->first.c_str()), NULL, 10);
    } else if (item->second == "FLOAT") {
      *(static_cast<double*>(member_refs_[item->first])) = strtod(XMLinput->get_xpath_content(cur,item->first.c_str()), NULL);
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
  std::string commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++) {
    commod = (const char*)(nodes->nodeTab[i]->children->content);
    in_commods_.push_back(commod);
  }

  // get components
  initComponents(cur);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::initComponents(xmlNodePtr cur) {
  Component* new_comp;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"component");
  // first, initialize the waste forms.
  for (int i=0;i<nodes->nodeNr;i++) {
    xmlNodePtr comp_node = nodes->nodeTab[i];
    if (new_comp->getComponentType(XMLinput->get_xpath_content(comp_node,"componenttype")) == WF){
      this->initComponent(comp_node);
    }
  }
  // now, initialize the rest 
  for (int i=0;i<nodes->nodeNr;i++) {
    xmlNodePtr comp_node = nodes->nodeTab[i];
    if (new_comp->getComponentType(XMLinput->get_xpath_content(comp_node,"componenttype")) 
       != WF){
      this->initComponent(comp_node);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Component* GenericRepository::initComponent(xmlNodePtr cur){
  Component* toRet = new Component();
  // the component class initialization function will pass down the xml pointer
  toRet->init(cur);

  // all components have a name and a type
  std::string comp_type = XMLinput->get_xpath_content(cur,"componenttype");

  // they will have allowed subcomponents (think russian doll)
  xmlNodeSetPtr allowed_sub_nodes;
  std::string allowed_commod;

  switch(toRet->getComponentType(comp_type)) {
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
      allowed_sub_nodes = XMLinput->get_xpath_elements(cur,"allowedcommod");
      for (int i=0;i<allowed_sub_nodes->nodeNr;i++) {
        allowed_commod = (const char*)(allowed_sub_nodes->nodeTab[i]->children->content);
        commod_wf_map_.insert(std::make_pair(allowed_commod, toRet));
      }
      wf_templates_.push_back(toRet);
      break;
    case WP:
      wp_templates_.push_back(toRet);
      // // get allowed waste forms
      allowed_sub_nodes = XMLinput->get_xpath_elements(cur,"allowedwf");
      for (int i=0;i<allowed_sub_nodes->nodeNr;i++) {
        std::string allowed_wf_name = (const 
            char*)(allowed_sub_nodes->nodeTab[i]->children->content);
        //iterate through wf_templates_
        //for each wf_template_
        for (std::deque< Component* >::iterator iter = wf_templates_.begin(); iter != 
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericRepository::copy(GenericRepository* src)
{

  // copy facility level stuff
  FacilityModel::copy(src);

  // copy variables specific to this model
  capacity_ = src->capacity_;
  x_= src->x_;
  y_= src->y_;
  z_= src->z_;
  dx_= src->dx_;
  dy_= src->dy_;
  dz_= src->dz_;
  inventory_size_ = src->inventory_size_;
  start_op_yr_ = src->start_op_yr_;
  start_op_mo_ = src->start_op_mo_;
  in_commods_ = src->in_commods_;
  far_field_ = new Component();
  far_field_->copy(src->far_field_);
  buffer_template_ = src->buffer_template_;
  wp_templates_ = src->wp_templates_;
  wf_templates_ = src->wf_templates_;
  wf_wp_map_ = src->wf_wp_map_;
  commod_wf_map_ = src->commod_wf_map_;
  buffers_.push_front(new Component());
  buffers_.front()->copy(buffer_template_);
  setPlacement(buffers_.front());

  // don't copy things that should start out empty
  // initialize empty structures instead
  stocks_ = std::deque< WasteStream >();
  inventory_ = std::deque< WasteStream >();
  waste_packages_ = std::deque< Component* >();
  waste_forms_ = std::deque< Component* >();
  is_full_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::copyFreshModel(Model* src)
{
  copy(dynamic_cast<GenericRepository*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string GenericRepository::str() {
 
  // this should ultimately print all of the components loaded into this repository.
  std::stringstream fac_model_ss;
  fac_model_ss << FacilityModel::str(); 
  std::string gen_repo_msg;

  gen_repo_msg += "}, wf {";
  for( std::deque< Component* >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  gen_repo_msg += "}, wp {";
  for( std::deque< Component* >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  gen_repo_msg += "}, buffer {";
  for( std::deque< Component* >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    gen_repo_msg += (*iter)->name();
  }
  if(NULL != far_field_){
    gen_repo_msg += "with far_field_ {" +  far_field_->name();
  }
  fac_model_ss << gen_repo_msg;
  return fac_model_ss.str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::receiveMessage(msg_ptr msg)
{
  throw CycException("GenericRepository doesn't know what to do with a msg.");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::addResource(Transaction trans, std::vector<rsrc_ptr> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (std::vector<rsrc_ptr>::iterator this_rsrc=manifest.begin();
       this_rsrc != manifest.end();
       this_rsrc++)
  {
    LOG(LEV_DEBUG2, "GenRepoFac") <<"GenericRepository " << ID() << " is receiving material with mass "
        << (*this_rsrc)->quantity();
    if((*this_rsrc)->type()==MATERIAL_RES){
      stocks_.push_front(std::make_pair(boost::dynamic_pointer_cast<Material>(*this_rsrc), trans.commod()));
    } else {
      std::string err = "The GenericRepository only accepts Material-type Resources.";
      throw CycException(err);
      LOG(LEV_ERROR, "GenRepoFac")<< err ;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTick(int time)
{
  // if this is the first timestep, register the far field
  if (time==0){
    setPlacement(far_field_);
    addComponentToTable(far_field_);
  }

  // make requests
  makeRequests(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::handleTock(int time) {

  // emplace the waste that's ready
  emplaceWaste();

  // calculate the heat
  transportHeat();
  
  // calculate the nuclide transport
  transportNuclides();
  
  // call the facility model's handle tock last 
  // to check for decommissioning
  FacilityModel::handleTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::makeRequests(int time){

  // should this model make requests for all of the commodities it accepts?
  // there should be a section of the repository for each accepted commodity
 
  // right now it picks one commodity per month and asks for that.
  // It chooses the next incommodity in the preference lineup
  std::string in_commod;
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
    GenericResource* request_res = new GenericResource(in_commod,"kg",requestAmt);

    // build the transaction and message
    Transaction trans(this, REQUEST);
    trans.setCommod(in_commod);
    trans.setMinFrac(minAmt/requestAmt);
    trans.setPrice(commod_price);
    trans.setResource(request_res); 

    Message* request = new Message(this, recipient, trans); 
    request->sendOn();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double GenericRepository::getCapacity(std::string commod){
  double toRet;
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
  } return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
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
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double GenericRepository::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (std::deque< WasteStream >::iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++) {
        total += iter->first->quantity();
    };
  };
  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::emplaceWaste(){
  // if there's anything in the stocks, try to emplace it
  if(!stocks_.empty()) {
    // for each waste stream in the stocks
    for (std::deque< WasteStream >::const_iterator iter = stocks_.begin(); iter != 
        stocks_.end(); iter ++) {
      // -- put the waste stream in the waste form
      // -- associate the waste stream with the waste form
      conditionWaste((*iter));
    }
    for (std::deque< Component* >::const_iterator iter = 
        current_waste_forms_.begin(); iter != current_waste_forms_.end(); iter 
        ++){
      // -- put the waste form in a waste package
      // -- associate the waste form with the waste package
      packageWaste((*iter));
    }
    int nwf = current_waste_forms_.size();
    for(int i=0; i < nwf; i++){
      waste_forms_.push_back(current_waste_forms_.front());
      current_waste_forms_.pop_front();
    }
    int nwp = current_waste_packages_.size();
    for(int i=0; i < nwp; i++){
      Component* iter = current_waste_packages_.front();
      // try to load each package in the current buffer 
      Component* current_buffer = buffers_.front();
      if (NULL == current_buffer) {
        std::string err_msg = "Buffers not yet loaded into Generic Repository.";
        throw CycException(err_msg);
      }
      // if the package is full
      if( iter->isFull()
          // and not too hot
          //&& (*iter)->getPeakTemp(OUTER) <= current_buffer->getTempLim() or 
          //too toxic
          //&& (*iter)->getPeakTox() <= current_buffer->getToxLim()
          ) {
        // emplace it in the buffer
        loadBuffer(iter);
        if( current_buffer->isFull() ) {
          buffers_.push_back(buffers_.front());
          buffers_.pop_front();
          if( buffers_.front()->isFull()){
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::conditionWaste(WasteStream waste_stream){
  // figure out what waste form to put the waste stream in
  Component* chosen_wf_template = NULL;
  chosen_wf_template = commod_wf_map_[waste_stream.second];
  if(chosen_wf_template == NULL){
    std::string err_msg = "The commodity '";
    err_msg += waste_stream.second;
    err_msg +="' does not have a matching WF in the GenericRepository.";
    throw CycException(err_msg);
  }
  // if there doesn't already exist a partially full one
  // @todo check for partially full wf's before creating new one (katyhuff)
  // create that waste form
  current_waste_forms_.push_back( new Component() );
  current_waste_forms_.back()->copy(chosen_wf_template);
  // and load in the waste stream
  current_waste_forms_.back()->absorb(waste_stream.first);
  return current_waste_forms_.back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::packageWaste(Component* waste_form){
  // figure out what waste package to put the waste form in
  bool loaded = false;
  Component* chosen_wp_template = NULL;
  std::string name = waste_form->name();
  chosen_wp_template = wf_wp_map_[name];
  if(chosen_wp_template == NULL){
    std::string err_msg = "The waste form '";
    err_msg += (waste_form)->name();
    err_msg +="' does not have a matching WP in the GenericRepository.";
    throw CycException(err_msg);
  }
  Component* toRet;
  // until the waste form has been loaded into a package
  while (!loaded){
    // look through the current waste packages 
    for (std::deque<Component*>::const_iterator iter= 
        current_waste_packages_.begin();
        iter != current_waste_packages_.end();
        iter++){
      // if there already exists an only partially full one of the right kind
      if( !(*iter)->isFull() && (*iter)->name() == 
          chosen_wp_template->name()){
        // fill it
        (*iter)->load(WP, waste_form);
        toRet = (*iter);
        loaded = true;
      } }
    // if no currently unfilled waste packages match, create a new waste package
    current_waste_packages_.push_back( new Component() );
    current_waste_packages_.back()->copy(chosen_wp_template);
    // and load in the waste form
    toRet = current_waste_packages_.back()->load(WP, waste_form); 
    loaded = true;
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::loadBuffer(Component* waste_package){
  // figure out what buffer to put the waste package in
  Component* chosen_buffer = buffers_.front();
  // and load in the waste package
  buffers_.front()->load(BUFFER, waste_package);
  // put this on the stack of waste packages that have been emplaced
  emplaced_waste_packages_.push_back(waste_package);
  // set the location of the waste package 
  setPlacement(waste_package);
  addComponentToTable(waste_package);
  // set the location of the waste forms within the waste package
  std::vector<Component*> daughters = waste_package->getDaughters();
  for (std::vector<Component*>::iterator iter = daughters.begin();  
      iter != daughters.end(); 
      iter ++){
    setPlacement(*iter);
    addComponentToTable((*iter));
  }
  return buffers_.front();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* GenericRepository::setPlacement(Component* comp){
  double x,y,z;
  // figure out what type of component it is
  switch(comp->getComponentType()) 
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
      x = (comp->getParent())->getX();
      y = (emplaced_waste_packages_.size()*dy_ - dy_/2) ; 
      z = dz_ ; 
      break;
    case WF :
      x = (comp->getParent())->getX();
      y = (comp->getParent())->getY();
      z = (comp->getParent())->getZ();
      break;
    default :
      std::string err = "ComponentType, '";
      err += comp->getComponentType();
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportHeat(){
  // update the thermal BCs everywhere
  // pass the transport heat signal through the components, inner -> outer
  for( std::deque< Component* >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportHeat();
  }
  for( std::deque< Component* >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportHeat();
  }
  for( std::deque< Component* >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportHeat();
  }
  if(NULL != far_field_){
    far_field_->transportHeat();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::transportNuclides(){
  // update the nuclide transport BCs everywhere
  // pass the transport nuclides signal through the components, inner -> outer
  for( std::deque< Component* >::const_iterator iter = waste_forms_.begin();
      iter != waste_forms_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  for( std::deque< Component* >::const_iterator iter = waste_packages_.begin();
      iter != waste_packages_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  for( std::deque< Component* >::const_iterator iter = buffers_.begin();
      iter != buffers_.end();
      iter++){
    (*iter)->transportNuclides();
  }
  if(NULL != far_field_){
    far_field_->transportNuclides();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::mapVars(std::string name, std::string type, void* ref) {
  member_types_.insert(std::make_pair( name , type));
  member_refs_.insert(std::make_pair( name , ref ));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::makeParamsTable(){
  // declare the table columns
  std::vector<column> columns;
  columns.push_back(std::make_pair("facID", "INTEGER"));
  std::map<std::string, std::string>::iterator item;
  for(item = member_types_.begin(); item != member_types_.end(); item++){
    columns.push_back(std::make_pair(item->first, item->second));
  }
  // declare the table's primary key
  primary_key pk;
  pk.push_back("facID");
  gr_params_table->defineTable(columns,pk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::defineComponentsTable(){

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void GenericRepository::addComponentToTable(Component* comp){

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

