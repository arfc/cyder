// STCDB.h
#if !defined(_STCDB)
#define _STCDB

#include <string>
#include <map>

#include "SqliteDb.h"
#include "STCDataTable.h"

#define MDB STCDB::Instance()

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
     Returns the stc for the material and isotope requested. 
     If the table has not been created, this will create it.

     @param mat a struct descriptor of the table
     @param iso an isotope identifier of the isotope whose stc you're interested in 
     @param the_time the time at which to query the stc
    **/
  double stc(mat_t mat, Iso tope, int the_time);

  /**
     returns the table matching the mat string. 

     @param mat a struct indicating the variables in the table 

     @return a STCDataTablePtr holding the data associated with the mat
     */
  STCDataTablePtr table(mat_t mat);

  /**
    Converts the mat struct into a coded name.

    @param mat the struct describing a material
    */
  std::string mat_name(mat_t mat);

  /**
     This returns the iso_index_ for a particular db and mat struct.
     iso_index_ is a map from isotope IDs to indexes in the stc_vec_

     @param db the database to query
     @param mat the struct describing the material
    */
  std::map<Iso, int> iso_index(SqliteDb* db, mat_t mat);

  /**
     This returns the stc_array_ for a particular db and mat struct.
     The stc_array holds stc values for specific isotope and time pairs.
     This is the main data in the table and has dimensions n_isos x n_timesteps.

     @param db the database to query
     @param mat the struct describing the material

     @return stc_array an array of stc values for specific isotope and time pairs.
    */
  boost::multi_array<double, 2> stc_array(SqliteDb* db, mat_t mat);


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
  STCDataTablePtr initializeFromSQL(mat_t mat);

  /**
     a mat from the names of the tables to the table pointers 
    */
  std::map<std::string, STCDataTablePtr> tables_;
     
  /**
     this helper function provides the WHERE clause that selects the mat from the db.

     @param mat the material properties to specify when querying the table.
    */
  std::string whereClause(mat_t mat);


};

#endif
