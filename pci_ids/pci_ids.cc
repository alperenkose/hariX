#include <fstream>
#include <cstring>
#include <iostream>
#include <list>

#include <Wt/WLogger>
#include "../harixApp.hpp"

#include "pci_devices.hpp"
#include "pci_classes.hpp"

std::list<pciVendor> vendor_list;

void add_vendor (std::string);
pciDevice* add_device (std::string, pciVendor*);
void add_subsys (std::string, pciDevice*);
int store_pci_all (void);
void delete_pci_all(void);


list<pciClass> class_list;

void add_class (string);
pciSubclass* add_subclass (string, pciClass*);
void add_progif (string, pciSubclass*);
int store_classes (void);
void delete_classes(void);



int update_device_codes( std::string pci_ids_file )
{
  pciDevice* last_device;
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
			last_device = add_device(analyze_line, &(vendor_list.back()));
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

  pciVendor* new_vendor = new pciVendor(vendor_code, vendor_name);
  vendor_list.push_back(*new_vendor);		// here a copy of new_vendor is added to list
  delete new_vendor;
}

//void add_device (std::string analyze)
pciDevice* add_device (std::string analyze, pciVendor* parent_vendor)
{
  std::string dev_code, dev_name;
  std::string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  dev_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  dev_name = analyze.substr(beg_pos);

  //  std::cout << '\t' << dev_code << " - " << dev_name << std::endl; // @test
  pciDevice* new_device = new pciDevice(parent_vendor, dev_code, dev_name);
  return new_device;
}

// void add_subsys (std::string analyze)
void add_subsys (std::string analyze, pciDevice* parent_device)
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
  pciSubsystem* new_subsys = new pciSubsystem(parent_device, subsys_vdr, subsys_dev, subsys_name);
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
  std::list<pciVendor>::iterator iter;

  for( iter = vendor_list.begin(); iter != vendor_list.end(); ++iter ){
  	if ( iter->store_db() != 0 )
	  return 1;
  }
  return 0;

}



int update_class_codes( std::string pci_ids_file )
{
  pciSubclass* last_subclass;
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

  pciClass* new_class = new pciClass(class_code, class_name);
  class_list.push_back(*new_class);		// here a copy of new_class is added to list
  delete new_class;
}

pciSubclass* add_subclass (string analyze, pciClass* parent_class)
{
  string sub_code, sub_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  sub_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  sub_name = analyze.substr(beg_pos);

  pciSubclass* new_subclass = new pciSubclass(parent_class, sub_code, sub_name);
  return new_subclass;
}

void add_progif (string analyze, pciSubclass* parent_subclass)
{
  string prog_code, prog_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  prog_code = analyze.substr( beg_pos, end_pos-beg_pos );

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  prog_name = analyze.substr(beg_pos);

  pciProgif* new_progif = new pciProgif(parent_subclass, prog_code, prog_name);
}


int store_classes(void)
{
  list<pciClass>::iterator iter;

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



int update_pci_ids( std::string pci_ids_file )
{
  if ( update_device_codes( pci_ids_file ) != 0 )
	return 1;

  if ( update_class_codes( pci_ids_file ) != 0 )
	return 1;

  return 0;
}
