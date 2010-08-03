/*!
  \file   pci_moduleDB.cc
  \date   Wed Jul 21 17:28:52 2010
  
  \brief  Database relation of os_pci_module.cc
  
  Store Operating System information in database, and add it's 'modules.pcimap'
  list. If the given OS exists just update it's 'modules.pcimap' list.
*/
#define NDEBUG					// set when NON debug edition
#include <vector>
#include <string>

#include <Wt/WLogger>
#include "../harixApp.hpp"		// just for wApp

#include "../pci_device.hpp"
#include "../os_info.hpp"

using std::vector;

/*
  Global function from database.cc to query uKernelID of
  the given operating system.
*/
//! \relates database.cc
std::string queryOsKernelId(std::string os_name,
							std::string release,
							std::string kernel,
							std::string architecture);


/*
  Global function from database.cc to record the given OS in database.
*/
//! \relates database.cc
std::string recordOsInfo (OsInfo);


/*
  Global function from database.cc to check if Module with given name exists.
  Return it's ID.
*/
//! \relates database.cc
std::string checkModuleNameId(std::string module_name);


/*
  Global function from database.cc to check
  if the Kernel has the provided Module in database.
*/
//! \relates database.cc
std::string checkKernelModule (std::string uKernelId, std::string modNameId);


/*
  Global function from database.cc to update pcimap-list entry
  of a given Module of OS.
*/
//! \relates database.cc
int insertPcimap ( const PciDevice* const currentPciDevice, std::string uniqueModuleId ); // extern database.cc


//! Store/Update OS-pcimap_list in database.
/*!
  Store OS along with it's 'modules.pcimap' list in database,
  or just update the 'modules.pcimap' list if the given OS exists.
  
  \param pci_map_list List of devices in 'modules.pcimap' as std::vector.
  \param osInfo Object holding the OS information.
  
  \return Success status of store/update.
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int store_pci_map( vector<PciDevice>& pci_map_list, OsInfo& osInfo )
{
  std::string uniqueKernelId_;	// Unique ID representing an OS.
  std::string mod_name_id;		// ID representing a Module name.

  // Get the unique ID for the given OS.
  uniqueKernelId_=queryOsKernelId(osInfo.getDistro(),osInfo.getRelease(),osInfo.getKernel(),osInfo.getArch());

  
  if( uniqueKernelId_ == "" )					// If the OS doesn't exist in database.
	uniqueKernelId_ = recordOsInfo(osInfo); 	// Store the OS in database, and get it's unique ID.

  
  // Iterate through the pcimap list.
  vector<PciDevice>::iterator vec_iter;
  for ( vec_iter = pci_map_list.begin(); vec_iter != pci_map_list.end(); ++vec_iter ){

	mod_name_id = checkModuleNameId( vec_iter->getModule() ); // Get ID of given Module Name.

	// check if the given OS has the Module in database, if not add it.
	std::string uniqueModuleId_;
	uniqueModuleId_ = checkKernelModule( uniqueKernelId_, mod_name_id );

	// Populate pcimap-list in database for the spesific Module of an OS.
	if( insertPcimap( &*vec_iter , uniqueModuleId_ ) != 0 ) // C++ Coding Standarts, Rule #78, by Herb Sutter
	  return 1;

#ifndef NDEBUG
	wApp->log("debug") << "PCIMAP ENTRY PROCESSED!!!";
#endif  // NDEBUG
	
  }
  
  return 0;
}

