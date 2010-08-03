/* pci_class_ids.hpp --- 
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
  \file   pci_class_ids.hpp
  \date   Fri Jul 30 11:37:44 2010
  
  \brief  PCI IDs Class Codes Classes
  
  Contains the implementation of PciIdsClass, PciIdsSubclass, PciIdsProgif Classes.
*/
#if !defined ( _PCI_CLASS_IDS_ )
#define _PCI_CLASS_IDS_

#include <vector>
#include <string>

//#define DEBUG					// To debug the communication with MySQL

class PciIdsClass;
class PciIdsSubclass;
class PciIdsProgif;



//! Corresponds to a Class entry in 'pci.ids'.
/*!
  Contains the Class code and name.
  Also holds list of Subclasses belonging to it in #kid_list.
  And manages storing of all 'Class-Subclass-Progif's to database
  with it's store_db() function.

  \sa pci_ids.cc
*/
class PciIdsClass
{
  
private:
  //! Hold Class Code.
  std::string code;

  //! Hold Class Name.
  std::string name;

  //! Hold pointers to Subclasses of the Class.
  std::vector<PciIdsSubclass*> kid_list;
  
public:

  //! Constructor.
  /*!
	Create a Pci IDs Class instance with provided code and name.	
	
	\param cCode Class Code.
	\param cName Class Name.
  */
  PciIdsClass(std::string cCode, std::string cName) : code(cCode), name(cName)
  {
  }


  //! Destructor.
  /*! 
	Loop through the Subclasses(#kid_list) of current Class and delete them.	
   */
  ~PciIdsClass();


  //! Add a Subclass of the Class.
  /*!
	Add Subclass to the list of #kid_list.
	
	\param kid Subclass of the Class.
  */
  void add_kid(PciIdsSubclass* kid)
  {
  	kid_list.push_back(kid);
  }


  //! Get Class Code.
  /*! 
	Get class code of current instance.
	
	\return The class code.
  */
  std::string get_code (void) const
  {
	return code;
  }


  //! Get Class Name.
  /*! 
	Get class name of the current instance.
	
	\return The class name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Store the Class, it's Subclasses and Prog-ifs to database.
  /*! 
	Iterate through the Class' Subclasses and each Subclass' Prog-ifs
	recursively, recording or updating the database for each entry.
	
	\return Success status
	- 0 -> (SUCCESS)
	- 1 -> (FAIL)
  */
  int store_db (void);

};





//! Corresponds to a Subclass entry in 'pci.ids'.
/*!
  Contains the Subclass code and name.
  Also holds list of Prog-ifs belonging to it in #progif_list.

  \sa pci_ids.cc
*/
class PciIdsSubclass
{
  
private:
  //! Hold pointer to Class of the Subclass.
  PciIdsClass* parent_class;

  //! Hold Subclass Code.
  std::string code;

  //! Hold Subclass Name.
  std::string name;

  //! Hold pointers to Prog-ifs of the Subclass.
  std::vector<PciIdsProgif*> progif_list;

public:

  //! Constructor.
  /*!
	Create a Pci IDs Subclass instance with provided code and name.
	And add itself to the list of Subclasses of provided Class.
	
	\param sParent Pointer to Class of the Subclass.
	\param sCode Subclass Code.
	\param sName Subclass Name.
  */
  PciIdsSubclass(PciIdsClass* sParent, std::string sCode, std::string sName)
	: parent_class(sParent), code(sCode), name(sName)
  {
	sParent->add_kid(this);		// Add current instance to it's Class' Subclass list.
  }


  //! Destructor.
  /*! 
	Loop through the Prog-ifs(#progif_list) of current Subclass and delete them.
  */
  ~PciIdsSubclass();


  //! Add a Prog-if of the Subclass.
  /*!
	Add Prog-if to the list of #progif_list.
	
	\param progif Prog-if of the Subclass.
  */
  void add_progif(PciIdsProgif* progif)
  {
	progif_list.push_back(progif);
  }


  //! Get Subclass Code.
  /*! 
	Get subclass code of current instance.	
	
	\return The subclass code.
  */
  std::string get_code (void) const
  {
	return code;
  }


  //! Get Subclass Name.
  /*! 
	Get subclass name of current instance.	
	
	\return The subclass name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Get pointer to it's Class.
  /*! 
	Get pointer to Class the Subclass belongs to. 
	
	\return Pointer to Class.
  */
  const PciIdsClass* const get_parent (void) const
  {
	return parent_class;
  }

  friend int PciIdsClass::store_db(void);
    
};





//! Corresponds to a Programming Interface entry in 'pci.ids'.
/*!
  Contains the Prog-if Code and Name.

  \sa pci_ids.cc
*/
class PciIdsProgif
{
  
private:
  //! Hold pointer to Subclass of the Prog-if.
  PciIdsSubclass* parent_subclass;

  //! Hold Prog-if Code.
  std::string code;

  //! Hold Prog-if Name.
  std::string name;

public:

  //! Constructor.
  /*!
	Create a Pci IDs Prog-if instance with provided code and name.
	And add itself to the list of Prog-ifs of provided Subclass.	
	
	\param pParent Pointer to Subclass of the Prog-if.
	\param pCode Prog-if Code.
	\param pName Prog-if Name.
  */
  PciIdsProgif(PciIdsSubclass* pParent, std::string pCode, std::string pName)
	: parent_subclass(pParent), code(pCode), name(pName)
  {
	pParent->add_progif(this);	// Add current instance to it's Subclass' Prog-if list.
  }


  //! Get Prog-if Code.
  /*! 
	Get Programming interface code of current instance.
	
	\return The prog-if code.
  */
  std::string get_code (void) const
  {
	return code;
  }
  

  //! Get Prog-if Name.
  /*! 
	Get Programming interface name of current instance.	
	
	\return The prog-if name.
  */
  std::string get_name (void) const
  {
	return name;
  }


  //! Get pointer to it's Subclass.
  /*! 
	Get pointer to Subclass the Prog-if belongs to.
	
	\return Pointer to Subclass.
  */
  const PciIdsSubclass* const get_parent (void) const
  {
	return parent_subclass;
  }
  
};

#endif	// _PCI_CLASS_IDS_
