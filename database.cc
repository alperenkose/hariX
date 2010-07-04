#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <vector>
#include <map>
#include <string>


#include "os_info.hpp"
#include "pci_device.hpp"
#include "harixApp.hpp"			// @test
#include <Wt/WLogger>

using std::string;

const string host_url("hosuttur");
const string user("root");
const string pass("alp");
const string database("bugware0");


static sql::ResultSet* commonResultSet;
static sql::Connection* connector;
static sql::Statement* stmt;

inline void connectDatabase()
{
  sql::Driver* driver = get_driver_instance();
  connector = driver->connect(host_url, user, pass);
  connector->setSchema(database);
  stmt = connector->createStatement();
}

inline void disconnectDatabase()
{
  delete commonResultSet;
  delete stmt;
  delete connector;
}


std::vector<string> queryClassName(string class_code, string subclass_code, string progif_code)
{
  std::vector<string> full_class_name;
  
  try{
	connectDatabase();

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


std::vector<string>
queryDeviceName(string vendor_code, string device_code, string subvendor_code, string subdevice_code)
{
  std::vector<string> full_device_name;
  
  try{
	connectDatabase();

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


std::multimap<string, string>
queryPcimapOsList(string vendor_code, string device_code, string subvendor_code, string subdevice_code,
			   string class_code, string subclass_code, string progif_code)
{
  std::multimap<string,string> ukernel_module_map; // ( unique_kernel_id, module_name_id )
  
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("CALL sp_queryPcimap('"+ vendor_code +"','"+ device_code +"'," +
										 "'" + subvendor_code + "','" + subdevice_code + "'," +
										 "'" + class_code + subclass_code + "','" + progif_code + "')");

	while( commonResultSet->next() ){
	  // ukernel_module_map[ commonResultSet->getString("uKernelID") ] = commonResultSet->getString("modNameID");
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

string queryOsKernelId( string os_name, string release, string kernel, string architecture )
{
  string ukernel_id="";
  // release="2010-02-15";			// @test
  // kernel="2.6.32-ARCH";			// @test
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

string queryModuleName( string mod_name_id )
{
  string module_name;
  
  try{
	connectDatabase();

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


string recordOsInfo (OsInfo osInfo)
{
  string osId_="", releaseId_="", uKernelId_="";
  try{
	connectDatabase();
	/**
	 * Check OS Name entry in DB
	 * @{
	 */
	commonResultSet = stmt->executeQuery("SELECT osID FROM OSes WHERE osName='"+ osInfo.getDistro() +"'");
	if( commonResultSet->first() ){
	  osId_ = commonResultSet->getString("osID");
	}
	else{
	  // Insert Distribution into OSes and retrieve osID
	  stmt->execute("INSERT INTO OSes(osName) VALUES ('"+ osInfo.getDistro() +"')");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		osId_ = commonResultSet->getString(1);
	  }
	}
	assert( osId_ != "" );
	/**
	 * @}
	 */


	/**
	 * Check Release entry in DB
	 * @{
	 */
	commonResultSet = stmt->executeQuery("SELECT releaseID FROM os_releases WHERE osID_FK="+ osId_ +
										 " AND releaseName='"+ osInfo.getRelease() +"'");
	if( commonResultSet->first() ){
	  releaseId_ = commonResultSet->getString("releaseID");
	}
	else{
	  // Insert Dist.Release into os_releases and retrieve releaseID
	  stmt->execute("INSERT INTO os_releases(osID_FK,releaseName) VALUES("+ osId_ +",'"+ osInfo.getRelease() +"')");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		releaseId_ = commonResultSet->getString(1);
	  }
	}
	assert( releaseId_ != "" );
	/**
	 * @}
	 */

  
	/**
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
	/**
	 * @}
	 */
	wApp->log("debug") << "UKERNEL INSERTED!!!"; 									// @test
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


string checkModuleNameId( string module_name )
{
  string mod_name_id;
  try{
	connectDatabase();
	commonResultSet = stmt->executeQuery("SELECT modNameID FROM module_names WHERE modName='"
										 + module_name +"'");
	if( commonResultSet->first() ){
	  mod_name_id = commonResultSet->getString("modNameID");
	}
	else {
	  stmt->execute("INSERT INTO module_names(modName) VALUES ('"+ module_name +"')");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		mod_name_id = commonResultSet->getString(1);
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


string checkKernelModule (string uKernelId, string modNameId)
{
  string uniqueModuleId_="";
  try{
	connectDatabase();
	
	commonResultSet = stmt->executeQuery("SELECT uModID FROM modules WHERE uKernelID_FK="+ uKernelId +
										 " AND modNameID_FK="+ modNameId);
	if( commonResultSet->first() ){
	  uniqueModuleId_ = commonResultSet->getString("uModID");
	}
	else {
	  stmt->execute("INSERT INTO modules(uKernelID_FK, modNameID_FK) VALUES ("+ uKernelId +","+ modNameId +")");
	  commonResultSet = stmt->executeQuery("SELECT LAST_INSERT_ID()");
	  if( commonResultSet->next() ){
		uniqueModuleId_ = commonResultSet->getString(1);
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


void insertPcimap ( const PciDevice* const currentPciDevice, std::string uniqueModuleId )
{
  try{
	connectDatabase();

	commonResultSet = stmt->executeQuery("SELECT * FROM pcimap WHERE uModID_FK="+ uniqueModuleId +
										 " AND vendor='"+ currentPciDevice->getVendor() +"'"+
										 " AND device='"+ currentPciDevice->getDevice() +"'"+
										 " AND subvendor='"+ currentPciDevice->getSubvendor() +"'"+
										 " AND subdevice='"+ currentPciDevice->getSubdevice() +"'"+
 " AND class='"+ currentPciDevice->getClass() + currentPciDevice->getSubclass() + currentPciDevice->getProgif() +"'"+
										 " AND classMask='"+ currentPciDevice->getClassMask() +"'");

	if( !commonResultSet->first() ){
	  std::string insertPciEntry = 
		"INSERT INTO pcimap VALUES ("+ uniqueModuleId  +",'"+ currentPciDevice->getVendor() +"',"
		+"'"+ currentPciDevice->getDevice() +"','"+ currentPciDevice->getSubvendor() +"',"
		+"'"+ currentPciDevice->getSubdevice() +"',"
		+"'"+ currentPciDevice->getClass() + currentPciDevice->getSubclass() + currentPciDevice->getProgif() +"',"
		+"'"+ currentPciDevice->getClassMask() + "')";
	  stmt->execute(insertPciEntry);
	  wApp->log("debug") << "INSERTED PCIMAP ENTRY!!!"; 					// @test
	  // @TODO: check success of insertion
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
