/* pci_class_ids.cc --- 
 *
 * Copyright (C) 2010 Alp Eren Köse
 *
 * Author: Alp Eren Köse <alperenkose@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*!
  \file   pci_class_ids.cc
  \date   Fri Jul 30 15:17:24 2010
  
  \brief  Implementation of PciIdsClass::store_db().
  
  Contains the implementation of PciIdsClass::store_db() function which
  updates/stores PCI IDs class codes to database.
*/
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "pci_class_ids.hpp"

using std::string;


/*
  Declaration of local function.
*/
static void throw_call_garbage(sql::Statement*);




int PciIdsClass::store_db (void)
{
  std::vector<PciIdsSubclass*>::iterator iter_sub;
  std::vector<PciIdsProgif*>::iterator iter_prog;

  PciIdsSubclass* tmp_sub;
  PciIdsProgif* tmp_prog;

  const string host_url("localhost"); // Where MySQL Server is located.
  const string user("root");		  // User to connect to the server.
  const string pass("password");	  // Password of the user.
  const string database("harix_db");  // Database to connect to.

  // Hold names and codes of the Pci IDs class entries.
  string class_code, class_name, subclass_code, subclass_name, prog_code, prog_name;

  // Could be better with MySQL Prepared Statement
  // but it's buggy operating with Stored Procedures at the moment
  
  try{

	/*
	 * Connect to Database with MySQL Connector/C++
	 * @{
	 */
	sql::Driver* driver = get_driver_instance();
	std::auto_ptr<sql::Connection> connector( driver->connect(host_url, user, pass) );
	connector->setSchema(database);
	std::auto_ptr<sql::Statement> stmt(connector->createStatement());
	/*
	 * @}
	 */

	class_code = "\"" + get_code() + "\"" ; 	// Get code of current Pci IDs class object.
	class_name = "\"" + get_name() + "\"";		// Get name of current Pci IDs class object.

	// Call the stored procedure to add or update the given class.	
	stmt->execute("CALL sp_pciClassesAdd("+ class_code + "," + class_name + ")");

	// Empty the retrieved result set, so we don't have problems with next statement.	
	throw_call_garbage(stmt.get());


	// Iterate through Subclass list of the Class
	for( iter_sub = kid_list.begin(); iter_sub != kid_list.end(); ++iter_sub ){
	  
	  tmp_sub = *iter_sub;
	  subclass_code = "\"" + tmp_sub->get_code() + "\"" ; 	// Get code of current Pci IDs subclass object.
	  subclass_name = "\"" + tmp_sub->get_name() + "\"";	// Get name of current Pci IDs subclass object.

	  // Call the stored procedure to add or update the given subclass of the class.
	  stmt->execute("CALL sp_pciSubClassesAdd("+ class_code + "," + subclass_code + "," + subclass_name + ")");
	  throw_call_garbage(stmt.get());


	  if( (tmp_sub->progif_list).size() == 0  ){ 			// If current Pci IDs subclass has no progif entry,
		
		prog_code =  "null" ;								// for the currently stored subclass,
		prog_name =  "null" ;								// put an entry in `pci_prog_ifs' with
															// progifCode and progifName values being null.
		stmt->execute("CALL sp_pciProgifsAdd("
					  + class_code + "," + subclass_code + "," + prog_code +  "," + prog_name + ")");
		throw_call_garbage(stmt.get());

		continue;											// Continue to look for other subclasses of the class.
	  }

	  // When the current Pci IDs subclass, has progif entries,
	  // iterate through the progif list of the subclass	  
	  for( iter_prog = (tmp_sub->progif_list).begin(); iter_prog != (tmp_sub->progif_list).end(); ++iter_prog ){
		
		  tmp_prog = *iter_prog;
		  prog_code =  "\"" + tmp_prog->get_code() + "\"" ; // Get code of current Pci IDs progif object.
		  prog_name =  "\"" + tmp_prog->get_name() + "\"" ; // Get name of current Pci IDs progif object.

		  // Call the stored procedure to add or update the given progif of the subclass.		  
		  stmt->execute("CALL sp_pciProgifsAdd("
						+ class_code + "," + subclass_code + "," + prog_code +  "," + prog_name +")");
		  throw_call_garbage(stmt.get());

		}
	}
  }
  catch (sql::SQLException &e) {
	/*
	  The MySQL Connector/C++ throws three different exceptions:
	  - sql::MethodNotImplementedException (derived from sql::SQLException)
	  - sql::InvalidArgumentException (derived from sql::SQLException)
	  - sql::SQLException (derived from std::runtime_error)
	*/
	std::cout << "# ERR: SQLException in " << __FILE__;
	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	std::cout << "# ERR: " << e.what();
	std::cout << " (MySQL error code: " << e.getErrorCode();
	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

	return 1;
  }

  return 0;
}




//! Empty result set of provided statement.
/*! 
  Empty the retrieved result set from the provided statement,
  so we don't have problems with next statement.
  
  \param normal_stmt SQL Statement object.
*/
static void throw_call_garbage(sql::Statement* normal_stmt)
{
  std::auto_ptr< sql::ResultSet > res;
  do {
	res.reset(normal_stmt->getResultSet()); 	// Get result set of the statement,
	while (res->next()) {						// and process until there are no more values left.
#ifdef DEBUG
	  std::cout << res->getInt(1) << " -> expected value is 0" << std::endl;
#endif
	}
  } while (normal_stmt->getMoreResults()); 		// continue if more ResultSets exists, which is not really likely
}




PciIdsClass::~PciIdsClass()
{
  int subclass_list_size = kid_list.size();

  // Loop through the Subclasses(kid_list) of current Class and delete them.  
  for ( int k=0; k<subclass_list_size; k++ ){
	
	delete kid_list.back();		// Since we store pointers in kid_list,
								// at first we delete the data pointed by the pointer.

	kid_list.pop_back();		// Then we remove the pointer from kid_list.
  }

}




PciIdsSubclass::~PciIdsSubclass()
{
  int progif_list_size = progif_list.size();

  // Loop through the Prog-ifs(progif_list) of current Subclass and delete them.	  
  for (int k=0; k < progif_list_size; k++){
	
	delete progif_list.back();	// Since we store pointers in progif_list,
								// at first we delete the data pointed by the pointer.
	
	progif_list.pop_back();		// Then we remove the pointer from progif_list.
  }
}
