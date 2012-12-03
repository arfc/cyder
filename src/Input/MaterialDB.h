// MaterialDB.h
#if !defined(_MATERIALDB)
#define _MATERIALDB

#include <string>
#include <map>
#include "SqliteDb.h"
#include "MatDataTable.h"

#define MDB MaterialDB::Instance()

/// a type definition for elements
typedef int Elem;

/**
   @class MaterialDB 
   The MaterialDB class provides an interface to the mat_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class MaterialDB {
private:
  /** 
     A pointer to this MaterialDB once it has been initialized
    */
  static MaterialDB* instance_;

  /**
    this database's file path
   */
  std::string file_path_;

public:
  /** 
     Provides a singleton instance for the MaterialDB.
     Like the Highlander, there should be only one. 

     @return a pointer to the MaterialDB
    */
  static MaterialDB* Instance();

  /**
     Default constructor for the MaterialDB class. 
     Initializes the data from the provided mat_data.sqlite file. 
   */
  MaterialDB();

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~MaterialDB();

  /**
     get the distribution coefficient [kg/kg] for some element in some material of interest
      
     @param mat the name of the material table in the database
     @param ent an identifier of type Elem, which is an int 

     @return K_d a double, the distribution coefficient [kg/kg] for the 
     element ent in the material mat. 
    */
  double K_d(std::string mat, Elem ent);

  /**
     get the solubility limit for some element in some material of interest
      
     @param mat the name of the material table in the database
     @param ent an identifier of type Elem, which is an int 

     @return S a double, the solubility limit [kg/m^3] for the element 
     ent in the material mat. 
    */
  double S(std::string mat, Elem ent);

  /**
     get the dispersion coefficient [kg/m^2/s] for some element
     in some material of interest
      
     @param mat the name of the material table in the database
     @param ent an identifier of type Elem, which is an int 

     @return D a double, the dispersion coefficient [kg/m^2/s] for the 
     element ent in the material mat. 
    */
  double D(std::string mat, Elem ent);

  /** 
     a map from the names of materials to table pointers
    */
  std::map<std::string, MatDataTablePtr> tables_;

  /**
     get a piece of data for some element in some material of interest
      
     @param mat the name of the material table in the database
     @param ent an identifier of type Elem, which is an int 
     @param data the name of the column in the materialDB table

     @return a double, the data sought for the element ent in the material mat. 
   */
  double data(std::string mat, Elem ent, std::string data);

  /**
     get a piece of data for some element in some material of interest
      
     @param mat the name of the material table in the database
     @param data the name of the column in the materialDB table

     @return a double, the data sought for the element ent in the material mat. 
   */
  std::map<Elem, double> data(std::string mat, std::string data);

  /**
     returns the table matching the mat string. 

     @param mat a string indicating the name of the table (clay, salt, etc.)

     @return a MatDataTablePtr holding the data associated with the mat
     */
  MatDataTablePtr table(std::string mat);

protected:
  /**
     checks whether a table associated with a particular mat has been created

     @param mat the string name of the table to check the existence of
     */
  bool initialized(std::string mat);

  /** 
     a function to initialize a large array of element_t structs via the 
     SQLite/C++ API 

     @param mat the string indicatin the material this table should represent
   */
  MatDataTablePtr initializeFromSQL(std::string mat);

};

#endif
