/* database.cc --- 
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
  \file   database.cc
  \date   Thu Jul 15 17:38:56 2010
  
  \brief  All Database Communication
  
  Encapsulates all the database related functions
  with the exception of pci_ids folder having their own functions to update PCI IDs.
*/
#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <vector>
#include <map>
// #include <string>


#include "os_info.hpp"
#include "pci_device.hpp"
#include "harixApp.hpp"			// needed for wApp
#include <Wt/WLogger>

using std::string;


//! Where MySQL Server is located.
const string host_url("localhost");

//! User to connect to the server.
const string user("root");

//! Password of the user.
const string pass("password");

//! Database to connect to.
const string database("harix_db");

//! Object to retrieve results of queries.
static sql::ResultSet* commonResultSet;

//! Object to establish SQL Server connection.
static sql::Connection* connector;

//! SQL statements are given through this.
static sql::Statement* stmt;


//! Connect to Database with MySQL Connector/C++.
/*! 
  Used in the beginning of each function to open a connection to database.
 */
inline void connectDatabase()
{
  sql::Driver* driver = get_driver_instance();
  connector = driver->connect(host_url, user, pass);
  connector->setSchema(database);
  stmt = connector->createStatement();
}


//! Delete the objects used to connect database.
/*! 
  Used in the end of each function to delete the #stmt, #commonResultSet and #connector.
 */
inline void disconnectDatabase()
{
  delete commonResultSet;
  delete stmt;
  delete connector;
}



