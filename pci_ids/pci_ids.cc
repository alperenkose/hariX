/*!
  \file   pci_ids.cc
  \date   Thu Jul 15 17:44:02 2010
  
  \brief  Store and Update PCI and Class IDs
  
  Updates database with the given pci.ids file.
  All PCI Vendors, Devices, Subsystems together with Classes, Subclasses and Prog-if's
  are saved in database if not exists, or updated if any change happened in name.

  A PciIdsVendor object is created for each vendor name in 'pci.ids' file, and
  for every device of that vendor a PciIdsDevice object is created which have also
  their PciIdsSubsys objects created for each subsystem.

  A PciIdsClass object is created for each class in 'pci.ids' file, and for every
  subclass of that class a PciIdsSubclass object is created which have also their
  PciIdsProgif objects created for each programming interface.
*/
#include <fstream>
#include <cstring>
// #include <iostream>
#include <list>

#include <Wt/WLogger>
#include "../harixApp.hpp"

#include "pci_device_ids.hpp"
#include "pci_class_ids.hpp"

//! Hold list of Vendors in 'pci.ids' file.
std::list<PciIdsVendor> vendor_list;

/*
  Declaration of local functions about PCI device codes.
*/
void add_vendor (std::string);
PciIdsDevice* add_device (std::string, PciIdsVendor*);
void add_subsys (std::string, PciIdsDevice*);
int store_pci_all (void);
void delete_pci_all(void);



//! Hold list of Classes in 'pci.ids' file.
list<PciIdsClass> class_list;


/*
  Declaration of local functions about PCI class codes.
*/
void add_class (string);
PciIdsSubclass* add_subclass (string, PciIdsClass*);
void add_progif (string, PciIdsSubclass*);
int store_classes (void);
void delete_classes(void);




//! Manage storing of PCI Device codes.
/*!
  Read PCI Device codes(Vendor - Device - Subvendor - Subdevice)
  from the 'pci.ids' file into object of classes PciIdsVendor,
  PciIdsDevice and PciIdsSubsys. Then call store_pci_all() function,
  to iterate through these to store in database.
  
  \param pci_ids_file Path to the 'pci.ids' file.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int update_device_codes( std::string pci_ids_file )
{
  PciIdsDevice* last_device;

  std::string hex_chars = "0123456789abcdef"; 		// String of hexadecimal chars to check.
  
  const int LSIZE = 200;
  char read_linec[LSIZE];							// Hold the line read as an array of char.
  std::string analyze_line;							// Hold the line read as string.
  
  int position = 0;
  
  std::ifstream pci_ids(pci_ids_file.c_str()); 			// Open the 'pci.ids' file for reading.
  if(!pci_ids){
	wApp->log("debug") << "Could not open pci.ids!!!";
	return 1;
  }
  
  while( !pci_ids.eof() ){							   	// Read continuously
	pci_ids.getline(read_linec, LSIZE);

	if( strcspn(read_linec, hex_chars.c_str() ) != 0 ) 	// if first char of line is not an HEX, continue
	  continue;
	else{												// else if we match an HEX at first char,
	  position = pci_ids.tellg() - strlen(read_linec) - 1; // get the position of first match.
	  break;
	}
  }
  // @TODO: What-if there is no line beginning with Hex chars, put exception..

  
  pci_ids.seekg(position); 				// Seek back to the first HEX match, cause pointer is at next line.

  

  while ( !pci_ids.eof() ){					// Read continuously
	pci_ids.getline( read_linec, LSIZE );
	analyze_line = read_linec;

	// If line is not empty and first character is an HEX
	if ( !analyze_line.empty() && ( hex_chars.find(analyze_line.at(0)) != std::string::npos ) ) {
	  
	  add_vendor(analyze_line);				// Then this is a vendor, so add it.

	  while(1){								// loop for the Devices and Subsys' of last read Vendor.
		pci_ids.getline( read_linec, LSIZE );
		analyze_line = read_linec;

		// if there is an empty or comment line, skip it..
		if ( analyze_line.empty() || analyze_line.at(0) == '#' )
		  continue;
		
		// So if the line is NOT empty, and first character is a TAB, then either a Device or Subsystem.
		if ( !analyze_line.empty() && analyze_line.at(0) == '\t' ){
		  
		  if ( analyze_line.at(1) != '\t' ){ 						// So this is a Device entry line.
			
			PciIdsVendor& list_last = vendor_list.back(); 			// Get the last read Vendor.

			// Add device providing it's vendor, and retrieve a pointer to it.
			last_device = add_device( analyze_line, &list_last );
			
		  }
		  else if ( analyze_line.at(1) == '\t' ){ 					// There are 2 TABs, so a Subsystem entry.
			
			// Add subsystem providing it's device.
			add_subsys(analyze_line, last_device);
			
		  }
		  else	// end-of-file
			break;
		  
		}
		else	// Vendor line, empty line or end-of-file
		  break;
		
	  }	// end-of-while(1)

	  // Last read line is not a device or subsystem line, so rewind it to see if it's a Vendor line.
	  position = pci_ids.tellg() - strlen(read_linec) - 1;
	  pci_ids.seekg(position);
	  
	}
  }	// end-of while ( !pci_ids.eof() )


  if ( store_pci_all() != 0 ){	// Store PCI Device Codes in the database
	delete_pci_all();
	return 1;					// return 1 if we fail..
  }


  delete_pci_all();				// Iterate in vendor_list and delete all objects

  return 0;
}



//! Create a Pci IDs Vendor object.
/*!
  Read Vendor from the given 'pci.ids' vendor line,
  create a PciIdsVendor object and add it to the #vendor_list.
  
  \param analyze Vendor line from 'pci.ids'.
*/
void add_vendor (std::string analyze)
{
  std::string vendor_code, vendor_name;
  std::string::size_type beg_pos, end_pos;
  end_pos = analyze.find_first_of(" ");
  vendor_code = analyze.substr( 0, end_pos ); 		// Substract the Vendor Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  vendor_name = analyze.substr(beg_pos); 			// Substract the Vendor Name from line.

  
  PciIdsVendor* new_vendor = new PciIdsVendor(vendor_code, vendor_name);
  vendor_list.push_back(*new_vendor);				// Add a copy of the Vendor instace to vendor_list.
  delete new_vendor;
}



