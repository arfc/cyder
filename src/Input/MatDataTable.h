// MatDataTable.h
#if !defined(_MATDATATABLE)
#define _MATDATATABLE

#include <string>
#include <vector>
#include <map>

/// a type definition for elements
typedef int Elem;

/// a type definition for chemical data types
typedef enum ChemDataType{DISP, KD, SOL, LAST_CHEM_DATA_TYPE};

/**
   Defines the structure of data associated with each row entry in the 
   database.
 */
typedef struct element_t
{
  int  Z; /**< an integer indicating the atomic (proton) number of an element >**/
  double  D; /**< a double indicating the disperson coeff. of an element [] >**/
  double  K_d; /**< a double indicating the distribution coeff. of an element [] >**/
  double  S; /**< a double indicating the solubility limit of an element [] >**/
  //double  <++>; /**< a double indicating the <++> of an element >**/
} element_t;

/**
   @class MatDataTable 
   The MatDataTable class provides an interface to the mat_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class MatDataTable {
private:

public:
  /**
     Default constructor for the MatDataTable class. 
     Initializes the data from the provided mat_data.sqlite file. 
   */
  MatDataTable();

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~MatDataTable();

protected:
  /**
     The integer length (number of rows) of the tables 
   */
  int elem_len_;

  /**
     The vector of element structs that holds the data in the mass table 
   */
  std::vector<element_t> elem_vec_;

  /** 
     a map for index lookup in the element vector. 
   */
  std::map<Elem, int> elem_index_;
};

#endif
