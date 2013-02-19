/*! \file NuclideModel.h
  \brief Declares the virtual NuclideModel class used by the Generic Repository 
  \author Kathryn D. Huff
 */
#if !defined(_NUCLIDEMODEL_H)
#define _NUCLIDEMODEL_H

#include <deque>

#include "Material.h"
#include "Geometry.h"
#include "MatTools.h"
#include "MatDataTable.h"

/**
   enumerated list of types of nuclide transport model
 */
enum NuclideModelType { 
  DEGRATE_NUCLIDE, 
  LUMPED_NUCLIDE, 
  MIXEDCELL_NUCLIDE, 
  ONEDIMPPM_NUCLIDE, 
  STUB_NUCLIDE, 
  LAST_NUCLIDE};

/** 
   type definition for a map from times to IsoConcMap
   The keys are timesteps, in the unit of the timesteps in the simulation.
   The values are the IsoConcMap maps at those timesteps
  */
typedef std::map<int, IsoConcMap> ConcHist;


/**
   type definition for a map from times to (normalized) IsoVectors, 
   paired with masses

  */
typedef std::map<int, std::pair<IsoVector, double> > VecHist;

/// A shared pointer for the abstract NuclideModel class
class NuclideModel;
typedef boost::shared_ptr<NuclideModel> NuclideModelPtr;

/** 
   @brief Abstract interface for GenericRepository nuclide transport 
   
   NuclideModels such as MixedCell, OneDimPPM, etc., will share a 
   virtual interface defined here so that information passing concerning 
   hydrogeologic nuclide transport fluxes and other boundary conditions 
   can be conducted between components within the repository.
 */
class NuclideModel : public boost::enable_shared_from_this<NuclideModel> {

public:
  /**
     A virtual destructor
    */
  virtual ~NuclideModel() {};

  /**
     initializes the model parameters from a Query Engine
     
     @param qe is the QueryEngine object containing intialization info
   */
  virtual void initModuleMembers(QueryEngine* qe)=0; 

  /**
     copies a component and its parameters from another
     
     @param src is the component being copied
   */
  virtual NuclideModelPtr copy(const NuclideModel& src)=0; 

  /**
     standard verbose printer explains the model
   */
  virtual void print()=0; 

  /**
     Absorbs the contents of the given Material into this NuclideModel.
     
     @param matToAdd the NuclideModel to be absorbed
   */
  virtual void absorb(mat_rsrc_ptr matToAdd) = 0;

  /**
     Extracts the contents of the given composition from this NuclideModel. Use this 
     function for decrementing a NuclideModel's mass balance after transferring 
     through a link. 
     
     @param comp_to_rem the composition to decrement against this NuclideModel
     @param comp_to_rem the mass in kg to decrement against this NuclideModel

     @return the material extracted
   */
  virtual mat_rsrc_ptr extract(CompMapPtr comp_to_rem, double kg_to_rem) = 0 ;

  /** 
     Determines what IsoVector to remove from the daughter nuclide models

     @param time the timestep at which the nuclides should be transported
     @param daughter nuclide_model of an internal component. there may be many.
     
     */
  virtual void update_inner_bc(int the_time, std::vector<NuclideModelPtr> daughters)=0; 

  /**
     Transports nuclides from the inner boundary to the outer boundary in this 
     component

     @param time the timestep at which the nuclides should be transported
   */
  virtual void transportNuclides(int time) = 0 ;

  /** 
     returns the NuclideModelType of the model
   */
  virtual NuclideModelType type() = 0;

  /** 
     returns the name of the NuclideModelType of the model
   */
  virtual std::string name() = 0;

  /** 
     Update the hists at a certain time

     @param the_time the time at which to update the hists
   */
  virtual void update(int the_time) = 0;

  /**
     returns the available material source term at the outer boundary of the 
     component in kg
    
     @return a pair, the available isovector and mass at the outer boundary 
   */
  virtual std::pair<IsoVector, double> source_term_bc()=0;

