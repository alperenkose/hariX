// Hey Emacs -*- C++ -*-
/* pci_device_ids.hpp --- 
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
  \file   pci_device_ids.hpp
  \date   Thu Jul 29 17:19:54 2010
  
  \brief  PCI IDs Device Codes Classes

  Contains the implementation of PciIdsVendor, PciIdsDevice and PciIdsSubsys Classes.
*/
#if !defined( _PCI_DEVICE_IDS_ )
#define _PCI_DEVICE_IDS_

#include <vector>
#include <string>

// #define DEBUG					// To debug the communication with MySQL

class PciIdsDevice;
class PciIdsSubsys;



//! Corresponds to a Vendor entry in 'pci.ids'.
/*!
  Contains the Vendor code and name.
  Also holds list of Devices belonging to it in #kid_list.
  And manages storing of all 'Vendor-Device-Subsystem's to database
  with it's store_db() function.

  \sa pci_ids.cc
*/
class PciIdsVendor
{
  
private:
  //! Hold Vendor Code.
  std::string code;
  
  //! Hold Vendor Name.
  std::string name;

  //! Hold pointers to Devices of the Vendor.
  std::vector<PciIdsDevice*> kid_list;
  
public:
  
  //! Constructor.
  /*!
	Create a Pci IDs Vendor instance with provided code and name.
	
	\param vCode Vendor Code.
	\param vName Vendor Name.
  */
  PciIdsVendor(std::string vCode, std::string vName) : code(vCode), name(vName)
  {
  }


  //! Destructor.
  /*!
	Loop through the Devices(#kid_list) of current Vendor and delete them.
   */
  ~PciIdsVendor();


  //! Add a Device of the Vendor.
  /*!
	Add Device to the list of #kid_list.
	
	\param kid Device of the Vendor.
  */
  void add_kid(PciIdsDevice* kid)
  {
  	kid_list.push_back(kid);
  }


  //! Get Vendor Code.
  /*! 
	Get vendor code of current instance.
	
	\return The vendor code.
  */
  std::string get_code (void) const
  {
	return code;
  }


  //! Get Vendor Name.
  /*! 
	Get vendor name of current instance.
	
	\return The vendor name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Store the Vendor, it's Devices and Subsystems to database.
  /*! 
	Iterate through the Vendor's Devices and each Device's Subsystems
	recursively, recording or updating the database for each entry.
	
	\return Success status
	- 0 -> (SUCCESS)
	- 1 -> (FAIL)
  */
  int store_db();

};





//! Corresponds to a Device entry in 'pci.ids'.
/*!
  Contains the Device code and name.
  Also holds list of Subsystems belonging to it in #subsystem_list.

  \sa pci_ids.cc
*/
class PciIdsDevice
{
  
private:
  //! Hold pointer to Vendor of the Device.
  PciIdsVendor* parent_vendor;

  //! Hold Device Code.
  std::string code;

  //! Hold Device Name.
  std::string name;

  //! Hold pointers to Subsystems of the Device.
  std::vector<PciIdsSubsys*> subsystem_list;

public:

  //! Constructor.
  /*!
	Create a Pci IDs Device instance with provided code and name.
	And add itself to the list of Devices of provided Vendor.
	
	\param dParent Pointer to Vendor of the Device.
	\param dCode Device Code.
	\param dName Device Name.
  */
  PciIdsDevice(PciIdsVendor* dParent, std::string dCode, std::string dName)
	: parent_vendor(dParent), code(dCode), name(dName)
  {
	dParent->add_kid(this);		// Add current instance to it's Vendor's Device list.
  }


  //! Destructor.
  /*! 
	Loop through the Subsystems(#subsystem_list) of current Device and delete them.	
  */
  ~PciIdsDevice();


  //! Add a Subsystem of the Device.
  /*!
	Add Subsystem to the list of #subsystem_list.	
	
	\param subsystem Subsystem of the Device.
  */
  void add_subsystem(PciIdsSubsys* subsystem)
  {
  	subsystem_list.push_back(subsystem);
  }


  //! Get Device Code.
  /*! 
	Get device code of current instance.
	
	\return The device code.
  */
  std::string get_code (void) const
  {
	return code;
  }


  //! Get Device Name.
  /*! 
	Get device name of current instance.
	
	\return The device name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Get pointer to it's Vendor.
  /*!
	Get pointer to Vendor the Device belongs to.
	
	\return Pointer to Vendor.
  */
  const PciIdsVendor* const get_parent (void) const
  {
	return parent_vendor;
  }

  friend int PciIdsVendor::store_db();
    
};




//! Corresponds to a Subsystem entry in 'pci.ids'.
/*!
  Contains the Subvendor code, Subdevice code and Subsystem name.

  \sa pci_ids.cc
*/
class PciIdsSubsys
{
  
private:
  //! Hold pointer to Device of the Subsystem.
  PciIdsDevice* parent_device;
  
  std::string subvendor,		/*!< Hold Subvendor Code. */
	subdevice,					/*!< Hold Subdevice Code. */
	name;						/*!< Hold Subsystem Name. */

public:

  //! Constructor.
  /*!
	Create a Pci IDs Subsystem instance with provided subvendor, subdevices codes
	and subsystem name.
	And add itself to the list of Subsystems of provided Device.
	
	\param sParent Pointer to Device of the Subsystem.
	\param sVendor Subvendor Code.
	\param sDevice Subdevice Code.
	\param sName Subsystem Name.
  */
  PciIdsSubsys(PciIdsDevice* sParent, std::string sVendor, std::string sDevice, std::string sName)
	: parent_device(sParent), subvendor(sVendor), subdevice(sDevice), name(sName)
  {
	sParent->add_subsystem(this); 	// Add current instance to it's Device's Subsystem list.
  }


  //! Get Subvendor Code.
  /*! 
	Get Subvendor code of current instance.	
	
	\return The subvendor code.
  */
  std::string get_subVendor (void) const
  {
	return subvendor;
  }


  //! Get Subdevice Code.
  /*! 
	Get Subdevice code of current instance.	
	
	\return The subdevice code.
  */
  std::string get_subDevice (void) const
  {
	return subdevice;
  }


  //! Get Subsystem Name.
  /*! 
	Get subsystem name of current instance.	
	
	\return The subsystem name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Get pointer to it's Device.
  /*!
	Get pointer to Device the Subsystem belongs to.
	
	\return Pointer to Device.
  */
  const PciIdsDevice* const get_parent (void) const
  {
	return parent_device;
  }
  
};


#endif	// _PCI_DEVICE_IDS_
