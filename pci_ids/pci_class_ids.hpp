// Hey Emacs -*- C++ -*-
/** pci_classes.h --- 
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


#if !defined ( _PCI_CLASS_IDS_ )
#define _PCI_CLASS_IDS_

#include <vector>
#include <string>

//#define DEBUG

using namespace std;

class PciIdsClass;
class PciIdsSubclass;
class PciIdsProgif;


class PciIdsClass
{
private:
  string code;
  string name;
  vector<PciIdsSubclass*> kid_list; // hold subclasses of the class
  
public:
  PciIdsClass(string cCode, string cName) : code(cCode), name(cName)
  {
  }

  ~PciIdsClass();
  
  void add_kid(PciIdsSubclass* kid)
  {
  	kid_list.push_back(kid);
  }

  string get_code (void) const
  {
	return code;
  }

  string get_name (void) const
  {
	return name;
  }

  int store_db (void);

};

class PciIdsSubclass
{
private:
  PciIdsClass* parent_class;
  string code, name;
  vector<PciIdsProgif*> progif_list; // hold progIfs of the subclass

public:
  PciIdsSubclass(PciIdsClass* sParent, string sCode, string sName) : parent_class(sParent), code(sCode), name(sName)
  {
	sParent->add_kid(this);
  }

  ~PciIdsSubclass();

  void add_progif(PciIdsProgif* progif)
  {
	progif_list.push_back(progif);
  }

  string get_code (void) const
  {
	return code;
  }

  string get_name (void) const
  {
	return name;
  }

  const PciIdsClass* const get_parent (void) const
  {
	return parent_class;
  }

  friend int PciIdsClass::store_db(void);
    
};

class PciIdsProgif
{
private:
  PciIdsSubclass* parent_subclass;
  string code, name;

public:
  PciIdsProgif(PciIdsSubclass* pParent, string pCode, string pName) : parent_subclass(pParent), code(pCode), name(pName)
  {
	pParent->add_progif(this);
  }

  string get_code (void) const
  {
	return code;
  }

  string get_name (void) const
  {
	return name;
  }

  const PciIdsSubclass* const get_parent (void) const
  {
	return parent_subclass;
  }
  
};

#endif	// _PCI_CLASS_IDS_
