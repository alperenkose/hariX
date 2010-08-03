/* harixApp.hpp --- 
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
  \file   harixApp.hpp
  \date   Thu Jul 15 13:11:46 2010
  
  \brief  HarixApp Class Interface
  
*/

#if !defined( _HARIXAPP_ )
#define _HARIXAPP_

#include <Wt/WApplication>

class QueryResult;

//! The WApplication instance created for every session.
/*!
  Manages all the pages of the application and surfing feeling.

  Inherits Wt::WApplication from Wt library.
 */
class HarixApp : public Wt::WApplication
{
private:
  //! Holds all the widgets to be shown(just like pages).
  Wt::WStackedWidget* stackedContent_;

public:

  //! Constructor
  /*!
	Called from the createApplication(const Wt::WEnvironment& env) function in main.cc file.
  */
  HarixApp( const Wt::WEnvironment& env );

  
  //! Called at URL changes
  /*! 
	Binded to WApplication::internalPathChanged which is triggered on URL changes.

	Manages which widget(page) to be shown depending on the path.
	\param path The URL to widget like /home, /analyze_os, /mainboards..
   */
  void internalPathChanged(const std::string& path);

  
  //! Selects the current widget(page) to be shown.
  /*!
	Adds the passed widget to the HarixApp::stackedContent_.

	Called from global function selectWidget(WContainerWidget* widget).
   */
  void setWidget( Wt::WContainerWidget* widget);


  //! Removes the widget(page)
  /*!
	Removes the passed widget from HarixApp::stackedContent_.

	Called from global function removeWidget(WContainerWidget* widget).
   */
  void removeWidget( Wt::WContainerWidget* widget );

};

#endif // _HARIXAPP_
