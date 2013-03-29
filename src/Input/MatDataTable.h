// MatDataTable.h
#if !defined(_MATDATATABLE)
#define _MATDATATABLE

#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>

/// a type definition for chemical data types
enum ChemDataType{DISP, KD, SOL, LAST_CHEM_DATA_TYPE};

/// a type definition for elements
typedef int Elem;

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

class MatDataTable;
typedef boost::shared_ptr<MatDataTable> MatDataTablePtr;

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
     Default values are zeros and null strings
   */
  MatDataTable();

  /**
     Detailed constructor for the MatDataTable class
     Fully initializes the object

    @param mat the mat_ data member, a string
    @param elem_vec the elem_vec_ data member, a vector of element structs, the data
    @param elem_index the elem_index_ data member, mapping the element IDs to indices
    @param ref_disp is the reference dispersion coefficient
    @param ref_kd is the reference kd coefficient
    @param ref_sol is the reference solubility limit
    */
  MatDataTable(std::string mat, std::vector<element_t> elem_vec, std::map<Elem, 
      int> elem_index, double ref_disp=NULL, double ref_kd=NULL, double 
      ref_sol=NULL);

  /**
     Destructor for the NullFacility class. 
     Makes certain to delete all appropriate data on the stack. 
   */
  ~MatDataTable();

  /**
     get the distribution coefficient [kg/kg] for some element in this material
      
     @param ent an identifier of type Elem, which is an int 

     @return K_d a double, the distribution coefficient [kg/kg] for the 
     element ent in the material mat. 
    */
  double K_d(Elem ent);

  /**
     get the solubility limit for some element in this material 
      
     @param ent an identifier of type Elem, which is an int 

     @return S a double, the solubility limit [kg/m^3] for the element 
     ent in the material mat. 
    */
  double S(Elem ent);

  /**
     get the dispersion coefficient [kg/m^2/s] for some element in this material
      
     @param ent an identifier of type Elem, which is an int 

     @return D a double, the dispersion coefficient [kg/m^2/s] for the 
     element ent in the material mat. 
    */
  double D(Elem ent);


  /** 
     gets a specific data object for some element in this material. 

     @param ent an identifier of type Elem, which is an int 
     @param data is a ChemDataType enum (DISP, KD, SOL, ...) 

     @return the data of type data for element elt in this material.
    */
  double data(Elem ent, ChemDataType data);


  /**
     returns the string name of the material that this table represents

     @return mat_
     */
  std::string mat(){return mat_;};
  double ref_disp(){return ref_disp_;};
  double ref_kd(){return ref_kd_;};
  double ref_sol(){return ref_sol_;};


  /**
     Returns the reference paramter supplied by the user.  That is, in the case 
     of data=DISP, the reference dispersion coefficient that the user has 
     supplied for the  reference element, Hydrogen is returned.  

     @param data is a ChemDataType enum (DISP, KD, SOL, ...) 
     */
  double ref(ChemDataType data);

  /**
     The reference chemical data parameter for the reference element, Hydrogen. 
     In the case of data=DISP, this is the dispersion coefficient for the 
     reference element, Hydrogen. The amount by which D(element) differs from 
     D(hydrogen) is its relative diffusivity. That is returned with this 
     function.  

     @param ent an identifier of type Elem, the element for which to return the rel_D 
     @param data is a ChemDataType enum (DISP, KD, SOL, ...) 
     */
  double rel(Elem ent, ChemDataType data);

protected:

  /**
     checks whether this element has a row entry in the table.
     ideally all of them will... 
     @throws CycException when theres some drama
    */
  void check_validity(Elem ent);
  /**
     The material that this table represents, 
     specifically, the name of the table in the DB
    */
  std::string mat_;

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

  /// The reference dispersion coefficient
  double ref_disp_;

  /// The reference K_d coefficient
  double ref_kd_;

  /// The reference solubility limit
  double ref_sol_;

  /// This is true if the MatDataTable has been initialized with real data.
  bool initialized_;
};

#endif
