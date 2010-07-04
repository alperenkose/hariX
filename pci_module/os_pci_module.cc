// #define NDEBUG					// To get rid of all the 'assert's
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "../pci_device.hpp"
#include "../os_info.hpp"

using std::string;
using std::vector;

char store_pci_map( vector<PciDevice>&, OsInfo& );
void destroy_device_list (void);

vector<PciDevice> device_list;

int os_pci_module( OsInfo osInfo, string pcimapFile )
{
  int success;
  std::ifstream pcimap( pcimapFile.c_str() );
  assert(pcimap);

  string driver_data;
  char coloum_count = 0;
  string mod_line, get_s;
  
  while ( getline(pcimap,mod_line) ){
	if ( mod_line[0] == '#' )
	  continue;
	std::istringstream s_in(mod_line);
	PciDevice *pci_dev = new PciDevice();
	coloum_count = 0;
	while ( s_in >> get_s ) {
	  switch(coloum_count) {
	  case 0:
		pci_dev->setModule(get_s);
		break;
	  case 1:
		get_s = get_s.substr(6);
		pci_dev->setVendor(get_s);
		break;
	  case 2:
		get_s = get_s.substr(6);
		pci_dev->setDevice(get_s);
		break;
	  case 3:
		get_s = get_s.substr(6);
		pci_dev->setSubvendor(get_s);
		break;
	  case 4:
		get_s = get_s.substr(6);
		pci_dev->setSubdevice(get_s);
		break;
	  case 5:
		pci_dev->setClass(get_s.substr(4,2)); // Class - Subclass - Progif
		pci_dev->setSubclass(get_s.substr(6,2));
		pci_dev->setProgif(get_s.substr(8));
		break;
	  case 6:
		get_s = get_s.substr(4);
		pci_dev->setClassMask(get_s);
		break;
	  case 7:
		driver_data = get_s;
		break;
	  }

	  ++coloum_count;
	}
	device_list.push_back(*pci_dev);
	delete pci_dev;	
  }


  success = store_pci_map(device_list, osInfo);
  
  destroy_device_list();

  return success;
}

void destroy_device_list(void)
{
  int device_list_size = device_list.size();
  for ( int k=0; k<device_list_size; ++k )
	device_list.pop_back();
}
