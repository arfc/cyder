// MatDataTable.h
#if !defined(_MATDATATABLE)
#define _MATDATATABLE

#include <string>
#include <vector>
#include <map>

/**
   @class MatDataTable 
   The MatDataTable class provides an interface to the mat_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class MatDataTable {
private:
  /**
     The name of the data file. Typically repo_data.sqlite
    */
  static std::string file_name_;

public:
  /**
     Default constructor for the MatDataTable class. 
     Initializes the data from the provided mat_data.sqlite file. 

     @param mat the name of the material represented by this data table

     @return a MatDataTable object representing mat
   */
  MatDataTable(std::string mat);

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~MatDataTable();

  /**
     get the Atomic Number of an isotope according to its 
     identifier. 
      
     @param tope is the isotope identifier of type Iso, which is an int 
     typedef  
     @return int the atomic number of the tope isotope. 
   */
  int getAtomicNum(int tope);

  /**
     get the Mass, a double, of an isotope according to its 
     identifier. 
      
     @param tope is the isotope identifier of type Iso, which is an int 
     typedef  
     @return the mass, a double, of the tope isotope. 
   */
   double gramsPerMol(int tope);

protected:
  /**
     Defines the structure of data associated with each row entry in the 
     mass database. Right now, strings are a little funky, so the names 
   */
  typedef struct nuclide_t
  {
    int  Z; /**< an integer indicating the atomic (proton) number of an atom >**/
    int  A; /**< an integer indicating the A (mass A=N+Z) number of an atom.  >**/
    double  mass; /**< a double indicating the mass of an atom >**/
  } nuclide_t;

  /**
     The integer length (number of rows) of the mass.h5/ame03/nuclide/ 
   */
  int nuclide_len_;

  /**
     The vector of nuclide structs that holds the data in the mass table 
   */
  std::vector<nuclide_t> nuclide_vec_;

  /** 
     a map for index lookup in the nuclide vector. 
   */
  std::map<int, int> isoIndex_;

  /** 
     a function to initialize a large array of nuclide_t structs via the 
     SQLite/C++ API 
   */
  void initializeSQL();

};

#endif
