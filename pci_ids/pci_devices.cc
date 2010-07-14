/** pci_devices.cc --- 
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

#include <cppconn/driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "pci_devices.hpp"

using std::string;

static void correct_quotes(string&, string::size_type&, const string&);
static void throw_call_garbage(sql::Statement*);

#ifdef DEBUG
int on_off = 1;
#endif

int pciVendor::store_db (void)
{
  std::vector<pciDevice*>::iterator iter_dev;
  std::vector<pciSubsystem*>::iterator iter_subsys;

  pciDevice* tmp_dev;
  pciSubsystem* tmp_subsys;

  const string host_url("localhost");
  const string user("root");
  const string pass("alp");
  const string database("harix_db");
  string vendor_code, vendor_name, device_code, device_name, subsys_vdr, subsys_dev, subsys_name, tmp_name;
  const string quote = "\"";
  string::size_type qpos;

  // Could be better with MySQL Prepared Statement
  // but it's buggy operating with Stored Procedures at the moment
  
  try{
  	sql::Driver* driver = get_driver_instance();
  	std::auto_ptr<sql::Connection> connector( driver->connect(host_url, user, pass) );
#ifdef DEBUG
	connector->setClientOption("clientTrace", &on_off);
#endif
  	connector->setSchema(database);
  	std::auto_ptr<sql::Statement> stmt(connector->createStatement());

	vendor_code = "\"" + get_code() + "\"" ;
	tmp_name = get_name();
	qpos = tmp_name.find(quote);
	if ( qpos != string::npos )
	  correct_quotes(tmp_name,qpos,quote);

	vendor_name = "\"" + tmp_name + "\"";
	// std::cout << vendor_code << " - " << vendor_name << std::endl; 						// @test
	stmt->execute("CALL sp_pci_VendorsAdd("+ vendor_code + "," + vendor_name + ")");
	throw_call_garbage(stmt.get());

	if ( kid_list.size() == 0 ){
	  // Add an entry in pci_devices for vendors without any device listed
	  device_code = "\"XXXX\"";
	  device_name = "null";
	  // std::cout << '\t' << device_code << " - " << device_name << std::endl; 		// @test
	  stmt->execute("CALL sp_pci_DevicesAdd("+ vendor_code + "," + device_code + "," + device_name + ")");
	  throw_call_garbage(stmt.get());
	  // Add an entry in pci_subsystems for vendors without any device listed
	  subsys_vdr =  "null" ;
	  subsys_dev =  "null" ;
	  subsys_name =  "null" ;
	  stmt->execute("CALL sp_pci_SubsystemsAdd(" + vendor_code + "," + device_code + "," + subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
	  throw_call_garbage(stmt.get());
	  // std::cout << '\t' << '\t' << subsys_vdr << " " << subsys_dev << "-" << subsys_name << std::endl;//@test

	}
	else {
	  for( iter_dev = kid_list.begin(); iter_dev != kid_list.end(); ++iter_dev ){
		tmp_dev = *iter_dev;
		device_code = "\"" + tmp_dev->get_code() + "\"" ;
		tmp_name = tmp_dev->get_name();
		qpos = tmp_name.find(quote);
		if ( qpos != string::npos )
		  correct_quotes(tmp_name,qpos,quote);

		device_name = "\"" + tmp_name + "\"";
		// std::cout << '\t' << device_code << " - " << device_name << std::endl; 		// @test
		stmt->execute("CALL sp_pci_DevicesAdd("+ vendor_code + "," + device_code + "," + device_name + ")");
		throw_call_garbage(stmt.get());

		if( (tmp_dev->subsystem_list).size() == 0  ){
		  subsys_vdr =  "null" ;
		  subsys_dev =  "null" ;
		  subsys_name =  "null" ;
		  stmt->execute("CALL sp_pci_SubsystemsAdd(" + vendor_code + "," + device_code + "," + subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
		  throw_call_garbage(stmt.get());
		  // std::cout << '\t' << '\t' << subsys_vdr << " " << subsys_dev << "-" << subsys_name << std::endl;//@test
		  continue;
		}
		for( iter_subsys = (tmp_dev->subsystem_list).begin(); iter_subsys != (tmp_dev->subsystem_list).end();
			 ++iter_subsys ){
		  tmp_subsys = *iter_subsys;
		  subsys_vdr =  "\"" + tmp_subsys->get_subVendor() + "\"" ;
		  subsys_dev =  "\"" + tmp_subsys->get_subDevice() + "\"" ;
		  tmp_name = tmp_subsys->get_name();
		  qpos = tmp_name.find(quote);
		  if ( qpos != string::npos )
			correct_quotes(tmp_name,qpos,quote);

		  subsys_name =  "\"" + tmp_name + "\"" ;
		  // std::cout << '\t' << '\t' << subsys_vdr << " " << subsys_dev << "-" << subsys_name << std::endl;//@test
		  stmt->execute("CALL sp_pci_SubsystemsAdd("+ vendor_code + "," + device_code + "," + subsys_vdr + "," + subsys_dev + "," + subsys_name + ")");
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

static void correct_quotes(string& tmp_str, string::size_type& qpos, const string& quote)
{
  do{
	tmp_str.replace(qpos, quote.size(), "\\\"" );
	qpos = tmp_str.find(quote, qpos+2);
  }while(qpos != string::npos);
}


// Throw the unneeded result returned from the SP CALL
static void throw_call_garbage(sql::Statement* normal_stmt)
{
  std::auto_ptr< sql::ResultSet > res;
  do {
	res.reset(normal_stmt->getResultSet());
	while (res->next()) {
#ifdef DEBUG
	  std::cout << res->getInt(1) << " -> expected value is 0" << std::endl;
#endif
	}
  } while (normal_stmt->getMoreResults());
}


pciVendor::~pciVendor()
{
  int device_list_size = kid_list.size();

  for ( int k=0; k < device_list_size; k++ ){
	delete kid_list.back();
	kid_list.pop_back();
  }

}

pciDevice::~pciDevice()
{
  int subsystem_list_size = subsystem_list.size();

  for (int k=0; k < subsystem_list_size; k++){
	delete subsystem_list.back();
	subsystem_list.pop_back();
  }
}
