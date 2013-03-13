// STCDataTable.h
#if !defined(_STCDATATABLE)
#define _STCDATATABLE

#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/multi_array.hpp>

/// typedef for isotope identifier
typedef int Iso;

/**
   Defines the structure of data associated with this row location in the  
   database.
 */
typedef struct mat_t
{
  double alpha_th; /**< a double indicating the thermal diffusivity of the material [] >**/
  double k_th; /**< a double indicating the thermal conductivity of the material [] >**/
  double spacing; /**< a double indicating the uniform spacing between packages [m] >**/
  double  r_calc; /**< a double indicating the radius where the STC is calculated >**/
  //double  <++>; /**< a double indicating the <++> of an element >**/
} mat_t;

/**
   Defines the structure of data associated with this row location in the  
   database.
 */
typedef struct stc_t
{
  double  iso; /**< a double indicating the isotope >**/
  double  time; /**< a double indicating the time at which the stc is valid >**/
  double  stc; /**< a double indicating the STC >**/
} element_t;

class STCDataTable;
typedef boost::shared_ptr<STCDataTable> STCDataTablePtr;

/**
   @class STCDataTable 
   The STCDataTable class provides an interface to the mat_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class STCDataTable {
private:

public:
  /**
     Default constructor for the STCDataTable class. 
     Default values are zeros and null strings
   */
  STCDataTable();

  /**
     Detailed constructor for the STCDataTable class
     Fully initializes the object

    @param mat the mat_ data member, a string naming this material
    @param stc_vec the stc_vec_ data member, a vector of stc_t structs, the data
    @param iso_index the iso_index_ data member, mapping the isotope IDs to indices
    @param time_index the time_index_ data member, mapping the timestep values to indices
    */
  STCDataTable(std::string mat, std::vector<stc_t> stc_vec, std::map<Iso, int> 
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

     @return mat_
     */
  std::string name(){return name_;};

protected:
  /**
     calls check_validity on the time and then returns its row index.

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
     checks whether this element has a row entry in the table.
     ideally all of them will... 
     @throws CycException when theres some drama
    */
  void check_validity(Iso tope);

  /**
     The name of the material that this table represents, 
     specifically, the name of the table in the DB
    */
  std::string name_;

  /** 
     The struct representing this material. 
     */
  mat_t mat_;

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
