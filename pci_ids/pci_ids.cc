/*!
  \file   pci_ids.cc
  \date   Thu Jul 15 17:44:02 2010
  
  \brief  Store and Update PCI and Class IDs
  
  Updates database with the given pci.ids file.
  All PCI Vendors, Devices, Subsystems together with Classes, Subclasses and Prog-if's
  are saved in database if not exists, or updated if any change happened in name.
*/
#include <fstream>
#include <cstring>
#include <iostream>
#include <list>

#include <Wt/WLogger>
#include "../harixApp.hpp"

#include "pci_device_ids.hpp"
#include "pci_class_ids.hpp"

std::list<PciIdsVendor> vendor_list;

void add_vendor (std::string);
PciIdsDevice* add_device (std::string, PciIdsVendor*);
void add_subsys (std::string, PciIdsDevice*);
int store_pci_all (void);
void delete_pci_all(void);


list<PciIdsClass> class_list;

void add_class (string);
PciIdsSubclass* add_subclass (string, PciIdsClass*);
void add_progif (string, PciIdsSubclass*);
int store_classes (void);
void delete_classes(void);



int update_device_codes( std::string pci_ids_file )
{
  PciIdsDevice* last_device;
  std::string analyze_line;
  std::string hex_chars = "0123456789abcdef";
  const int LSIZE = 200;
  char read_linec[LSIZE];
  int position = 0;
  std::ifstream pci_ids(pci_ids_file.c_str());
  if(!pci_ids){
	wApp->log("debug") << "Could not open pci.ids!!!";
	return 1;
  }
  
  while( !pci_ids.eof() ){
	pci_ids.getline(read_linec, LSIZE);

	if( strcspn(read_linec, hex_chars.c_str() ) != 0 )
	  continue;
	else{
	  position = pci_ids.tellg() - strlen(read_linec) - 1;
	  break;
	}
  }
  // @TODO: What-if there is no line beginning with Hex chars, put exception..
  
  pci_ids.seekg(position);

  while ( !pci_ids.eof() ){
	pci_ids.getline( read_linec, LSIZE );
	analyze_line = read_linec;

	if ( !analyze_line.empty() && ( hex_chars.find(analyze_line.at(0)) != std::string::npos ) ) {
	  add_vendor(analyze_line);	

	  while(1){
		pci_ids.getline( read_linec, LSIZE );
		analyze_line = read_linec;
		if ( analyze_line.empty() || analyze_line.at(0) == '#' )
		  continue;
		if ( !analyze_line.empty() && analyze_line.at(0) == '\t' ){
		  if ( analyze_line.at(1) != '\t' ){
			PciIdsVendor& list_last = vendor_list.back();
			last_device = add_device( analyze_line, &list_last );
			//			add_device(analyze_line);						// @test
		  }
		  else if ( analyze_line.at(1) == '\t' ){
			add_subsys(analyze_line, last_device);
			//			add_subsys(analyze_line); // @test
		  }
		  else	// empty line or end-of-file
			break;
		}
		else	// empty line or end-of-file
		  break;
	  }	// end-of-while(1)
	  position = pci_ids.tellg() - strlen(read_linec) - 1;
	  pci_ids.seekg(position);
	}
  }

  // store Devices in the database
  if ( store_pci_all() != 0 ){
	delete_pci_all();
	return 1;
  }

  // iterate in vendor_list and delete all objects
  delete_pci_all();

  return 0;
}

