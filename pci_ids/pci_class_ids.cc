/** pci_classes.cc --- 
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

#include "pci_class_ids.hpp"

static void throw_call_garbage(sql::Statement*);

int PciIdsClass::store_db (void)
{
  vector<PciIdsSubclass*>::iterator iter_sub;
  vector<PciIdsProgif*>::iterator iter_prog;

  PciIdsSubclass* tmp_sub;
  PciIdsProgif* tmp_prog;

  const string host_url("localhost");
  const string user("root");
  const string pass("password");
  const string database("harix_db");
  string class_code, class_name, subclass_code, subclass_name, prog_code, prog_name;

  // Could be better with MySQL Prepared Statement
  // but it's buggy operating with Stored Procedures at the moment
  try{
	sql::Driver* driver = get_driver_instance();
	std::auto_ptr<sql::Connection> connector( driver->connect(host_url, user, pass) );
	connector->setSchema(database);
	std::auto_ptr<sql::Statement> stmt(connector->createStatement());

	class_code = "\"" + get_code() + "\"" ;
	class_name = "\"" + get_name() + "\"";
	stmt->execute("CALL sp_pciClassesAdd("+ class_code + "," + class_name + ")");
	throw_call_garbage(stmt.get());
	// std::cout << class_code << " - " << class_name << std::endl; 						// @test

	for( iter_sub = kid_list.begin(); iter_sub != kid_list.end(); ++iter_sub ){
	  tmp_sub = *iter_sub;
	  subclass_code = "\"" + tmp_sub->get_code() + "\"" ;
	  subclass_name = "\"" + tmp_sub->get_name() + "\"";
	  stmt->execute("CALL sp_pciSubClassesAdd("+ class_code + "," + subclass_code + "," + subclass_name + ")");
	  throw_call_garbage(stmt.get());
	  // std::cout << '\t' << subclass_code << " - " << subclass_name << std::endl; 		// @test

	  if( (tmp_sub->progif_list).size() == 0  ){
		prog_code =  "null" ;
		prog_name =  "null" ;
		stmt->execute("CALL sp_pciProgifsAdd("+ class_code + "," + subclass_code + "," + prog_code +  "," + prog_name + ")");
		throw_call_garbage(stmt.get());
		// std::cout << '\t' << '\t' << prog_code << " " << prog_name << std::endl;//@test
		continue;
	  }
	  for( iter_prog = (tmp_sub->progif_list).begin(); iter_prog != (tmp_sub->progif_list).end(); ++iter_prog ){
		  tmp_prog = *iter_prog;
		  prog_code =  "\"" + tmp_prog->get_code() + "\"" ;
		  prog_name =  "\"" + tmp_prog->get_name() + "\"" ;
		  stmt->execute("CALL sp_pciProgifsAdd("+ class_code + "," + subclass_code + "," + prog_code +  "," + prog_name +")");
		  throw_call_garbage(stmt.get());
		  // std::cout << '\t' << '\t' << prog_code << " " << prog_name << std::endl;//@test
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

// Throw the unneeded result returned from the CALL to the SP
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

PciIdsClass::~PciIdsClass()
{
  int subclass_list_size = kid_list.size();

  for ( int k=0; k<subclass_list_size; k++ ){
	delete kid_list.back();
	kid_list.pop_back();
  }

}

PciIdsSubclass::~PciIdsSubclass()
{
  int progif_list_size = progif_list.size();

  for (int k=0; k < progif_list_size; k++){
	delete progif_list.back();
	progif_list.pop_back();
  }
}