//! Create a Pci IDs Device object.
/*!
  Read Device details from the given 'pci.ids' device line and
  create a PciIdsDevice object providing it's Vendor.
  
  \param analyze Device line from 'pci.ids'.
  \param parent_vendor Pointer to the Vendor object of Device.
  
  \return The created Pci IDs Device object.
*/
PciIdsDevice* add_device (std::string analyze, PciIdsVendor* parent_vendor)
{
  std::string dev_code, dev_name;
  std::string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  dev_code = analyze.substr( beg_pos, end_pos-beg_pos ); 	// Substract the Device Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  dev_name = analyze.substr(beg_pos); 						// Substract the Device Name from line.

  // Create a Pci IDs Device object providing a pointer to it's Vendor object.
  PciIdsDevice* new_device = new PciIdsDevice(parent_vendor, dev_code, dev_name);
  return new_device;			// The created Pci IDs Device object.
}



//! Create a Pci IDs Subsystem object.
/*!
  Read Subsystem details from the given 'pci.ids' subsystem line and
  create a PciIdsSubsys object providing it's Device.
  
  \param analyze Subsystem line from 'pci.ids'.
  \param parent_device Pointer to the Device object of Subsystem.
*/
void add_subsys (std::string analyze, PciIdsDevice* parent_device)
{
  std::string subsys_vdr, subsys_dev, subsys_name;
  std::string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  subsys_vdr = analyze.substr( beg_pos, end_pos-beg_pos ); 	// Substract the Subvendor Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  subsys_dev = analyze.substr( beg_pos, end_pos-beg_pos ); 	// Substract the Subdevice Code from line.

  beg_pos = analyze.find_first_not_of(" ", end_pos);
  subsys_name = analyze.substr(beg_pos); 					// Substract the Subsystem Name from line.
  
  // Create a Pci IDs Subsystem object providing a pointer to it's Device object.
  PciIdsSubsys* new_subsys = new PciIdsSubsys(parent_device, subsys_vdr, subsys_dev, subsys_name);
}



