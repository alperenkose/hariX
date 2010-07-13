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


#if !defined ( _PCI_CLASSES_H )
#define _PCI_CLASSES_H

#include <vector>
#include <string>

//#define DEBUG

using namespace std;

class pciClass;
class pciSubclass;
class pciProgif;


class pciClass
{
private:
  string code;
  string name;
  vector<pciSubclass*> kid_list; // hold subclasses of the class
  
public:
  pciClass(string cCode, string cName) : code(cCode), name(cName)
  {
  }

  ~pciClass();
  
  void add_kid(pciSubclass* kid)
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

class pciSubclass
{
private:
  pciClass* parent_class;
  string code, name;
  vector<pciProgif*> progif_list; // hold progIfs of the subclass

public:
  pciSubclass(pciClass* sParent, string sCode, string sName) : parent_class(sParent), code(sCode), name(sName)
  {
	sParent->add_kid(this);
  }

  ~pciSubclass();

  void add_progif(pciProgif* progif)
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

  const pciClass* const get_parent (void) const
  {
	return parent_class;
  }

  friend int pciClass::store_db(void);
    
};

class pciProgif
{
private:
  pciSubclass* parent_subclass;
  string code, name;

public:
  pciProgif(pciSubclass* pParent, string pCode, string pName) : parent_subclass(pParent), code(pCode), name(pName)
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

  const pciSubclass* const get_parent (void) const
  {
	return parent_subclass;
  }
  
};

#endif	// _PCI_CLASSES_H
