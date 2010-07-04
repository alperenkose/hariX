#include <cppconn/driver.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>
#include <vector>
#include <map>

using std::string;

const string host_url("hosuttur");
const string user("root");
const string pass("alp");
const string database("bugware0");


sql::ResultSet* commonResultSet;
sql::Connection* connector;
sql::Statement* stmt;

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
  	std::cout << "# ERR: SQLException in " << __FILE__;
  	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	std::cout << "# ERR: " << e.what();
  	std::cout << " (MySQL error code: " << e.getErrorCode();
  	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

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
  	std::cout << "# ERR: SQLException in " << __FILE__;
  	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	std::cout << "# ERR: " << e.what();
  	std::cout << " (MySQL error code: " << e.getErrorCode();
  	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

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
  	std::cout << "# ERR: SQLException in " << __FILE__;
  	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	std::cout << "# ERR: " << e.what();
  	std::cout << " (MySQL error code: " << e.getErrorCode();
  	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

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
  	std::cout << "# ERR: SQLException in " << __FILE__;
  	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	std::cout << "# ERR: " << e.what();
  	std::cout << " (MySQL error code: " << e.getErrorCode();
  	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

  }
  disconnectDatabase();

  return full_os_details;
  
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
  	std::cout << "# ERR: SQLException in " << __FILE__;
  	std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
  	std::cout << "# ERR: " << e.what();
  	std::cout << " (MySQL error code: " << e.getErrorCode();
  	std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

  }
  disconnectDatabase();

  return module_name;

  
  
}
