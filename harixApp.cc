/* harixApp.cc --- 
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
  \file   harixApp.cc
  \date   Thu Jul 15 13:16:19 2010
  
  \brief  Implementation of HarixApp Class
  
  This is where all the browsing is managed.
  Widgets(as pages) are added/removed and shown/hide as needed.
*/
#include <Wt/WStackedWidget>
#include <Wt/WLogger>

#include "harixApp.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "lspci_query/pcimapResult.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "mainboards/mainboards.hpp"
#include "home.hpp"

using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");
  setCssTheme("polished"); // use the polished theme folder from /resources/themes/
  useStyleSheet("resources/stylizr.css");

  // Common stack to hold any pages..
  stackedContent_ = new WStackedWidget(root());

  // Create an HomeWidget(representing the home page) instance
  HomeWidget* home_page = HomeWidget::Instance(stackedContent_);
  WApplication::instance()->setInternalPath("/home",true); // set URL to /application_path.wt#/home

  /*
	WApplication::internalPathChanged signal connected to the HarixApp::internalPathChanged.
	So any URL change will trigger HarixApp::internalPathChanged function!
  */
  wApp->internalPathChanged().connect(SLOT(this, HarixApp::internalPathChanged));
}

void HarixApp::setWidget( WContainerWidget* widget )
{
  // sets the current visual widget of stackedContent_ while hiding the other widgets
  stackedContent_->setCurrentWidget( widget );
}

void HarixApp::removeWidget( WContainerWidget* widget )
{
  // Deletes the passed widget from stackedContent_
  stackedContent_->removeWidget( widget );
}


//! Global function to select current widget(page).
/*!
  Calls the HarixApp::setWidget function
  to set the current page shown.
  \relatesalso HarixApp
 */
void selectWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->setWidget(widget);
}



//! Global function to remove a widget(page) to be shown.
/*! 
  Calls the HarixApp::removeWidget function with widget argument to be deleted.
  \relatesalso HarixApp
 */
void removeWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->removeWidget(widget);
}



void HarixApp::internalPathChanged(const std::string& path)
{
  // Define pointers to widgets(pages) to be shown
  HomeWidget* home_page;
  AnalyzeOsWidget* analyze_page;
  PcimapQueryWidget* querydev_page;
  PcimapResultWidget* queryresult_page;
  MainboardsWidget* mainboards_page;

  /*
   * Reset pages on URL changes to their defaults!
   * @{
   */
  if ( (analyze_page = AnalyzeOsWidget::Instance()) != NULL ){
	analyze_page->resetAll();
  }
  if ( (querydev_page = PcimapQueryWidget::Instance()) != NULL ){
	querydev_page->resetAll();
  }
  if ( (queryresult_page = PcimapResultWidget::Instance()) != NULL ){
	queryresult_page->resetAll(); // Deletes the widget, PcimapResultWidget is recreated in every call.
  }
  if ( (mainboards_page = MainboardsWidget::Instance()) != NULL ){
	mainboards_page->resetAll(); // Deletes the widget, MainboardsWidget is recreated in every call.
  }
  /*
   * @}
   */

  
  /*
   * React appropriately to page requests
   * @{
   */
  if( path == "/home"){
	if ( (home_page = HomeWidget::Instance()) != NULL )
	  selectWidget( home_page ); 	// Show the home page
	else{
	  wApp->log("debug") << "This should not happen!!!";
	  assert(0);					// Kill the application
	}
  }
  else if( path == "/analyze_os"){
	if ( ( analyze_page = AnalyzeOsWidget::Instance()) == NULL )
	  selectWidget( AnalyzeOsWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it..
	else
	  selectWidget( analyze_page );
  }
  else if( path == "/query_devices"){
	if ( ( querydev_page = PcimapQueryWidget::Instance()) == NULL )
	  selectWidget( PcimapQueryWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it.
	else
	  selectWidget( querydev_page );
  }
  else if( path == "/query_results"){
	// Don't allow direct URL access to PcimapResultWidget, and redirect to PcimapQueryWidget.
	WApplication::instance()->setInternalPath("/query_devices",true);
  }
  else if( path == "/mainboards"){
	if ( (mainboards_page = MainboardsWidget::Instance()) == NULL )
	  selectWidget( MainboardsWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it..
	else
	  selectWidget( mainboards_page );
  }
  /*
   * @}
   */

}
