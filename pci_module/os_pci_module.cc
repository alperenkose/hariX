/* os_pci_module.cc --- 
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
/*!
  \file   os_pci_module.cc
  \date   Wed Jul 21 16:08:28 2010
  
  \brief  Organize OS-Modules Save

  Read 'modules.pcimap' and Organize storing of OS-pcimap_list into database.
*/

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


/*
  Global function from pci_moduleDB.cc to store 'modules.pcimap'
  relating with the given operating system.
*/
//! \relates pci_moduleDB.cc
char store_pci_map( vector<PciDevice>&, OsInfo& );

void destroy_device_list (void);


//! Hold list of devices in 'modules.pcimap' file.
vector<PciDevice> device_list;



//! Organize saving of OS-pcimap_list in database.
/*!
  Read the 'modules.pcimap' list into #device_list variable.
  And call the function store_pci_map() for storing OS-pcimap_list.
  
  \param osInfo Object holding the OS information.
  \param pcimapFile Path to file representing 'modules.pcimap' in Server.
  
  \return Success status
  - 0 -> (SUCCESS)
  - 1 -> (FAIL)
*/
int os_pci_module( OsInfo osInfo, string pcimapFile )
{
  int success;

  // Open file 'modules.pcimap' uploaded to server.
  std::ifstream pcimap( pcimapFile.c_str() );
  assert(pcimap);				// Oops we failed to open it. Fatal.

  string driver_data;
  char coloum_count = 0;		// Holding the column number we are currently reading in 'modules.pcimap' file.
  string mod_line, get_s;
  
  while ( getline(pcimap,mod_line) ){
	if ( mod_line[0] == '#' )
	  continue;					// If it is a comment, skip it..
	
	std::istringstream s_in(mod_line); 		// Put the line read into a stream.
	
	PciDevice *pci_dev = new PciDevice(); 	// Create an object to hold the values of current device read.
	
	coloum_count = 0;
	while ( s_in >> get_s ) {				// Read consequent columns in 'modules.pcimap'.
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
		pci_dev->setClass(get_s.substr(4,2)); // Separately store Class,Subclass and Progif from 'class' column.
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
	device_list.push_back(*pci_dev); 		// Put the current device read into device_list vector.
	delete pci_dev;	
  }


  success = store_pci_map(device_list, osInfo); // Store/Update OS-pcimap_list.
  
  destroy_device_list();

  return success;
}


//! Delete devices from memory.
/*! 
  Delete the list of devices temporarily stored in #device_list variable.
 */
void destroy_device_list(void)
{
  int device_list_size = device_list.size();
  for ( int k=0; k<device_list_size; ++k )
	device_list.pop_back();
}