//! Query full class name for provided codes.
/*!
  Retrieve a string list of Class, Subclass and Prog-if names
  corresponding to provided codes.
  
  \param class_code Class Code of a device.
  \param subclass_code Subclass Code of a device.
  \param progif_code Programming interface Code of a device.
  
  \return Class, Subclass and Prog-if names that correspond to provided codes.
*/
std::vector<string> queryClassName(string class_code, string subclass_code, string progif_code)
{
  std::vector<string> full_class_name; 	// Hold Class, Subclass and Prog-if name.
  
  try{
	connectDatabase();

	// Make a call to the stored procedure which will return a line of corresponding names to the codes.
	// At least Class-Subclass code pair should exist in database for non-empty return.
	commonResultSet = stmt->executeQuery("CALL sp_queryClassName('"+ class_code +"','"+ subclass_code +"'," +
										 "'" + progif_code  + "')");

	if( commonResultSet->next() ){
	  full_class_name.push_back(commonResultSet->getString("className"));
	  full_class_name.push_back(commonResultSet->getString("subclassName"));
	  full_class_name.push_back(commonResultSet->getString("progifName"));
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";

  }
  disconnectDatabase();

  return full_class_name;
}




//! Query full device name for provided codes.
/*!
  Retrieve a string list of Vendor, Device and Subsystem names
  corresponding to provided codes.
  
  \param vendor_code Vendor Code of a device.
  \param device_code Device Code of a device.
  \param subvendor_code Subvendor Code of a device.
  \param subdevice_code Subdevice Code of a device.
  
  \return Vendor, Device and Subsystem name that correspond to provided codes.
*/
std::vector<string>
queryDeviceName(string vendor_code, string device_code, string subvendor_code, string subdevice_code)
{
  std::vector<string> full_device_name; 	// Hold Vendor, Device and Subsystem name.
  
  try{
	connectDatabase();

	// Make a call to the stored procedure which will return a line of corresponding names to the codes.
	// At least Vendor-Device code pair should exist in database for non-empty return.
	commonResultSet = stmt->executeQuery("CALL sp_queryDeviceName('"+ vendor_code +"','"+ device_code +"'," +
										 "'" + subvendor_code + "','" + subdevice_code + "')");

	if( commonResultSet->next() ){
	  full_device_name.push_back(commonResultSet->getString("vendorName"));
	  full_device_name.push_back(commonResultSet->getString("deviceName"));
	  full_device_name.push_back(commonResultSet->getString("subsysName"));
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return full_device_name;
}



//! Query list of OSes with their modules that support the provided device.
/*!
  Retrieves list of Unique IDs representing OSes and their modules which
  support the provided device.
  
  \param vendor_code Vendor Code of provided device.
  \param device_code Device Code of provided device.
  \param subvendor_code Subvendor Code of provided device.
  \param subdevice_code Subdevice Code of provided device.
  \param class_code Class Code of provided device.
  \param subclass_code Subclass Code of provided device.
  \param progif_code Prog-if Code of provided device.
  
  \return List of (Unique OS ID - Module Name ID) pairs.
*/
std::multimap<string, string>
queryPcimapOsList(string vendor_code, string device_code, string subvendor_code, string subdevice_code,
			   string class_code, string subclass_code, string progif_code)
{
  std::multimap<string,string> ukernel_module_map; // ( unique_kernel_id, module_name_id )
  
  try{
	connectDatabase();

	// Make a call to the stored procedure which will return a list of OS-Module pairs that support the device.
	commonResultSet = stmt->executeQuery("CALL sp_queryPcimap('"+ vendor_code +"','"+ device_code +"'," +
										 "'" + subvendor_code + "','" + subdevice_code + "'," +
										 "'" + class_code + subclass_code + "','" + progif_code + "')");

	while( commonResultSet->next() ){
	  ukernel_module_map.insert( std::pair<string,string>(commonResultSet->getString("uKernelID"), commonResultSet->getString("modNameID") ) );
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return ukernel_module_map;
}



//! Query full name of an OS.
/*!
  Queries full OS name with it's unique id and puts them into a std::vector container.
  
  \param unique_kernel_id Unique kernel ID of an OS.
  
  \return List of Distribution name, Release name, Kernel version and architecture.
*/
std::vector<string> queryOs( string unique_kernel_id )
{
  std::vector<string> full_os_details;
  
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT osName, releaseName, kernelVersion, machineHardware FROM vw_osList WHERE uKernelID='" + unique_kernel_id +"'");

	if( commonResultSet->next() ){
	  full_os_details.push_back(commonResultSet->getString("osName"));
	  full_os_details.push_back(commonResultSet->getString("releaseName"));
	  full_os_details.push_back(commonResultSet->getString("kernelVersion"));
	  full_os_details.push_back(commonResultSet->getString("machineHardware"));
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return full_os_details;
  
}



//! Query unique kernel ID of an OS.
/*!
  Queries the Unique Kernel ID representing each OS stored in database.

  \param os_name Distribution name(e.g. Ubuntu).
  \param release Release name of given distribution(e.g. 9.10).
  \param kernel Kernel Version of given distribution(e.g. 2.6.32).
  \param architecture Architecture of the kernel compiled for(e.g. i686).
  
  \return The Unique Kernel ID of OS.
*/
string queryOsKernelId( string os_name, string release, string kernel, string architecture )
{
  string ukernel_id="";

  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT uKernelID FROM vw_osList WHERE osName='"+ os_name + "' " +
										 "AND releaseName='"+ release  +"' " +
										 "AND kernelVersion='"+ kernel +"' " +
										 "AND machineHardware='"+ architecture  +"'");

	if( commonResultSet->next() ){
	  ukernel_id = commonResultSet->getString("uKernelID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  return ukernel_id;
}




//! Query Name of the Module having the provided ID.
/*! 
  
  \param mod_name_id ID of a Module name in database.
  
  \return Name of the Module.
*/
string queryModuleName( string mod_name_id )
{
  string module_name;			// Hold name of module.
  
  try{
	connectDatabase();

	// Select name of the module with provided ID.
	commonResultSet = stmt->executeQuery("SELECT modName FROM module_names WHERE modNameID='"
										 + mod_name_id +"'");

	if( commonResultSet->next() ){
	  module_name = commonResultSet->getString("modName");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return module_name;
  
}



//! Record the given OS in database.
/*!
  Updates database tables `OSes' and `os_releases' if they don't
  have the corresponding entries for current OS, and lastly writes an entry
  into `kernels' table for the new OS.
  
  \param osInfo Object holding information of the OS.
  
  \return The Unique Kernel ID of added OS.
*/
string recordOsInfo (OsInfo osInfo)
{
  string osId_="", releaseId_="", uKernelId_="";
  try{
	connectDatabase();
	/*
	 * Check OS Name entry in DB
	 * @{
	 */
	commonResultSet = stmt->executeQuery("SELECT osID FROM OSes WHERE osName='"+ osInfo.getDistro() +"'");
	
	if( commonResultSet->first() ){ 	// We alread have a distribution with that name.
	  
	  osId_ = commonResultSet->getString("osID");		// Return the osID from `OSes` table.
	  
	}
	else{								// Distribution with given OS name doesn't exist.
	  
	  // Insert Distribution into OSes.
	  stmt->execute("INSERT INTO OSes(osName) VALUES ('"+ osInfo.getDistro() +"')");

	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		osId_ = commonResultSet->getString(1); 			// Return the currently inserted osID from `OSes` table.
	  }
	  
	}
	assert( osId_ != "" );
	/*
	 * @}
	 */


	/*
	 * Check Release entry in DB
	 * @{
	 */
	commonResultSet = stmt->executeQuery("SELECT releaseID FROM os_releases WHERE osID_FK="+ osId_ +
										 " AND releaseName='"+ osInfo.getRelease() +"'");
	
	if( commonResultSet->first() ){ 	// We alread have a distribution with given OS and Release names.
	  
	  releaseId_ = commonResultSet->getString("releaseID"); // Return the releaseID from `os_releases` table.
	}
	else{					  			// Distribution with given Release name doesn't exist.
	  
	  // Insert Dist.Release into os_releases.
	  stmt->execute("INSERT INTO os_releases(osID_FK,releaseName) VALUES("+ osId_ +",'"+ osInfo.getRelease() +"')");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		// Return the currently inserted releaseID from `os_releases` table.
		releaseId_ = commonResultSet->getString(1);
	  }
	  
	}
	assert( releaseId_ != "" );
	/*
	 * @}
	 */

  
	/*
	 * Insert uKernel entry in DB
	 * @{
	 */
	// Insert Kernel Version into kernels and retrieve uKernelID
	stmt->execute("INSERT INTO kernels( releaseID_FK, kernelVersion, machineHardware ) VALUES ("
				  + releaseId_ +",'"+ osInfo.getKernel() +"','"+ osInfo.getArch() +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  uKernelId_ = commonResultSet->getString(1);
	}
	/*
	 * @}
	 */

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  assert( uKernelId_ != "" );
  return uKernelId_;
}



//! Check if Module with given name exists.
/*!
  Query the database `module_names' table with given name, and
  insert it if it doesn't exist. Returns modNameID from `module_names' table.
  
  \param module_name Name of the Kernel module(driver).
  
  \return The ID representing the given module name.
*/
string checkModuleNameId( string module_name )
{
  string mod_name_id;
  try{
	connectDatabase();
	commonResultSet = stmt->executeQuery("SELECT modNameID FROM module_names WHERE modName='"
										 + module_name +"'");
	
	if( commonResultSet->first() ){ 		// If the given module name is already in database.
	  
	  mod_name_id = commonResultSet->getString("modNameID"); // Get it's ID.
	  
	}
	else {									// If module name is not found, insert it..
	  
	  stmt->execute("INSERT INTO module_names(modName) VALUES ('"+ module_name +"')");
	  
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		mod_name_id = commonResultSet->getString(1); 		// And get it's ID.
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
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  
  assert( mod_name_id != "" );
  return mod_name_id;
}



//! Check if the Kernel has the provided Module in database.
/*!
  Query the database `modules' table with "Unique Kernel ID" - "Module Name ID" pair,
  and insert if it doesn't exist.
  
  \param uKernelId Unique Kernel ID of an OS.
  \param modNameId ID representing a module name.
  
  \return The unique ID representing the given Module of the given OS.
*/
string checkKernelModule (string uKernelId, string modNameId)
{
  string uniqueModuleId_="";
  try{
	connectDatabase();
	
	commonResultSet = stmt->executeQuery("SELECT uModID FROM modules WHERE uKernelID_FK="+ uKernelId +
										 " AND modNameID_FK="+ modNameId);
	
	if( commonResultSet->first() ){ 			// We found the Module for the OS.
	  uniqueModuleId_ = commonResultSet->getString("uModID"); 	// So get it's ID.
	  
	}
	else {										// Module is not found for the OS, so insert a new record.
	  stmt->execute("INSERT INTO modules(uKernelID_FK, modNameID_FK) VALUES ("+ uKernelId +","+ modNameId +")");
	  
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		uniqueModuleId_ = commonResultSet->getString(1); 		// Get it's ID, which is the last inserted one.
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
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  
  assert( uniqueModuleId_ != "" );
  return uniqueModuleId_;
}



//! Update pcimap-list entry of a given Module of OS.
/*!
  First check if the entry already exists in database,
  if not add an entry with Unique Module ID(uModID from `modules' table) and
  the given device.
  
  \param currentPciDevice Object holding the device details.
  \param uniqueModuleId Unique ID representing a Module of an OS.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int insertPcimap ( const PciDevice* const currentPciDevice, std::string uniqueModuleId )
{
  try{
	connectDatabase();

	// Select from the database `pcimap' table for pcimap-list entry of a specific Module of an OS.
	commonResultSet = stmt->executeQuery("SELECT * FROM pcimap WHERE uModID_FK="+ uniqueModuleId +
										 " AND vendor='"+ currentPciDevice->getVendor() +"'"+
										 " AND device='"+ currentPciDevice->getDevice() +"'"+
										 " AND subvendor='"+ currentPciDevice->getSubvendor() +"'"+
										 " AND subdevice='"+ currentPciDevice->getSubdevice() +"'"+
 " AND class='"+ currentPciDevice->getClass() + currentPciDevice->getSubclass() + currentPciDevice->getProgif() +"'"+
										 " AND classMask='"+ currentPciDevice->getClassMask() +"'");

	if( !commonResultSet->first() ){ 			// The provided entry does not exist in pcimap-list so insert it.
	  
	  std::string insertPciEntry = 
		"INSERT INTO pcimap VALUES ("+ uniqueModuleId  +",'"+ currentPciDevice->getVendor() +"',"
		+"'"+ currentPciDevice->getDevice() +"','"+ currentPciDevice->getSubvendor() +"',"
		+"'"+ currentPciDevice->getSubdevice() +"',"
		+"'"+ currentPciDevice->getClass() + currentPciDevice->getSubclass() + currentPciDevice->getProgif() +"',"
		+"'"+ currentPciDevice->getClassMask() + "')";
	  stmt->execute(insertPciEntry);
	  // @TODO: check success of insertion with Statement -stmt- ; and put return
	  
	}
	
  }
  catch (sql::SQLException &e) {
	/*
	  The MySQL Connector/C++ throws three different exceptions:
	  - sql::MethodNotImplementedException (derived from sql::SQLException)
	  - sql::InvalidArgumentException (derived from sql::SQLException)
	  - sql::SQLException (derived from std::runtime_error)
	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
	disconnectDatabase();
	return 1;
  }
  disconnectDatabase();

  return 0;
}



//! Query ID of the mainboard with provided name.
/*!
  Retrieves ID of the mainboard, returns an empty string if it
  does not exist in database.
  
  \param board_name Name of a mainboard.
  
  \return The ID representing the provided mainboard in database.
*/
string queryBoardModelId( string board_name )
{
  string board_id = "";				// Hold the ID of the mainboard.
  try{
	connectDatabase();

	// Select ID of the mainboard with given name.
	commonResultSet = stmt->executeQuery("SELECT boardID FROM board_models WHERE boardName='"
										 + board_name +"'");

	if( commonResultSet->next() ){
	  board_id = commonResultSet->getString("boardID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return board_id;
}



//! Delete Mainboard with given ID.
/*!
  Delete the mainboard from database together with the devices
  belonging to it in `dev_board' table.
  
  \param board_id ID of a mainboard.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int deleteBoardModel( std::string board_id )
{
  try{
	connectDatabase();

	// Delete board from `board_models' table, which will cascade into `dev_board' to delete it's devices.
	stmt->execute("DELETE FROM board_models WHERE boardID="+ board_id);
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
	return 1;
  }
  disconnectDatabase();
  return 0;
}



//! Store the provided mainboard.
/*!
  Stores the mainboard with given name in database and
  return it's ID.
  
  \param board_name Name of the mainboard to be saved.
  
  \return The ID of currently stored mainboard.
*/
string insertBoardModel( string board_name )
{
  string board_id;				// Hold the ID of the mainboard.

  try{
	connectDatabase();

	// Insert the given mainboard in database.
	stmt->execute("INSERT INTO board_models(boardName) VALUES ('"+ board_name +"')");

	// Retrieve the ID given at last insertion which is the currently inserted mainboard ID.
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  board_id = commonResultSet->getString(1);
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return board_id;
}



//! Store devices of given mainboard.
/*!
  Insert the provided devices belonging to the given mainboard
  to database.
  
  \param board_id ID of the mainboard the devices intented to be added to.
  \param device_id_list Unique ID list of devices to be added.
*/
void insertBoardDevices( string board_id, std::vector<string>& device_id_list )
{
  try{
  	connectDatabase();

	// Iterate through the list of devices and insert them in `dev_board' database table with given mainboard.
  	std::vector<string>::iterator device_iter;
  	for( device_iter = device_id_list.begin(); device_iter != device_id_list.end(); ++device_iter ){
  	  stmt->execute("INSERT INTO dev_board VALUES ("+ board_id +","+ *device_iter +")");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
  	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
  	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
  	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
  	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
	
}



//! Query PCI IDs Vendor name for provided code.
/*!
  Retrieve vendor name corresponding to provided code.
  
  \param vendor_code Vendor Code of a device.
  
  \return Vendor name that correspond to provided code.
*/
string queryPciIdsVendorName( string vendor_code )
{
  string vendor_name="";		// Hold Vendor name.
  
  try{
	connectDatabase();

	// Make a call to the stored procedure which will return an entry of corresponding name to the code.
	commonResultSet = stmt->executeQuery("SELECT vendorName FROM pci_vendors WHERE vendorCode='"
										 + vendor_code +"'");

	if( commonResultSet->next() ){
	  vendor_name = commonResultSet->getString("vendorName");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return vendor_name;
  
}




//! Insert a PCI IDs Vendor entry.
/*!
  Insert a PCI Vendor to database, does not check for anything.
  
  \param vendor_code Vendor Code.
  \param vendor_name Vendor Name.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
void insertPciIdsVendor( string vendor_code, string vendor_name )
{
  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_vendors VALUES ('"+ vendor_code  +"','"+ vendor_name +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
}





//! Query ID of a PCI IDs Device entry in database.
/*!
  Retrieve ID of a PCI IDs Device entry(pci.ids device line) stored in database.
  
  \param vendor_code Vendor Code of Device entry.
  \param device_code Device Code.
  
  \return ID representing the PCI IDs Device entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
string queryPciIdsDeviceId( string vendor_code, string device_code )
{
  string device_id="";
  
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT deviceID FROM pci_devices WHERE vendorCode_FK='"
										 + vendor_code +"' AND deviceCode='"+ device_code  +"'");

	if( commonResultSet->next() ){
	  device_id = commonResultSet->getString("deviceID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return device_id;
}



//! Insert a PCI IDs Device entry in database.
/*!
  Add a PCI IDs Device entry(pci.ids device line) to database..
  
  \param vendor_code Vendor Code of the Device entry.
  \param device_code Device Code.
  \param device_name Device Name.
  
  \return ID representing the PCI IDs Device entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
string insertPciIdsDevice( string vendor_code, string device_code, string device_name )
{
  string device_id;

  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_devices(deviceCode,deviceName,vendorCode_FK) VALUES ('"+ device_code  +"',"
				  "'"+ device_name +"','"+ vendor_code +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  device_id = commonResultSet->getString(1);
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return device_id;
}



//! Insert a PCI IDs subsystem entry in database.
/*!
  Add a PCI IDs Subsystem entry(pci.ids subsystem line) to database..
  
  \param device_id ID representing the Subsystem's Device in database.
  \param subvendor_code Subvendor Code.
  \param subdevice_code Subdevice Code.
  \param subsysname Subsystem Name.
  
  \return ID representing the PCI IDs Subsystem entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
string insertPciIdsSubsys( string device_id , string subvendor_code, string subdevice_code, string subsysname )
{
  string uPcisub_id;

  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_subsystems(subvendor,subdevice,subSysName,deviceID_FK)"
				  " VALUES ('"+ subvendor_code +"','"+ subdevice_code +"','"+ subsysname +"','"+ device_id +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  uPcisub_id = commonResultSet->getString(1);
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return uPcisub_id;
}



//! Query unique ID of a PCI IDs Subsystem entry in database.
/*!
  Retrieve ID of a PCI IDs Subsystem entry(pci.ids subsystem line) stored in database.
  
  \param vendor_code Vendor Code of the Device of the Subsystem.
  \param device_code Code of the Subsystem's Device.
  \param subvendor_code Subvendor Code.
  \param subdevice_code Subdevice Code.
  
  \return ID representing the PCI IDs Subsystem entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
string queryPciIdsSubsysId(string vendor_code, string device_code, string subvendor_code, string subdevice_code)
{
  string uPcisubId = "";

  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("CALL sp_queryDeviceName('"+ vendor_code +"','"+ device_code +"'," +
										 "'" + subvendor_code + "','" + subdevice_code + "')");

	if( commonResultSet->next() ){
	  uPcisubId = commonResultSet->getString("uPcisubID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return uPcisubId;
}



//! Query PCI IDs Class name for provided code.
/*!
  Retrieve class name corresponding to provided code.
  
  \param class_code Class code of a device.
  
  \return Class name.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
string queryPciIdsClassName( string class_code )
{
  string class_name="";
  
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT className FROM pci_classes WHERE classCode='"
										 + class_code +"'");

	if( commonResultSet->next() ){
	  class_name = commonResultSet->getString("className");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return class_name;
}



//! Insert a PCI IDs Class entry.
/*!
  Insert a PCI Class to database, does not check for anything.
  
  \param class_code Class Code.
  \param class_name Class Name.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>
*/
void insertPciIdsClass( string class_code, string class_name )
{
  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_classes VALUES ('"+ class_code  +"','"+ class_name +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
}



//! Query ID of a PCI IDs Subclass entry in database.
/*!
  Retrieve ID of a PCI IDs Subclass entry(pci.ids subclass line) stored in database.
  
  \param class_code Class Code of Subclass entry.
  \param subclass_code Subclass Code.
  
  \return ID representing the PCI IDs Subsystem entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>  
*/
string queryPciIdsSubclassId( string class_code, string subclass_code )
{
  string subclass_id="";
  
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT subClassID FROM pci_subclasses WHERE classCode_FK='"
										 + class_code +"' AND subClassCode='"+ subclass_code  +"'");

	if( commonResultSet->next() ){
	  subclass_id = commonResultSet->getString("subClassID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return subclass_id;
}



//! Insert a PCI IDs Subclass entry.
/*!
  Add a PCI IDs Subclass entry(pci.ids subclass line) to database..
  
  \param class_code Class Code of the Subclass entry.
  \param subclass_code Subclass Code.
  \param subclass_name Subclass Name.
  
  \return ID representing the PCI IDs Subclass entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>  
*/
string insertPciIdsSubclass(string class_code, string subclass_code, string subclass_name )
{
  string subclass_id;

  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_subclasses(subClassCode,subClassName,classCode_FK) "
				  "VALUES ('"+ subclass_code +"',"
				  "'"+ subclass_name +"','"+ class_code +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  subclass_id = commonResultSet->getString(1);
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return subclass_id;
}



//! Insert a PCI IDs Prog-if entry in database.
/*!
  Add a PCI IDs Prog-if entry(pci.ids prog-if line) to database..  
  
  \param subclassId ID representing the Prog-if's Subclass in database.
  \param progif_code Prog-if Code.
  \param progif_name Prog-if Name.
  
  \return ID representing the PCI IDs Prog-if entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>  
*/
string insertPciIdsProgif( string subclassId, string progif_code, string progif_name)
{
  string uProgif_id;

  try{
	connectDatabase();

	stmt->execute("INSERT INTO pci_prog_ifs(progifCode,progifName,subClassID_FK)"
				  " VALUES ('"+ progif_code +"','"+ progif_name +"','"+ subclassId +"')");
	commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	if( commonResultSet->next() ){
	  uProgif_id = commonResultSet->getString(1);
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return uProgif_id;
}



//! Query unique ID of a PCI IDs Prog-if entry in database.
/*!
  Retrieve ID of a PCI IDs Prog-if entry(pci.ids prog-if line) stored in database.  
  
  \param class_code Class Code of the Subclass of the Prog-if.
  \param subclass_code Code of the Prog-if's Subclass.
  \param progif_code Prog-if Code.
  
  \return ID representing the PCI IDs Prog-if entry.

  \warning <b>THIS FUNCTION IS NOT USED IN THE APPLICATION!</b>  
*/
string queryPciIdsProgifId(string class_code, string subclass_code, string progif_code)
{
  string uProgifId = "";

  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("CALL sp_queryClassName('"+ class_code +"','"+ subclass_code +"'," +
										 "'" + progif_code  + "')");

	if( commonResultSet->next() ){
	  uProgifId = commonResultSet->getString("uProgifID");
	}
	
  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";

  }
  disconnectDatabase();
  
  return uProgifId;
}



//! Check if the provided PCI device is stored before.
/*!
  Query the `pci_all' database table and see if the PCI device is stored
  before, if not add it to the database. And return it's PCI special ID
  from `pci_all' database table.
  
  \param currentPciDevice PCI device pointing to the instance to be checked.
  
  \return PCI special ID of provided device.
*/
string checkPciSpcId( const PciDevice* const currentPciDevice )
{
  string pciSpcId;

  try{
	connectDatabase();

	// Select PCI special ID of provided device.
	commonResultSet = stmt->executeQuery("SELECT pciSpcID FROM pci_all WHERE "
										 "vendorCode='"+ currentPciDevice->getVendor() + "' AND " +
										 "deviceCode='"+ currentPciDevice->getDevice() + "' AND " +
										 "subvendorCode='"+ currentPciDevice->getSubvendor() + "' AND " +
										 "subdeviceCode='"+ currentPciDevice->getSubdevice() + "' AND " +
										 "classCode='"+ currentPciDevice->getClass() + "' AND " +
										 "subclassCode='"+ currentPciDevice->getSubclass() + "' AND " +
										 "progifCode='"+ currentPciDevice->getProgif() + "'");
	if( commonResultSet->first() ){
	  pciSpcId = commonResultSet->getString("pciSpcID");
	}
	else {						// if device doesn't exist, insert it..
	  stmt->execute("INSERT INTO pci_all"
					"(vendorCode, deviceCode, subvendorCode, subdeviceCode, classCode,subclassCode,progifCode) "
					"VALUES ('"+ currentPciDevice->getVendor() +"'," +
					"'"+ currentPciDevice->getDevice() +"',"+
					"'"+ currentPciDevice->getSubvendor() +"',"+
					"'"+ currentPciDevice->getSubdevice() +"',"+
					"'"+ currentPciDevice->getClass() +"',"+
					"'"+ currentPciDevice->getSubclass() +"',"+
					"'"+ currentPciDevice->getProgif() +"')");

	  // Retrieve the ID given at last insertion which is the currently inserted PCI special ID.
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		pciSpcId = commonResultSet->getString(1);
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
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return pciSpcId;
}



//! Check if the provided device is stored before.
/*!
  Query the `all_devices' database table and see if the device is stored
  before, if not add it to the database.
  If bus type is not provided, it is assumed that it is a PCI device.
  Might be any device depending on bus type, designed for future growth of database with different busses.
  
  \todo FUTURE WORK: Might add different types of buses like USB, PNP..
  
  \param dev_special_id Special Device ID depending on bus type.
  \param bus_type Bus type ID.
  
  \return Unique Device ID of provided device.
*/
string checkUniqueDeviceId( string dev_special_id, int bus_type = 1 )
{
  string uDevId;				// Hold the unique device ID.
  std::stringstream ss;
  ss << bus_type;
  string type = ss.str();

  try{
	connectDatabase();

	// See what type of a bus is the device?
	switch (bus_type){
	  
	case 1:						// If it is a PCI device
	  // See if it is already stored before.
	  commonResultSet = stmt->executeQuery("SELECT uDevID FROM all_devices WHERE busTypeID_FK="+ type +
										   " AND pciSpcID_FK="+ dev_special_id);

	  if( commonResultSet->first() ){
		uDevId = commonResultSet->getString("uDevID");
	  }
	  else {					// If the given PCI device doesn't exist in `all_devices' db table, add it..
		stmt->execute("INSERT INTO all_devices(busTypeID_FK,pciSpcID_FK) VALUES ("+ type +","+ dev_special_id+")");
		commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
		if( commonResultSet->next() ){
		  uDevId = commonResultSet->getString(1);
		}
	  }
	  break;
	  
	  // case 2:						// Might be for USB..
	  // case 3:						// Might be for PNP, whatever..
	}

  }
  catch (sql::SQLException &e) {
	/*
	  The MySQL Connector/C++ throws three different exceptions:
	  - sql::MethodNotImplementedException (derived from sql::SQLException)
	  - sql::InvalidArgumentException (derived from sql::SQLException)
	  - sql::SQLException (derived from std::runtime_error)
	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  
  return uDevId;
}



//! Retrieve all previously saved mainboards.
/*! 
  Retrieve all entries in `board_models' database table.
  
  \return List of (Board ID - Board Name) pairs.
*/
std::multimap<string, string>
getBoardList()
{
  std::multimap<string, string> mainboard_list; 	// Hold list of mainboard entries ( boardID, boardName ).
  
  try{
	connectDatabase();

	// Select all from `board_models' table.
	commonResultSet = stmt->executeQuery("SELECT * FROM board_models");

	while( commonResultSet->next() ){
	  // Insert retrieved entries to mainboard_list.
	  mainboard_list.insert( std::pair<string,string>( commonResultSet->getString("boardID"),
													   commonResultSet->getString("boardName") ) );
	}

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return mainboard_list;
}



//! Query devices belonging to given mainboard.
/*!
  Retrieve a string list of Unique Device IDs belonging to
  the provided mainboard.
  
  \param board_id ID of a Mainboard.
  
  \return List of Unique Device IDs.
*/
std::vector<string> queryBoardDeviceList( string board_id )
{
  std::vector<string> udev_id_list; 			// Hold list of Unique Device IDs to be returned.

  try{
	connectDatabase();
	
	// Select from `dev_board' database table with provided mainboard ID.
	commonResultSet = stmt->executeQuery("SELECT uDevID_FK FROM dev_board WHERE boardID_FK="+ board_id );

	while( commonResultSet->next() ){ 			// Get list of unique device IDs.
	  udev_id_list.push_back( commonResultSet->getString("uDevID_FK") );
	}

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return udev_id_list;
}



//! Query hardware codes of a Device.
/*!
  Retrieve hardware codes of a device depending on it's bus type.
  
  \param udev_id Unique ID of a device.
  \param bus_type Bus type ID.
  
  \return Hardware codes of a device.

  \warning Only works for PCI bus type now which is 1.
  \todo FUTURE WORK: support for additional busses.
*/
std::vector<string> queryDeviceCodes( string udev_id, int bus_type = 1 )
{
  std::vector<string> device_codes; 			// Hold hardware code list of a device to be returned.

  std::stringstream ss;
  ss << bus_type;
  string type = ss.str();

  try{
	connectDatabase();

	switch (bus_type){
	case 1:						// if it is a PCI Device
	  // Find the PCI special ID of the device.
	  commonResultSet = stmt->executeQuery("SELECT pciSpcID_FK FROM all_devices WHERE uDevID="+ udev_id);

	  if( commonResultSet->first() ){
		string pci_spc_id = commonResultSet->getString("pciSpcID_FK");

		// And retrieve it's hardware codes from `pci_all' database table.
		commonResultSet = stmt->executeQuery("SELECT * FROM pci_all WHERE pciSpcID="+ pci_spc_id );
		if( commonResultSet->next() ){
		  device_codes.push_back( commonResultSet->getString("vendorCode") );
		  device_codes.push_back( commonResultSet->getString("deviceCode") );
		  device_codes.push_back( commonResultSet->getString("subvendorCode") );
		  device_codes.push_back( commonResultSet->getString("subdeviceCode") );
		  device_codes.push_back( commonResultSet->getString("classCode") );
		  device_codes.push_back( commonResultSet->getString("subclassCode") );
		  device_codes.push_back( commonResultSet->getString("progifCode") );
		}
	  }
	  break;
	  // case 2:						// Might be for USB..
	  // case 3:						// Might be for PNP, whatever..
	}
  }
  catch (sql::SQLException &e) {
	/*
	  The MySQL Connector/C++ throws three different exceptions:
	  - sql::MethodNotImplementedException (derived from sql::SQLException)
	  - sql::InvalidArgumentException (derived from sql::SQLException)
	  - sql::SQLException (derived from std::runtime_error)
	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return device_codes;
}



//! Retrieve all Distributions.
/*! 
  Retrieve all entries from `OSes' database table.
  
  \return List of ( OS ID - OS Name ) pairs.
*/
std::multimap<string,string>
getOsList()
{
  std::multimap<string, string> os_list; 			// Hold list of OS Distribution entries ( osID, osName ).
  
  try{
	connectDatabase();

	// Select all entries from `OSes' table.
	commonResultSet = stmt->executeQuery("SELECT * FROM OSes");

	while( commonResultSet->next() ){
	  // Insert entries to os_list.
	  os_list.insert( std::pair<string,string>( commonResultSet->getString("osID"),
												commonResultSet->getString("osName") ) );
	}

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();

  return os_list;
}



//! Query Releases of provided OS Distribution.
/*!
  Retrieve list of Releases of the provided Distribution with os_id.
  
  \param os_id ID of an OS Distribution.
  
  \return List of ( release ID - release Name ) pairs.
*/
std::multimap<string,string>
queryOsReleases( string os_id )
{
  std::multimap<string,string> release_list; 	// Hold list of release entries ( releaseID, releaseName ).

  try{
	connectDatabase();

	// Select entries from the `os_releases' table which belong to the Distribution with os_id ID.
	commonResultSet = stmt->executeQuery("SELECT * FROM os_releases WHERE osID_FK="+ os_id );

	while( commonResultSet->next() ){
	  // Insert retrieved entries into release_list.
	  release_list.insert( std::pair<string,string>( commonResultSet->getString("releaseID"),
												commonResultSet->getString("releaseName") ) );
	}

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  
  return release_list;
}



//! Query Kernels of provided Distribution Release.
/*!
  Retrieve Kernel list of provided Distribution Release with release_id ID.
  
  \param release_id Unique ID representing an OS.
  
  \return List of ( Unique Kernel ID - Kernel Version-Arch ) pairs.
*/
std::multimap<std::string,std::string>
queryReleaseKernels( std::string release_id )
{
  std::multimap<string,string> kernel_list; 	// Hold list of Kernels(uKernelID, kernelVersion-machineHardware)

  try{
	connectDatabase();

	// Select entries from the `kernels' table which belong to the Release with release_id ID.
	commonResultSet = stmt->executeQuery("SELECT * FROM kernels WHERE releaseID_FK="+ release_id );

	while( commonResultSet->next() ){
	  // Insert retrieved entries into kernel_list. Put Kernel Version and it's architecture together.
	  kernel_list.insert( std::pair<string,string>( commonResultSet->getString("uKernelID"),
													commonResultSet->getString("kernelVersion") +
													"-" + commonResultSet->getString("machineHardware") ) );
	}

  }
  catch (sql::SQLException &e) {
  	/*
  	  The MySQL Connector/C++ throws three different exceptions:
  	  - sql::MethodNotImplementedException (derived from sql::SQLException)
  	  - sql::InvalidArgumentException (derived from sql::SQLException)
  	  - sql::SQLException (derived from std::runtime_error)
  	*/
	wApp->log("debug") << "# ERR: SQLException in " << __FILE__;
	wApp->log("debug") << "(" << __FUNCTION__ << ") on line " << __LINE__;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
	wApp->log("debug") << "# ERR: " << e.what();
  	wApp->log("debug") << " (MySQL error code: " << e.getErrorCode();
	wApp->log("debug") << ", SQLState: " << e.getSQLState() << " )";
  }
  disconnectDatabase();
  
  return kernel_list;
  
}