void add_vendor (std::string analyze)
{
  std::string vendor_code, vendor_name;
  std::string::size_type beg_pos, end_pos;
  end_pos = analyze.find_first_of(" ");
  vendor_code = analyze.substr( 0, end_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  vendor_name = analyze.substr(beg_pos);

  PciIdsVendor* new_vendor = new PciIdsVendor(vendor_code, vendor_name);
  vendor_list.push_back(*new_vendor);		// here a copy of new_vendor is added to list
  delete new_vendor;
}

//void add_device (std::string analyze)
PciIdsDevice* add_device (std::string analyze, PciIdsVendor* parent_vendor)
{
  std::string dev_code, dev_name;
  std::string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  dev_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  dev_name = analyze.substr(beg_pos);

  //  std::cout << '\t' << dev_code << " - " << dev_name << std::endl; // @test
  PciIdsDevice* new_device = new PciIdsDevice(parent_vendor, dev_code, dev_name);
  return new_device;
}

// void add_subsys (std::string analyze)
void add_subsys (std::string analyze, PciIdsDevice* parent_device)
{
  std::string subsys_vdr, subsys_dev, subsys_name;
  std::string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  subsys_vdr = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  subsys_dev = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ", end_pos);
  subsys_name = analyze.substr(beg_pos);
  
  //  std::cout << '\t'<< '\t' << subsys_vdr << " " << subsys_dev << " - " << subsys_name << std::endl; // @test
  PciIdsSubsys* new_subsys = new PciIdsSubsys(parent_device, subsys_vdr, subsys_dev, subsys_name);
}


void delete_pci_all(void)
{
  int vendor_list_size = vendor_list.size();
  
  for( int j=0; j<vendor_list_size; j++ ){
	vendor_list.pop_front();		// STL pop calls the destructor of the current object
  }

}

int store_pci_all(void)
{
  std::list<PciIdsVendor>::iterator iter;

  for( iter = vendor_list.begin(); iter != vendor_list.end(); ++iter ){
  	if ( iter->store_db() != 0 )
	  return 1;
  }
  return 0;

}



int update_class_codes( std::string pci_ids_file )
{
  PciIdsSubclass* last_subclass;
  string analyze_line;
  const int LSIZE = 200;
  char read_linec[LSIZE];
  int position = 0;
  ifstream class_ids(pci_ids_file.c_str());
  if(!class_ids){
	wApp->log("debug") << "Could not open pci.ids!!!";
	return 1;
  }

  while( !class_ids.eof() ){
	class_ids.getline(read_linec, LSIZE);
	
	// regexp might be used with C++ TR1
	if( read_linec[0] != 'C' )
	  continue;
	else{
	  position = class_ids.tellg() - strlen(read_linec) - 1;
	  break;
	}
  }
  // @TODO: What-if there is no line beginning with "C", put exception..
  
  class_ids.seekg(position);

  while ( !class_ids.eof() ){
	class_ids.getline( read_linec, LSIZE );
	analyze_line = read_linec;

		
	if ( !analyze_line.empty() && analyze_line.at(0) == 'C'){
	  add_class(analyze_line);

	  do{
		class_ids.getline( read_linec, LSIZE );
		analyze_line = read_linec;
		if ( !analyze_line.empty() && analyze_line.at(0) == '\t' ){
		  if ( analyze_line.at(1) != '\t' ){
			last_subclass = add_subclass(analyze_line, &(class_list.back()));
		  }
		  else if ( analyze_line.at(1) == '\t' ){
			add_progif(analyze_line, last_subclass);
		  }
		  else	// empty line or end-of-file
			break;
		}
		else	// empty line or end-of-file
		  break;
		
	  } while( analyze_line.at(0) !=  'C' );
	  position = class_ids.tellg() - strlen(read_linec) - 1;
	  class_ids.seekg(position);
	}
  }

  // store Classes in the database
  if( store_classes() != 0 ){
	delete_classes();
	return 1;
  }

  // iterate in class_list and delete all objects
  delete_classes();

  return 0;
}

void add_class (string analyze)
{
  string class_code, class_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_of(" ");
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  class_code = analyze.substr( beg_pos+1 , end_pos-beg_pos-1 );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  class_name = analyze.substr(beg_pos);

  PciIdsClass* new_class = new PciIdsClass(class_code, class_name);
  class_list.push_back(*new_class);		// here a copy of new_class is added to list
  delete new_class;
}

PciIdsSubclass* add_subclass (string analyze, PciIdsClass* parent_class)
{
  string sub_code, sub_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  sub_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  sub_name = analyze.substr(beg_pos);

  PciIdsSubclass* new_subclass = new PciIdsSubclass(parent_class, sub_code, sub_name);
  return new_subclass;
}

void add_progif (string analyze, PciIdsSubclass* parent_subclass)
{
  string prog_code, prog_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  prog_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  prog_name = analyze.substr(beg_pos);

  PciIdsProgif* new_progif = new PciIdsProgif(parent_subclass, prog_code, prog_name);
}


int store_classes(void)
{
  list<PciIdsClass>::iterator iter;

  for( iter = class_list.begin(); iter != class_list.end(); ++iter ){
  	if( iter->store_db() != 0 )
	  return 1;
  }

  return 0;
}

void delete_classes(void)
{
  int class_list_size = class_list.size();
  
  for( int j=0; j<class_list_size; j++ ){
	class_list.pop_front();		// STL pop calls the destructor of the current object
  }

}


//! Update database with the given 'pci.ids' file.
/*!

  \param pci_ids_file Path to the 'pci.ids' file
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int update_pci_ids( std::string pci_ids_file )
{
  if ( update_device_codes( pci_ids_file ) != 0 )
	return 1;

  if ( update_class_codes( pci_ids_file ) != 0 )
	return 1;

  return 0;
}