//! Delete objects holding Pci IDs Device Codes.
/*!
  Loop through the list of #vendor_list and delete every instance.
  
  On deletion of a PciIdsVendor object, it deletes it's PciIdsDevice objects.
  And on deletion of a PciIdsDevice object, it deletes it's PciIdsSubsys objects.
  So nothing left behind!
 */
void delete_pci_all(void)
{
  int vendor_list_size = vendor_list.size();
  
  for( int j=0; j<vendor_list_size; j++ ){
	vendor_list.pop_front(); 					// STL pop calls the destructor of the current PciIdsVendor object
  }

}



//! Store PCI Device Codes.
/*! 
  Iterate through the list of #vendor_list and
  call PciIdsVendor::store_db() function of each instance
  which will save the vendor and all it's Pci IDs Device Codes(Devices-Subsystems)
  to database.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int store_pci_all(void)
{
  std::list<PciIdsVendor>::iterator iter;

  for( iter = vendor_list.begin(); iter != vendor_list.end(); ++iter ){
  	if ( iter->store_db() != 0 ) // Store Vendor together with it's Devices and Subsystems of the Devices.
	  return 1;
  }
  return 0;

}




//! Manage storing of PCI Class codes.
/*!
  Read PCI Class codes(Class - Subclass - Progif)
  from the 'pci.ids' file into object of classes PciIdsClass,
  PciIdsSubclass and PciIdsProgif. Then call store_classes() function,
  to iterate through these to store in database.
  
  \param pci_ids_file Path to the 'pci.ids' file.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int update_class_codes( std::string pci_ids_file )
{
  PciIdsSubclass* last_subclass;
  
  const int LSIZE = 200;
  char read_linec[LSIZE];						// Hold the line read as an array of char.
  string analyze_line;							// Hold the line read as string.

  int position = 0;

  ifstream class_ids(pci_ids_file.c_str()); 	// Open the 'pci.ids' file for reading.
  if(!class_ids){
	wApp->log("debug") << "Could not open pci.ids!!!";
	return 1;
  }

  while( !class_ids.eof() ){					// Read continuously
	class_ids.getline(read_linec, LSIZE);
	
	// @TODO: regexp might be used with C++ TR1
	if( read_linec[0] != 'C' )					// if first char of line is not 'C', continue
	  continue;
	else{										// else if we match a 'C' at first char,
	  position = class_ids.tellg() - strlen(read_linec) - 1; // get the position of first match.
	  break;
	}
  }
  // @TODO: What-if there is no line beginning with "C", put exception..

  
  class_ids.seekg(position); 	// Seek back to the first 'C' match, cause pointer is at next line.

  

  while ( !class_ids.eof() ){				// Read continuously
	class_ids.getline( read_linec, LSIZE );
	analyze_line = read_linec;

	// If line is not empty and first character is a 'C'		
	if ( !analyze_line.empty() && analyze_line.at(0) == 'C'){
	  
	  add_class(analyze_line);				// Then this is a Class, so add it.

	  do{									// loop for the Subclasses and Progifs of last read Class.
		class_ids.getline( read_linec, LSIZE );
		analyze_line = read_linec;

		//! \todo Put skip for empty and comment lines as in update-device-codes.

		// So if the line is NOT empty, and first character is a TAB, then either Subclass or Prog-if.		
		if ( !analyze_line.empty() && analyze_line.at(0) == '\t' ){
		  
		  if ( analyze_line.at(1) != '\t' ){ 				// So this is a Subclass entry line.

			// Add Subclass providing it's Class, and retrieve a pointer to it.
			last_subclass = add_subclass(analyze_line, &(class_list.back()));
			
		  }
		  else if ( analyze_line.at(1) == '\t' ){ 			// There are 2 TABs, so a Prog-if entry.

			// Add Prog-if providing it's Subclass.
			add_progif(analyze_line, last_subclass);
			
		  }
		  else	// end-of-file
			break;
		}
		else	// Class line, empty line or end-of-file
		  break;
		
	  } while( analyze_line.at(0) !=  'C' );

	  // Last read line is not a Subclass or Prog-if line, so rewind it to see if it's a Class line.	  
	  position = class_ids.tellg() - strlen(read_linec) - 1;
	  class_ids.seekg(position);
	  
	}
  }	// end-of while ( !pci_ids.eof() )

  

  if( store_classes() != 0 ){	// Store PCI Class Codes in the database
	delete_classes();
	return 1;					// return 1 if we fail..
  }


  delete_classes();				// Iterate in class_list and delete all objects

  return 0;
}



//! Create a Pci IDs Class object.
/*!
  Read Class from the given 'pci.ids' class line,
  create a PciIdsClass object and add it to the #class_list.
  
  \param analyze Class line from 'pci.ids'.
*/
void add_class (string analyze)
{
  string class_code, class_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_of(" ");
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  class_code = analyze.substr( beg_pos+1 , end_pos-beg_pos-1 ); // Substract the Class Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  class_name = analyze.substr(beg_pos); 						// Substract the Class Name from line.

  PciIdsClass* new_class = new PciIdsClass(class_code, class_name);
  class_list.push_back(*new_class);					// Add a copy of the Vendor instace to vendor_list.
  delete new_class;
}



