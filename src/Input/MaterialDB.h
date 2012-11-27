// MaterialDB.h
#if !defined(_MATERIALDB)
#define _MATERIALDB

#include <string>
#include <vector>
#include <map>

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
    this database
   */
  SqliteDb* db_;

public:
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

     @return K_d a double, the dispersion coefficient [kg/m^2/s] for the 
     element ent in the material mat. 
    */
  double D(std::string mat, Elem ent);

protected:
  /** 
     a map from the names of materials to table pointers
    */
  map<std::string, MatDataTablePtr> tables_;

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
     @param ent an identifier of type Elem, which is an int 
     @param data the name of the column in the materialDB table

     @return a double, the data sought for the element ent in the material mat. 
   */
  map<Elem, double> data(std::string mat, Elem ent, std::string data);

  /** 
     a function to initialize a large array of element_t structs via the 
     SQLite/C++ API 
   */
  void initializeFromSQL(std::string mat);

};

#endif
