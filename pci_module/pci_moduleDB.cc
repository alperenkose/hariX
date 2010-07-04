#include <vector>
#include <string>

#include "../pci_device.hpp"
#include "../os_info.hpp"

#include "../harixApp.hpp"		// just for wApp
#include <Wt/WLogger>

using std::vector;

std::string queryOsKernelId(std::string os_name,
							std::string release,
							std::string kernel,
							std::string architecture); 	// extern database.cc

std::string recordOsInfo (OsInfo); 						// extern database.cc

std::string checkModuleNameId(std::string module_name); // extern database.cc

std::string checkKernelModule (std::string uKernelId, std::string modNameId); // extern database.cc

void insertPcimap ( const PciDevice* const currentPciDevice, std::string uniqueModuleId ); // extern database.cc


char store_pci_map( vector<PciDevice>& pci_map_list, OsInfo& osInfo )
{
  std::string uniqueKernelId_;
  std::string mod_name_id;

	uniqueKernelId_=queryOsKernelId(osInfo.getDistro(),osInfo.getRelease(),osInfo.getKernel(),osInfo.getArch());

	if( uniqueKernelId_ == "" )
	  uniqueKernelId_ = recordOsInfo(osInfo);

	vector<PciDevice>::iterator vec_iter;
	for ( vec_iter = pci_map_list.begin(); vec_iter != pci_map_list.end(); ++vec_iter ){

	  mod_name_id = checkModuleNameId( vec_iter->getModule() );

	  // check uKernelID, modNameID pair in modules..
	  std::string uniqueModuleId_;
	  uniqueModuleId_ = checkKernelModule( uniqueKernelId_, mod_name_id );

	  // insert modules.pcimap entry into pcimap
	  insertPcimap( &*vec_iter , uniqueModuleId_ ); // C++ Coding Standarts, Rule #78, by Herb Sutter
	  wApp->log("debug") << "PCIMAP ENTRY PROCESSED!!!"; 					// @test
	
	}

}