  /**
     returns the prescribed concentration at the boundary, the source term bc
     in kg
    
     @param tope the isotope to query
     @return C the concentration at the boundary in kg/m^3
   */
  double source_term_bc(Iso tope) { 
    IsoVector st=this->source_term_bc().first;
    double massfrac = st.massFraction(tope);
    return( (massfrac == 0) ? massfrac*this->source_term_bc().second : 0);
  };

  /**
     returns the prescribed concentration at the boundary, the dirichlet bc
     in kg/m^3
    
     @param tope the isotope to query
     @return C the concentration at the boundary in kg/m^3
   */
  virtual IsoConcMap dirichlet_bc() = 0;

  /**
     returns the prescribed concentration at the boundary, the dirichlet bc
     in kg/m^3
    
     @param tope the isotope to query
     @return C the concentration at the boundary in kg/m^3
   */
  Concentration dirichlet_bc(Iso tope) { 
    IsoConcMap::iterator found = this->dirichlet_bc().find(tope);
    return(found != this->dirichlet_bc().end() ? (*found).second : 0);
  };
  
  /**
     returns the concentration gradient at the boundary, the Neumann bc
    
     @param c_ext the external concentration in the parent component
     @param r_ext the radius in the parent component corresponding to c_ext
     @param tope the isotope to query
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  virtual ConcGradMap neumann_bc(IsoConcMap c_ext, Radius r_ext) = 0;

  /**
     returns the concentration gradient at the boundary, the Neumann bc
    
     @param c_ext the external concentration in the parent component
     @param r_ext the radius in the parent component corresponding to c_ext
     @param tope the isotope to query
     @return dCdx the concentration gradient at the boundary in kg/m^3
   */
  ConcGrad neumann_bc( IsoConcMap c_ext, Radius r_ext, Iso tope) {
    IsoConcMap::iterator found = this->neumann_bc(c_ext, r_ext).find(tope);
    return((found != this->neumann_bc(c_ext, r_ext).end()) ? (*found).second : 0);
  };

  /**
     returns the flux at the boundary, the Neumann bc
    
     @return qC the solute flux at the boundary in kg/m^2/s
   */
  virtual IsoFluxMap cauchy_bc(IsoConcMap c_ext, Radius r_ext) = 0;
  Flux cauchy_bc(IsoConcMap c_ext, Radius r_ext, Iso tope) {
    IsoConcMap::iterator found = this->cauchy_bc(c_ext,r_ext).find(tope);
    return(found != this->cauchy_bc(c_ext, r_ext).end() ? (*found).second : 0);
  };
    

  /// Allows the geometry object to be set
  void set_geom(GeometryPtr geom){ geom_=geom; };

  /// Returns the geom_ data member
  const GeometryPtr geom() const {return geom_;};

  /** 
     returns the current contained contaminant mass, in kg, at time

     @param time the time to query the contained contaminant mass
     @return contained_mass_ throughout the component volume, in kg, at time
   */
  double contained_mass(int the_time){return this->vec_hist(the_time).second;}

  /**
     Returns the IsoVector mass pair for a certain time

     @param time the time to query the isotopic history
     @return vec_hist_.at(time). If not found an empty pair is returned.
     */
  std::pair<IsoVector, double> vec_hist(int the_time){
    if( last_updated() < the_time ){
      update(the_time);
    }
    std::pair<IsoVector, double> to_ret;
    VecHist::const_iterator it;
    if( !vec_hist_.empty() ) {
      it = vec_hist_.find(the_time);
      if( it != vec_hist_.end() ){
        to_ret = (*it).second;
        assert(to_ret.second < 1000 );
      } 
    } else { 
      CompMapPtr zero_comp = CompMapPtr(new CompMap(MASS));
      (*zero_comp)[92235] = 0;
      to_ret = std::make_pair(IsoVector(zero_comp),0);
    }
    return to_ret;
  }

