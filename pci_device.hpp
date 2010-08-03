/* pci_device.hpp --- 
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
  \file   pci_device.hpp
  \date   Wed Jul 28 17:48:11 2010
  
  \brief  PciDevice Class Implemetation
  
*/
#if !defined( _PCI_DEVICE_ )
#define _PCI_DEVICE_


//! Represents a PCI Device.
/*!
  This is a 2 purpose class depending on usage:
  - To represent a real-world PCI Device as used in PcimapQueryWidget.
  - To represent an entry in 'modules.pcimap' as used in os_pci_module.cc.
*/
class PciDevice
{
  
private:
  std::string vendor_,			/*!< Vendor Code of the device. */
	device_,					/*!< Device Code. */
	subvendor_,					/*!< Subvendor Code. */
	subdevice_,					/*!< Subdevice Code. */
	classc_,					/*!< Class Code of the device. */
	subclass_,					/*!< Subclass Code.  */
	progif_,					/*!< Programming Interface Code. */
	classMask_,					/*!< Class Mask if used to represent an entry in 'modules.pcimap'.  */
	module_;					/*!< Module name if used to represent an entry in 'modules.pcimap'. */

  
  std::string vendorName_,		/*!< Vendor Name of the device. \warning NOT used! */
	deviceName_,				/*!< Device Name. \warning NOT used! */
	subsysName_,				/*!< Subsystem Name. \warning NOT used! */
	className_,					/*!< Class Name. \warning NOT used! */
	subclassName_,				/*!< Subclass Name. \warning NOT used! */
	progifName_;				/*!< Programming Interface Name. \warning NOT used! */

  
public:

  //! Constructor.
  /*! 
	Initialized with no info.
   */
  PciDevice()
  {
  }


  //! Set Module name.
  /*!
	This is used when representing an entry in 'modules.pcimap'.
	
	\param module_name Name of a Module.
  */
  void setModule ( const std::string module_name )
  {
	module_ = module_name;
  }

  //! Get Module name.
  /*! 
	This is used when representing an entry in 'modules.pcimap'.
	
	\return Name of the Module supporting the devices.
  */
  std::string getModule () const
  {
	return module_;
  }


  //! Set Vendor Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Vendor Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported vendor code, 'ffff' means match any.
	</ul>
  */
  void setVendor ( const std::string vendor_code )
  {
	vendor_ = vendor_code;
  }

  //! Get Vendor Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Vendor Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported vendor code, 'ffff' means match any.
	</ul>
  */
  std::string getVendor () const
  {
	return vendor_;
  }



  //! Set Device Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Device Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported device code, 'ffff' means match any.
	</ul>
  */
  void setDevice ( const std::string device_code )
  {
	device_ = device_code;
  }


  //! Get Device Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Device Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported device code, 'ffff' means match any.
	</ul>
  */
  std::string getDevice () const
  {
	return device_;
  }



  //! Set Subvendor Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Subvendor Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported subvendor code, 'ffff' means match any.
	</ul>
  */
  void setSubvendor ( const std::string subvendor_code )
  {
	subvendor_ = subvendor_code;
  }


  //! Get Subvendor Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Subvendor Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported subvendor code, 'ffff' means match any.
	</ul>
  */
  std::string getSubvendor () const
  {
	return subvendor_;
  }



  //! Set Subdevice Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Subdevice Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported subdevice code, 'ffff' means match any.
	</ul>
  */
  void setSubdevice ( const std::string subdevice_code )
  {
	subdevice_ = subdevice_code;
  }


  //! Get Subdevice Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Subdevice Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported subdevice code, 'ffff' means match any.
	</ul>
  */
  std::string getSubdevice () const
  {
	return subdevice_;
  }



  //! Set Class Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Class Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported class code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  void setClass ( const std::string class_code )
  {
	classc_ = class_code;
  }


  //! Get Class Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Class Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported class code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  std::string getClass () const
  {
	return classc_;
  }



  //! Set Subclass Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Subclass Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported subclass code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  void setSubclass ( const std::string subclass_code )
  {
	subclass_ = subclass_code;
  }


  //! Get Subclass Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Subclass Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported subclass code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  std::string getSubclass () const
  {
	return subclass_;
  }



  //! Set Prog-if Code.
  /*!
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Set Prog-if Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Set the supported prog-if code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  void setProgif ( const std::string progif_code )
  {
	progif_ = progif_code;
  }


  //! Get Prog-if Code.
  /*! 
	Depending on usage:
	<ul>
	<li>As PCI Device\n
	Get Prog-if Code of the device.
	<li>As an 'modules.pcimap' entry\n
	Get the supported prog-if code. Doesn't mean anything by itself, used together as class-subclass-progif.
	</ul>
  */
  std::string getProgif () const
  {
	return progif_;
  }



  //! Set Class Mask.
  /*!
	This is used when representing an entry in 'modules.pcimap'.
	Tells which parts of class-subclass-progif code should be matched to find supported devices.
	
	\param class_mask Column class_mask from 'modules.pcimap'.
  */
  void setClassMask ( const std::string class_mask )
  {
	classMask_ = class_mask;
  }


  //! Get Class Mask.
  /*! 
	This is used when representing an entry in 'modules.pcimap'.
	Tells which parts of class-subclass-progif code should be matched to find supported devices.
  */
  std::string getClassMask () const
  {
	return classMask_;
  }

};

#endif // _PCI_DEVICE_
