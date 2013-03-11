// STCDB.h
#if !defined(_STCDB)
#define _STCDB

#include <string>
#include <map>
#include "SqliteDb.h"
#include "STCDataTable.h"

#define MDB STCDB::Instance()

/// a type definition for elements
typedef int Elem;

/**
   @class STCDB 
   The STCDB class provides an interface to the mat_data.sqlite 
   database, providing a robust and correct mass lookup by isotope 
 */
class STCDB {
private:
  /** 
     A pointer to this STCDB once it has been initialized
    */
  static STCDB* instance_;

  /**
    this database's file path
   */
  std::string file_path_;

public:
  /** 
     Provides a singleton instance for the STCDB.
     Like the Highlander, there should be only one. 

     @return a pointer to the STCDB
    */
  static STCDB* Instance();

  /**
     Default constructor for the STCDB class. 
     Initializes the data from the provided mat_data.sqlite file. 
   */
  STCDB();

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~STCDB();

  /**
     returns the table matching the mat string. 

     @param mat a string indicating the name of the table (a0.2k0.001s5r2)

     @return a STCDataTablePtr holding the data associated with the mat
     */
  STCDataTablePtr table(std::string mat);

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
  STCDataTablePtr initializeFromSQL(std::string mat);

  

};

#endif
