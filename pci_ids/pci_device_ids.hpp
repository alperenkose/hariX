// Hey Emacs -*- C++ -*-
/** pci_device_ids.hpp --- 
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

#if !defined( _PCI_DEVICE_IDS_ )
#define _PCI_DEVICE_IDS_

#include <vector>
#include <string>

//#define DEBUG

class PciIdsDevice;
class PciIdsSubsys;

class PciIdsVendor
{
private:
  std::string code;
  std::string name;
  std::vector<PciIdsDevice*> kid_list; // hold devices of the vendor
  
public:
  PciIdsVendor(std::string vCode, std::string vName) : code(vCode), name(vName)
  {
  }

  ~PciIdsVendor();
  
  void add_kid(PciIdsDevice* kid)
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

class PciIdsDevice
{
private:
  PciIdsVendor* parent_vendor;
  std::string code, name;
  std::vector<PciIdsSubsys*> subsystem_list; // hold subsystems of the device

public:
  PciIdsDevice(PciIdsVendor* dParent, std::string dCode, std::string dName) : parent_vendor(dParent), code(dCode), name(dName)
  {
	dParent->add_kid(this);
  }

  ~PciIdsDevice();

  void add_subsystem(PciIdsSubsys* subsystem)
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

  const PciIdsVendor* const get_parent (void) const
  {
	return parent_vendor;
  }

  friend int PciIdsVendor::store_db(void);
    
};

class PciIdsSubsys
{
private:
  PciIdsDevice* parent_device;
  std::string subvendor, subdevice, name;

public:
  PciIdsSubsys(PciIdsDevice* sParent, std::string sVendor, std::string sDevice, std::string sName) : parent_device(sParent), subvendor(sVendor), subdevice(sDevice), name(sName)
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

  const PciIdsDevice* const get_parent (void) const
  {
	return parent_device;
  }
  
};


#endif	// _PCI_DEVICE_IDS_
