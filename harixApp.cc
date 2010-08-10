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
#include <Wt/WEnvironment>		// To retrieve the initial URL on app start

#include "harixApp.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "lspci_query/pcimapResult.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "mainboards/mainboards.hpp"
#include "home.hpp"
#include "pci_device.hpp"


using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");
  setCssTheme("polished"); // use the polished theme folder from /resources/themes/
  useStyleSheet("resources/stylizr.css");


  useStyleSheet("wt_ie.css","IE gte 7");
  useStyleSheet("wt_ie6.css","IE gte 6");


  // Common stack to hold any pages..
  stackedContent_ = new WStackedWidget(root());

  // Create an HomeWidget(representing the home page) instance
  HomeWidget* home_page = HomeWidget::Instance(stackedContent_);
  
  std::string internal_path =  env.internalPath(); // get the initial URL at App start.

  if( internal_path.compare(0,15,"/query_results~") == 0 ){ // if this is a query link, open Query Results page.
	internalPathChanged(internal_path);
  }
  else{
	WApplication::instance()->setInternalPath("/home",true); // set URL to /application_path.wt#/home
  }


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


static std::vector<PciDevice> getBoardDeviceList( std::string board_name );



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
  else if( path.compare(0,15,"/query_results~") == 0 ){ // if mainboard and os is provided at URL

	if ( path.compare(15,6,"board=") == 0 ) {
	  std::string board_name, ukernel_id;
	  std::string::size_type beg_pos, end_pos;

	  beg_pos = path.find_first_of("=");
	  end_pos = path.find_first_of("&");
	  board_name = path.substr( beg_pos+1, end_pos-beg_pos-1 );

	  beg_pos = path.find_first_of("=",end_pos);
	  ukernel_id = path.substr(beg_pos+1);
	  
	  // Retrieve PCI devices of the mainboard.
	  std::vector<PciDevice> device_list = getBoardDeviceList( board_name );

	  PcimapResultWidget* pcilist;
	  if ( (pcilist = PcimapResultWidget::Instance()) == NULL ){

		// Switch to PcimapResultWidget providing mainboard name and OS ID too..
		selectWidget( PcimapResultWidget::Instance( device_list, stackedContent_,
													board_name,
													ukernel_id) );
	  }
	  else
		wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
	  
	}
  }
  /*
   * @}
   */

}


/*
  Retrieves ID of the mainboard, returns an empty string if it does not exist in database.
*/
//! \relates database.cc
std::string queryBoardModelId( std::string board_name );

/*
  Retrieve unique device ID list of devices belonging to given mainboard.
*/
//! \relates database.cc
std::vector<std::string> queryBoardDeviceList( std::string board_id );

/*
  Retrieve hardware codes of a device depending on it's bus type.
*/
//! \relates database.cc
std::vector<std::string> queryDeviceCodes( std::string udev_id, int bus_type = 1 );


static std::vector<PciDevice> getBoardDeviceList( std::string board_name  )
{
  std::vector<PciDevice> device_list; 			// Hold list of PCI Devices to be returned.

  std::string board_id;
  board_id = queryBoardModelId( board_name );
  
  std::vector<std::string> udev_id_list;		   // Hold list of Unique device IDs.
  udev_id_list = queryBoardDeviceList( board_id ); // Get list of Unique device IDs belonging to the mainboard.

  
  // Hold the PCI Device codes as in given index below:
  // 0-vendor; 1-device; 2-subvendor; 3-subdevice; 4-class; 5-subclass; 6-progif
  std::vector<std::string> device_codes;


  PciDevice *pci_device;

  // Iterate through the list of retrieved Unique device IDs.
  std::vector<std::string>::iterator udev_iter;
  for( udev_iter = udev_id_list.begin(); udev_iter != udev_id_list.end(); ++udev_iter ){
	
	device_codes = queryDeviceCodes( *udev_iter ); // Retrieve Device codes of current PCI device.

	pci_device = new PciDevice();

	pci_device->setVendor( device_codes.at(0) );
	pci_device->setDevice( device_codes.at(1) );
	pci_device->setSubvendor( device_codes.at(2) );
	pci_device->setSubdevice( device_codes.at(3) );
	pci_device->setClass( device_codes.at(4) );
	pci_device->setSubclass( device_codes.at(5) );
	pci_device->setProgif( device_codes.at(6) );

	device_list.push_back(*pci_device);
	delete pci_device;
  }

  return device_list;
}
