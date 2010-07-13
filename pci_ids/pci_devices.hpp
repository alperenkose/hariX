// Hey Emacs -*- C++ -*-
/** pci_devices.h --- 
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

#if !defined( _PCI_DEVICES_ )
#define _PCI_DEVICES_

#include <vector>
#include <string>

//#define DEBUG

class pciDevice;
class pciSubsystem;

class pciVendor
{
private:
  std::string code;
  std::string name;
  std::vector<pciDevice*> kid_list; // hold devices of the vendor
  
public:
  pciVendor(std::string vCode, std::string vName) : code(vCode), name(vName)
  {
  }

  ~pciVendor();
  
  void add_kid(pciDevice* kid)
  {
  	kid_list.push_back(kid);
  }

  std::string get_code (void) const
  {
	return code;
  }

  std::string get_name (void) const
  {
	return name;
  }

  int store_db (void);

};

class pciDevice
{
private:
  pciVendor* parent_vendor;
  std::string code, name;
  std::vector<pciSubsystem*> subsystem_list; // hold subsystems of the device

public:
  pciDevice(pciVendor* dParent, std::string dCode, std::string dName) : parent_vendor(dParent), code(dCode), name(dName)
  {
	dParent->add_kid(this);
  }

  ~pciDevice();

  void add_subsystem(pciSubsystem* subsystem)
  {
  	subsystem_list.push_back(subsystem);
  }

  std::string get_code (void) const
  {
	return code;
  }

  std::string get_name (void) const
  {
	return name;
  }

  const pciVendor* const get_parent (void) const
  {
	return parent_vendor;
  }

  friend int pciVendor::store_db(void);
    
};

class pciSubsystem
{
private:
  pciDevice* parent_device;
  std::string subvendor, subdevice, name;

public:
  pciSubsystem(pciDevice* sParent, std::string sVendor, std::string sDevice, std::string sName) : parent_device(sParent), subvendor(sVendor), subdevice(sDevice), name(sName)
  {
	sParent->add_subsystem(this);
  }

  std::string get_subVendor (void) const
  {
	return subvendor;
  }

  std::string get_subDevice (void) const
  {
	return subdevice;
  }

  std::string get_name (void) const
  {
	return name;
  }

  const pciDevice* const get_parent (void) const
  {
	return parent_device;
  }
  
};


#endif	// _PCI_DEVICES_H
