/* pci_device_ids.cc --- 
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
  \file   pci_device_ids.cc
  \date   Thu Jul 29 19:39:05 2010
  
  \brief  Implementation of PciIdsVendor::store_db().

  Contains the implementation of PciIdsVendor::store_db() function which
  updates/stores PCI IDs device codes to database.
*/
#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "pci_device_ids.hpp"

using std::string;


/*
  Declaration of local functions.
*/
static void correct_quotes(string&, string::size_type&);
static void throw_call_garbage(sql::Statement*);


#ifdef DEBUG					// To debug the communication with MySQL
int on_off = 1;
#endif



int PciIdsVendor::store_db()
{
  std::vector<PciIdsDevice*>::iterator iter_dev;
  std::vector<PciIdsSubsys*>::iterator iter_subsys;

  PciIdsDevice* tmp_dev;
  PciIdsSubsys* tmp_subsys;

  const string host_url("localhost"); 						// Where MySQL Server is located.
  const string user("root");								// User to connect to the server.
  const string pass("password");							// Password of the user.
  const string database("harix_db");						// Database to connect to.

  // Hold names and codes of the Pci IDs device entries.
  string vendor_code, vendor_name, device_code, device_name, subsys_vdr, subsys_dev, subsys_name, tmp_name;
  
  const string quote = "\"";
  string::size_type qpos;

  // Could be better with MySQL Prepared Statement
  // but it's buggy operating with Stored Procedures at the moment
  
  try{
	
	/*
	 * Connect to Database with MySQL Connector/C++
	 * @{
	 */
	sql::Driver* driver = get_driver_instance();
  	std::auto_ptr<sql::Connection> connector( driver->connect(host_url, user, pass) );
#ifdef DEBUG
	connector->setClientOption("clientTrace", &on_off);
#endif
  	connector->setSchema(database);
  	std::auto_ptr<sql::Statement> stmt(connector->createStatement());
	/*
	 * @}
	 */

	vendor_code = "\"" + get_code() + "\"" ; 	// Get code of current Pci IDs vendor object.
	
	tmp_name = get_name(); 						// Get name of current Pci IDs vendor object,
	qpos = tmp_name.find(quote); 				// see if it has qoutes(") inside,
	if ( qpos != string::npos )					// which will cause a problem in SQL statement,
	  correct_quotes(tmp_name,qpos);	 		// so correct qoutes to (\").

	vendor_name = "\"" + tmp_name + "\"";
	
	// Call the stored procedure to add or update the given vendor.
	stmt->execute("CALL sp_pci_VendorsAdd("+ vendor_code + "," + vendor_name + ")");

	// Empty the retrieved result set, so we don't have problems with next statement.
	throw_call_garbage(stmt.get());

	
	if ( kid_list.size() == 0 ){ 					// If the current Pci IDs vendor has no device entry,

	  device_code = "\"XXXX\"";						// set the device code as "XXXX" and
	  device_name = "null";							// put an entry in `pci_device'.
	  stmt->execute("CALL sp_pci_DevicesAdd("+ vendor_code + "," + device_code + "," + device_name + ")");
	  throw_call_garbage(stmt.get());

	  subsys_vdr =  "null" ;   						// And for the currently stored device,
	  subsys_dev =  "null" ;						// put an entry in `pci_subsystems'
	  subsys_name =  "null" ; 						// with subvendor, subdevice, subSysName values being null.
	  stmt->execute("CALL sp_pci_SubsystemsAdd("
					+ vendor_code + "," + device_code + ","
					+ subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
	  throw_call_garbage(stmt.get());

	}
	else {		 									// When the current Pci IDs vendor has device entries

	  // loop through the device list of vendor
	  for( iter_dev = kid_list.begin(); iter_dev != kid_list.end(); ++iter_dev ){
		
		tmp_dev = *iter_dev;
		device_code = "\"" + tmp_dev->get_code() + "\"" ; // Get code of current Pci IDs device object.
		tmp_name = tmp_dev->get_name();					// And get name of current Pci IDs device object,
		qpos = tmp_name.find(quote); 					// correcting qoutes.
		if ( qpos != string::npos )
		  correct_quotes(tmp_name,qpos);

		device_name = "\"" + tmp_name + "\"";
		
		// Call the stored procedure to add or update the given device of the vendor.
		stmt->execute("CALL sp_pci_DevicesAdd("+ vendor_code + "," + device_code + "," + device_name + ")");
		throw_call_garbage(stmt.get());
		

		if( (tmp_dev->subsystem_list).size() == 0  ){ 		// If current Pci IDs device has no subsystem entry,
		  
		  subsys_vdr =  "null" ;							// for the currently stored device,
		  subsys_dev =  "null" ;							// put an entry in `pci_subsystems' with
		  subsys_name =  "null" ;							// subvendor, subdevice, subSysName values being null.
		  stmt->execute("CALL sp_pci_SubsystemsAdd("
						+ vendor_code + "," + device_code + ","
						+ subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
		  throw_call_garbage(stmt.get());

		  continue;											// Continue to look for other devices of the vendor.
		  
		}

		// When the current Pci IDs device, has subsystem entries,
		// iterate through the subsystem list of the device
		for( iter_subsys = (tmp_dev->subsystem_list).begin(); iter_subsys != (tmp_dev->subsystem_list).end();
			 ++iter_subsys ){
		  
		  tmp_subsys = *iter_subsys;
		  subsys_vdr =  "\"" + tmp_subsys->get_subVendor() + "\"" ; // Get subvendor of Pci IDs subsystem object.
		  subsys_dev =  "\"" + tmp_subsys->get_subDevice() + "\"" ; // Get subdevice of Pci IDs subsystem object.
		  tmp_name = tmp_subsys->get_name(); 					// Get subsystem name of Pci IDs subsystem object,
		  qpos = tmp_name.find(quote);							// correcting qoutes.
		  if ( qpos != string::npos )
			correct_quotes(tmp_name,qpos);

		  subsys_name =  "\"" + tmp_name + "\"" ;

		  // Call the stored procedure to add or update the given subsystem of the device.
		  stmt->execute("CALL sp_pci_SubsystemsAdd("
						+ vendor_code + "," + device_code + ","
						+ subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
		  throw_call_garbage(stmt.get());

		}
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



//! Correct quotes of provided string for SQL statement.
/*!
  Change quotes in a string from (") to (\") so that the SQL
  statements work properly.
  
  \param tmp_str String to be corrected.
  \param qpos Position of first quote found in string.
*/
static void correct_quotes(string& tmp_str, string::size_type& qpos)
{
  const string quote = "\"";
  do{
	tmp_str.replace(qpos, quote.size(), "\\\"" ); 	// Replace to quote
	qpos = tmp_str.find(quote, qpos+2);				// Find the next occurence
  }while(qpos != string::npos);						// until the end of string
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




PciIdsVendor::~PciIdsVendor()
{
  int device_list_size = kid_list.size();
  
  // Loop through the Devices(kid_list) of current Vendor and delete them.
  for ( int k=0; k < device_list_size; k++ ){
	
	delete kid_list.back();		// Since we store pointers in kid_list,
								// at first we delete the data pointed by the pointer.
	
	kid_list.pop_back();		// Then we remove the pointer from kid_list.
  }

}



PciIdsDevice::~PciIdsDevice()
{
  int subsystem_list_size = subsystem_list.size();

  // Loop through the Subsystems(subsystem_list) of current Device and delete them.	
  for (int k=0; k < subsystem_list_size; k++){
	
	delete subsystem_list.back(); 	// Since we store pointers in subsystem_list,
									// at first we delete the data pointed by the pointer.
	
	subsystem_list.pop_back();	  	// Then we remove the pointer from subsystem_list.
	
  }
}