  /** 
     The IsoVector representing the summed, normalized material in 
     the component.
     @TODO should put this in a toolkit or something. Doesn't belong here.

     @param time the time at which to retrieve the IsoVector

     @return vec_hist(time).first
   */
  IsoVector contained_vec(int the_time){
    IsoVector to_ret = vec_hist(the_time).first;
    return to_ret;
  }

  /**
     Returns the map of isotopes to concentrations at the time profided

     @param time the time at which to query the concentration history
     @return conc_hist_.at(time). If not found an empty IsoConcMap is return
    */
  IsoConcMap conc_hist(int the_time){
    if( last_updated() < the_time ){
      update(the_time);
    }
    IsoConcMap to_ret;
    ConcHist::iterator it;
    it = conc_hist_.find(the_time);
    if( it != conc_hist_.end() ){
      to_ret = (*it).second;
    } else {
      to_ret[92235] = 0 ; // zero
    }
    return to_ret;
  }

  /**
     Returns the concetration of a certain isotope at a certain time

     @param time the time to query the concentration history
     @param tope the isotope to query
     @return conc_hist(time)[iso].second, or zero if not found
    */
  Concentration conc_hist(int the_time, Iso tope){
    Concentration to_ret;
    IsoConcMap conc_map = conc_hist(the_time);
    IsoConcMap::iterator it;
    it = conc_map.find(tope);
    if(it != conc_map.end()){
      to_ret = (*it).second;
    }else{
      to_ret = 0;
    }
    return to_ret;
  }

  /**
     Returns the concentration gradient based on a simple finite difference 
     between two datapoints.

     @param c_ext external concentration, at r_ext
     @param c_int internal concentration, at r_int
     @param r_ext external radial midpoint
     @param r_int interal radil midpoint
    */
  ConcGrad calc_conc_grad(Concentration c_ext, Concentration 
      c_int, Radius r_ext, Radius r_int){
  
    ConcGrad to_ret;
    if(r_ext <= r_int){ 
      std::stringstream msg_ss;
      msg_ss << "The outer radius must be greater than the inner radius.";
      LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
      throw CycRangeException(msg_ss.str());
    } else if( r_ext == std::numeric_limits<double>::infinity() ){
      std::stringstream msg_ss;
      msg_ss << "The outer radius cannot be infinite.";
      LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
      throw CycRangeException(msg_ss.str());
    } else {
      to_ret = (c_ext - c_int) / (r_ext - r_int) ;
    }
    return to_ret;
  }

  void set_mat_table(MatDataTablePtr mat_table){mat_table_ = MatDataTablePtr(mat_table);}

  /// Returns wastes_
  std::deque<mat_rsrc_ptr> wastes() {return wastes_;};

  /// returns the time at which the vec_hist and conc_hist were updated
  int last_updated(){return last_updated_;};

  /** sets the last time that the vec_hist and conc_hist were updated,
    * first verifying whether last_updated is larger than last_updated_ 
    *
    * @param last_updated the time to set last_updated_ to.
    */ 
  void set_last_updated(int new_last_updated){
    if( last_updated_ <= new_last_updated ){
      last_updated_=new_last_updated;
    } else {
      std::stringstream msg_ss;
      msg_ss << "The new update time " << new_last_updated << " is before the current update time " << last_updated_;
      LOG(LEV_ERROR, "GRDRNuc") << msg_ss.str();;
      throw CycRangeException(msg_ss.str());
    }
  };

protected:
  /// A vector of the wastes contained by this component
  ///wastes(){return component_->wastes();};
  std::deque<mat_rsrc_ptr> wastes_;

  /// The map of times to isotopes to concentrations, in kg/m^3
  ConcHist conc_hist_;
  
  /// The map of times to IsoVectors
  VecHist vec_hist_;
  
  /// A shared pointer to the geometry of the component
  GeometryPtr geom_;

  /// A shared pointer to this Component's material data table 
  MatDataTablePtr mat_table_;

  /// the time at which the histories were last updated
  int last_updated_;

};
#endif
