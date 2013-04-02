// STCDataTable.h
#if !defined(_STCDATATABLE)
#define _STCDATATABLE

#include <string>
#include <vector>
#include <map>

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

/// typedef for isotope identifier
typedef int Iso;

/**
   Defines the structure of data associated with this row location in the  
   database.
 */
typedef struct th_params_t
{
  public : 
    double alpha_th; /**< a double indicating the thermal diffusivity of the material [] >**/
    double k_th; /**< a double indicating the thermal conductivity of the material [] >**/
    double spacing; /**< a double indicating the uniform spacing between packages [m] >**/
    double  r_calc; /**< a double indicating the radius where the STC is calculated >**/
    //double  <++>; /**< a double indicating the <++> of an element >**/
    th_params_t& a(double a_in){ alpha_th = a_in; return *this; }
    th_params_t& k(double k_in){ k_th = k_in; return *this; }
    th_params_t& s(double s_in){ spacing = s_in; return *this; }
    th_params_t& r(double r_in){ r_calc = r_in; return *this; }

} th_params_t;


class STCDataTable;
typedef boost::shared_ptr<STCDataTable> STCDataTablePtr;

/**
   @class STCDataTable 
   The STCDataTable class provides an interface to the stc_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class STCDataTable {
private:
  /**
     Default constructor for the STCDataTable class. 
     Default values are zeros and null strings
   */
  STCDataTable();

public:

  /**
     Detailed constructor for the STCDataTable class
     Fully initializes the object

    @param name the name_ data member, a string naming this material
    @param stc_array the stc_array_ data member, a 2d array of stc values (n_isos x n_timesteps) 
    @param iso_index the iso_index_ data member, mapping the isotope IDs to indices
    @param time_index the time_index_ data member, mapping the timestep values to indices
    */
  STCDataTable(std::string name, boost::multi_array<double, 2> stc_array, std::map<Iso, int> 
      iso_index, std::map<int, int> time_index);

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~STCDataTable();

  /**
     get the specific temperature change [K] for an isotope in this material.
      
     @param tope an identifier of type Iso, which is an int 
     @param the_time, an integer indicating the timestep at which to determine the stc 

     @return stc a double, the specific temperature change at time the_time [K]
    */
  double stc(Iso tope, int the_time);

  /**
     returns the string name of the material that this table represents

     @return name_
     */
  std::string name(){return name_;};

  /**
     calls checkValidity on the time and then returns its row index.

     @param the_time the timestep value (e.g., 200)
     @return the index of that time in the stc_array
     */
  int timeToInd(int the_time);

  /**
     calls check_validity on the isotope and then returns its row index.

     @param tope the isotope identifier (e.g., 92235)
     @return the index of the row of that isotope
     */
  int isoToInd(Iso tope);

  /**
     calls check_validity on the isotope and then returns its row value.


     @param index the map of indices for this table in which the val exists
     @param ind the index of the row of that isotope
     @return tope the isotope identifier (e.g., 92235)
     */
  int indToVal(int ind, std::map<int, int> index);

  /**
     calls checkValidity on the iso and then returns its row value.

     @param ind the index of the value of interest
     @return the value of that index in the stc_array
     */
  int indToIso(int ind);

  /**
     calls checkValidity on the time and then returns its row value.

     @param ind the index of the value of interest
     @return the value of that index in the stc_array
     */
  int indToTime(int ind);

  std::map<int, int>timeIndex(){return time_index_; }
  std::map<Iso, int>isoIndex(){return iso_index_; }

protected:
  /**
     checks whether this element has a row entry in the table.
     ideally all of them will... 

     @param val the value to find in the index
     @param index the map between values and indices

     @throws CycException when theres some drama
    */
  void checkValidity(int val, std::map<int, int> index);

  /**
     The name of the material that this table represents, 
     specifically, the name of the table in the DB
    */
  std::string name_;

  /** 
     The struct representing this material. 
     */
  th_params_t th_params_;

  /**
     The array of stc data. It's a 2d array with dimensions iso x time. Think 
     of it as stc[n_isos][n_timesteps].
     */
  boost::multi_array<double, 2> stc_array_;

  /** 
     a map for isotope index lookup in the stc array. 
   */
  std::map<Iso, int> iso_index_;

  /** 
     a map for time index lookup in the stc array. 
   */
  std::map<int, int> time_index_;
};

#endif