//! Create a Pci IDs Subclass object.
/*!
  Read Subclass from the given 'pci.ids' subclass line and
  create a PciIdsSubclass object providing it's Class.
  
  \param analyze Subclass line from 'pci.ids'.
  \param parent_class Pointer to the Class object of Subclass.
  
  \return The created Pci IDs Subclass object.
*/
PciIdsSubclass* add_subclass (string analyze, PciIdsClass* parent_class)
{
  string sub_code, sub_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  sub_code = analyze.substr( beg_pos, end_pos-beg_pos ); 	// Substract the Subclass Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  sub_name = analyze.substr(beg_pos); 						// Substract the Subclass Name from line.

  // Create a Pci IDs Subclass object providing a pointer to it's Class object.
  PciIdsSubclass* new_subclass = new PciIdsSubclass(parent_class, sub_code, sub_name);
  return new_subclass;			// The created Pci IDs Subclass object.
}



//! Create a Pci IDs Prog-if object.
/*!
  Read Programming interface from the given 'pci.ids' prog-if line and
  create a PciIdsProgif object providing it's Subclass.
  
  \param analyze Prog-if line from 'pci.ids'.
  \param parent_subclass Pointer to the Subclass object of Prog-if.
*/
void add_progif (string analyze, PciIdsSubclass* parent_subclass)
{
  string prog_code, prog_name;
  string::size_type beg_pos, end_pos;
  beg_pos = analyze.find_first_not_of('\t');
  end_pos = analyze.find_first_of(" ", beg_pos+1);
  prog_code = analyze.substr( beg_pos, end_pos-beg_pos ); 	// Substract the Prog-if Code from line.

  beg_pos = analyze.find_first_not_of(" ",end_pos);
  prog_name = analyze.substr(beg_pos); 						// Substract the Prog-if Name from line.

  // Create a Pci IDs Prog-if object providing a pointer to it's Subclass object.
  PciIdsProgif* new_progif = new PciIdsProgif(parent_subclass, prog_code, prog_name);
}



//! Store PCI Class Codes.
/*! 
  Iterate through the list of #class_list and
  call PciIdsClass::store_db() function of each instance
  which will save the class and all it's Pci IDs Class Codes(Subclasses-Progifs)
  to database.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int store_classes(void)
{
  list<PciIdsClass>::iterator iter;

  for( iter = class_list.begin(); iter != class_list.end(); ++iter ){
  	if( iter->store_db() != 0 )	// Store Class together with it's Subclasses and Prog-ifs of the Subclasses.
	  return 1;
  }

  return 0;
}



//! Delete objects holding Pci IDs Class Codes.
/*!
  Loop through the list of #class_list and delete every instance.
  
  On deletion of a PciIdsClass object, it deletes it's PciIdsSubclass objects.
  And on deletion of a PciIdsSubclass object, it deletes it's PciIdsProgif objects.
  So nothing left behind!  
 */
void delete_classes(void)
{
  int class_list_size = class_list.size();
  
  for( int j=0; j<class_list_size; j++ ){
	class_list.pop_front();		// STL pop calls the destructor of the current PciIdsClass object
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
  if ( update_device_codes( pci_ids_file ) != 0 ) 	// Manage update of PCI Device codes.
	return 1;

  if ( update_class_codes( pci_ids_file ) != 0 ) 	// Manage update of PCI Class codes.
	return 1;

  return 0;
}
