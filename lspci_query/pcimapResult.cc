/* pcimapResult.cc --- 
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
  \file   pcimapResult.cc
  \date   Fri Jul 23 18:58:34 2010
  
  \brief  Implementation of PcimapResultWidget Class
*/
#include <Wt/WPanel>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTableView>
#include <Wt/WLogger>
#include <Wt/WTable>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WAnchor>


#include "../harixApp.hpp"			// needed for wApp.
#include "pcimapResult.hpp"
#include "pcimapQuery.hpp"
#include "../pci_device.hpp"
#include "../div.hpp"

using namespace Wt;
using std::string;

//! \relates harixApp.cc
void selectWidget( WContainerWidget* widget );

//! \relates harixApp.cc
void removeWidget( WContainerWidget* widget );


/*
  Query full name of an OS with it's ID.
*/
//! \relates database.cc
std::vector<string> queryOs( string unique_kernel_id );


//! Path to icons to be shown for specific OS support queries.
static const char *supportIcons[] = {
  "resources/button_cancel_16x16.png",
  "resources/button_ok_16x16.png"
};


PcimapResultWidget* PcimapResultWidget::instance_ = NULL; // Pointer to the only instance of PcimapResultWidget.
int PcimapResultWidget::instance_count = 0;				  // Number of instances. Not of any use just for testing!



PcimapResultWidget::PcimapResultWidget( std::vector<PciDevice>& lspci_list,
										WContainerWidget* parent,
										string board_name,
										string ukernel_id) : WContainerWidget(parent)
{
  ++instance_count;
  parent_ = parent;

  /*
   * Header part for navigation
   * @{
   */
  WContainerWidget *result;
  addWidget( result = new WContainerWidget() );
  Div *header = new Div(result, "header");
  Div *logo = new Div(header, "widgparty");
  logo->addWidget( new WText("<p>hari<a>X</a></p>") );
  Div *content = new Div(result, "content");
  Div *menu = new Div(content, "menu");
  WContainerWidget *menulist = new WContainerWidget(menu);
  menulist->setList(true);
  
  WAnchor *aHome;
  menulist->addWidget( aHome = new WAnchor("","Home") );
  aHome->setRefInternalPath("/home");
  WAnchor *aAnalyzeOs;
  menulist->addWidget( aAnalyzeOs = new WAnchor("","Analyze OS") );
  aAnalyzeOs->setRefInternalPath("/analyze_os");
  WAnchor *aQueryDevices;
  menulist->addWidget( aQueryDevices = new WAnchor("","Query Devices") );
  aQueryDevices->setRefInternalPath("/query_devices");
  aQueryDevices->setStyleClass("selected");
  WAnchor *aMainboards;
  menulist->addWidget( aMainboards = new WAnchor("","Mainboards") );
  aMainboards->setRefInternalPath("/mainboards");

  /*
	Container 'page' holding contents shown in page apart from the Header Part.
	Anything else from the Header to be shown in page is added to 'page' container.
  */
  Div *page = new Div(result, "pagequeryresult");
  /*
   * @}
   */
  

  
  uKernelId_ = ukernel_id;		// Get the unique OS ID if passed, default is empty string.

  lspci_list_ = lspci_list;		// Get the list of devices to be shown.

  

  /*
   * Device List Panel
   * @{
   */
  page->addWidget( panelTable_ = new WPanel() );
  panelTable_->resize(1000,300);
  panelTable_->setCentralWidget( lspciTable_ = new WTableView() );

  lspciTable_->setSortingEnabled(false); 				// Disable sorting in device list table
  lspciTable_->setSelectionMode(SingleSelection);		// and set single line selection.
  lspciTable_->setAlternatingRowColors(true);

  
  if( ukernel_id == "" ){		// if No OS is passed as argument then set the model accordingly.
	lspciModel_ = new WStandardItemModel( 0, 3, this );
	lspciModel_->setHeaderData(0, Horizontal, string("Class"));
	lspciModel_->setHeaderData(1, Horizontal, string("Vendor"));
	lspciModel_->setHeaderData(2, Horizontal, string("Device"));
	lspciTable_->setColumnWidth(0, WLength(240));
	lspciTable_->setColumnWidth(1, WLength(200));
	lspciTable_->setColumnWidth(2, WLength(500));
	if( board_name != "" ){		// if a mainboard name is passed, then show it at panel title.
	  panelTable_->setTitle("Configuration of Mainboard: " + board_name );
	}
  }
  else {						// if an OS ID is passed, put an extra column in model for OS support of device.
	lspciModel_ = new WStandardItemModel( 0, 4, this );
	lspciModel_->setHeaderData(0, Horizontal, string("Class"));
	lspciModel_->setHeaderData(1, Horizontal, string("Vendor"));
	lspciModel_->setHeaderData(2, Horizontal, string("Device"));
	lspciModel_->setHeaderData(3, Horizontal, string("Support"));
	lspciTable_->setColumnWidth(0, WLength(200));
	lspciTable_->setColumnWidth(1, WLength(180));
	lspciTable_->setColumnWidth(2, WLength(500));
	lspciTable_->setColumnWidth(3, WLength(60));

	// Then query full name of OS and show it at panel title.
	std::vector<string> current_os = queryOs( uKernelId_ );
	panelTable_->setTitle("OS: "+ current_os.at(0) + " " + current_os.at(1) + " " +
						  current_os.at(2) + "-" + current_os.at(3) +
						  " support for Mainboard: " + board_name + "" );
  }


  // Append each device to be listed to the source model with calls to pciDeviceToRowItem() function.
  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list_.begin(); lspci_iter != lspci_list_.end(); ++lspci_iter ){
	lspciModel_->appendRow( pciDeviceToRowItem( &(*lspci_iter) ) );
  }
  

  // Set the source model for the table for listing devices.
  lspciTable_->setModel(lspciModel_);
  
  /*
   * @}
   */

  
  // Trigger lspciTableRowSelected() function on double click of a line in lspciTable_.
  lspciTable_->doubleClicked().connect(this, &PcimapResultWidget::lspciTableRowSelected);

  page->addWidget( new WBreak() );

  

  /*
   * Device Details and OS Support Panel
   * @{
   */

  // Set fields for OS Support Model containing drivers.
  osSupportModel_ = new WStandardItemModel( 0, 5, this );
  osSupportModel_->setHeaderData(0, Horizontal, string("OS"));
  osSupportModel_->setHeaderData(1, Horizontal, string("Release"));
  osSupportModel_->setHeaderData(2, Horizontal, string("Kernel"));
  osSupportModel_->setHeaderData(3, Horizontal, string("Arch."));
  osSupportModel_->setHeaderData(4, Horizontal, string("Driver"));

  page->addWidget( panelSupport_ = new WPanel() );
  panelSupport_->resize(1000, WLength() );
  panelSupport_->setTitle("Device Details");


  Wt::WTable* layoutSupport;
  panelSupport_->setCentralWidget( layoutSupport = new WTable() );
  
  // Set fields for device codes..
  layoutSupport->rowAt(0)->setHeight( WLength(25) );
  layoutSupport->elementAt(0,0)->addWidget( new WText("Vendor:") );
  layoutSupport->elementAt(0,1)->addWidget( editVendor_ = new WLineEdit() );
  layoutSupport->elementAt(0,2)->addWidget( new WText("Device:") );
  layoutSupport->elementAt(0,3)->addWidget( editDevice_ = new WLineEdit() );
  layoutSupport->elementAt(0,4)->addWidget( new WText("Subvendor:") );
  layoutSupport->elementAt(0,5)->addWidget( editSubvendor_ = new WLineEdit() );
  layoutSupport->elementAt(0,6)->addWidget( new WText("Subdevice:") );
  layoutSupport->elementAt(0,7)->addWidget( editSubdevice_ = new WLineEdit() );
  layoutSupport->elementAt(0,8)->addWidget( new WText("Class:") );
  layoutSupport->elementAt(0,9)->addWidget( editClass_ = new WLineEdit() );
  layoutSupport->elementAt(0,10)->addWidget( new WText("Subclass:") );
  layoutSupport->elementAt(0,11)->addWidget( editSubclass_ = new WLineEdit() );
  layoutSupport->elementAt(0,12)->addWidget( new WText("Progif:") );
  layoutSupport->elementAt(0,13)->addWidget( editProgif_ = new WLineEdit() );

  editVendor_->setReadOnly(true);
  editVendor_->resize( WLength(40), WLength::Auto );
  editDevice_->setReadOnly(true);
  editDevice_->resize( WLength(40), WLength::Auto );
  editSubvendor_->setReadOnly(true);
  editSubvendor_->resize( WLength(40), WLength::Auto );
  editSubdevice_->setReadOnly(true);
  editSubdevice_->resize( WLength(40), WLength::Auto );
  editClass_->setReadOnly(true);
  editClass_->resize( WLength(20), WLength::Auto );
  editSubclass_->setReadOnly(true);
  editSubclass_->resize( WLength(20), WLength::Auto );
  editProgif_->setReadOnly(true);
  editProgif_->resize( WLength(20), WLength::Auto );
  
  layoutSupport->elementAt(1,0)->setColumnSpan(14);
  layoutSupport->elementAt(1,0)->addWidget( new WText("Operating System Support Details:") );
  layoutSupport->rowAt(1)->setHeight( WLength(25) );
  
  layoutSupport->elementAt(2,0)->setColumnSpan(14);
  layoutSupport->elementAt(2,0)->addWidget( osSupportTable_ = new WTableView() );
  layoutSupport->rowAt(2)->setHeight( WLength(250) );


  WPushButton *bSaveBoard;
  layoutSupport->elementAt(3,0)->addWidget( bSaveBoard = new WPushButton("Save Mainboard!") );

  // Trigger bSaveBoard_Click() function on 'Save Mainboard' click.
  bSaveBoard->clicked().connect(this, &PcimapResultWidget::bSaveBoard_Click);
  

  // Set the source model for the table listing OS supports and their modules.
  osSupportTable_->setModel(osSupportModel_);
  osSupportTable_->resize(950,250);
  osSupportTable_->setColumnWidth(0, WLength(200));
  osSupportTable_->setColumnWidth(1, WLength(200));
  osSupportTable_->setColumnWidth(2, WLength(200));
  osSupportTable_->setColumnWidth(3, WLength(100));
  osSupportTable_->setColumnWidth(4, WLength(200));
  /*
   * @}
   */

  
  
  if( board_name != "" ){			// if a board name is passed as argument,then disable 'Save Mainboard' button.
	bSaveBoard->disable();
  }
  else {							// if not then prepare the dialog window to be shown for status of save.
	dialogWarn_ = new WDialog();

	txtWarn_ = new WText(dialogWarn_->contents());
	new WBreak( dialogWarn_->contents() );
	WPushButton *dialogWarnClose = new WPushButton("Close", dialogWarn_->contents());

	dialogWarnClose->clicked().connect( SLOT(dialogWarn_, WDialog::accept) );
  }

}



PcimapResultWidget*
PcimapResultWidget::Instance( std::vector<PciDevice> lspci_list,
							  WContainerWidget* parent,
							  string board_name, // passed if showing a mainboard configuration
							  string ukernel_id) // passed if a specific OS support queried..
{
  // create an instance of PcimapResultWidget, with default parameters if they are not passed as argument.
  instance_ = new PcimapResultWidget(lspci_list, parent, board_name, ukernel_id);

  return instance_;
}

PcimapResultWidget* PcimapResultWidget::Instance()
{
  if ( !instance_ ){
	// there is no instance of PcimapResultWidget, so return NULL..
	return NULL;
  }

  return instance_;
}


PcimapResultWidget::~PcimapResultWidget()
{
  instance_ = NULL;				// reset static variables.
  instance_count = 0;
  
  lspci_list_.clear();			// and clear list of received devices.
}



/*
  Query full class name of a device with provided codes.
*/
//! \relates database.cc
std::vector<string>
queryClassName(string class_code,string subclass_code,string progif_code);


/*
  Query full device name of a device with provided codes.
*/
//! \relates database.cc
std::vector<string>
queryDeviceName(string vendor_code,string device_code,string subvendor_code,string subdevice_code);


/*
  Query Vendor name of a device with provided code.
*/
//! \relates database.cc
string queryPciIdsVendorName( string vendor_code );


/*
  Query the list of OSes that support the device with provided codes.
*/
//! \relates database.cc
std::multimap<string, string>
queryPcimapOsList(string vendor_code, string device_code, string subvendor_code, string subdevice_code,
				  string class_code, string subclass_code, string progif_code);





std::vector<WStandardItem*>
PcimapResultWidget::pciDeviceToRowItem( const PciDevice* current_item )
{
  std::vector<string> full_class, full_device; 	// Hold class and device names to be shown.
  
  std::vector<WStandardItem *> result; 			// Hold the line to be returned.
  WStandardItem *item;

  full_class =  queryClassName(current_item->getClass(), 	// Returns Class,Subclass,Progif names..
							   current_item->getSubclass(),
							   current_item->getProgif());

  full_device = queryDeviceName(current_item->getVendor(), 	// Returns Vendor,Device,Subsys names..
								current_item->getDevice(),
								current_item->getSubvendor(),
								current_item->getSubdevice());

  // column 0:
  if( !full_class.empty() ){
	item = new WStandardItem( full_class.at(1) ); 		// Put just the Subclass name..
  }
  else
	item = new WStandardItem("Unknown Class"); 			// class-subclass pair doesnot exist in db!

  result.push_back(item);

  if( !full_device.empty() ){
	// column 1: 
	item = new WStandardItem( full_device.at(0) ); 		// Put the Vendor name..
	result.push_back(item);

	if( full_device.at(1) != "" )
	  // column 2: 
	  item = new WStandardItem( full_device.at(1) ); 	// Put the Device name..
	else
	  item = new WStandardItem( "Unknown Device" );
	result.push_back(item);	  

  }
  else{							// Vendor-Device pair does not exist in database!
	string vendor_name;
	if( (vendor_name = queryPciIdsVendorName(current_item->getVendor())) == "" ) // Query only for Vendor name.
	  item = new WStandardItem( "Unknown Vendor" );
	else
	  item = new WStandardItem( vendor_name );
	result.push_back(item);
  	item = new WStandardItem( "Unknown Device" );
	result.push_back(item);
  }

  if( uKernelId_ != "" ){		// if unique OS ID is provided then put an extra Support column.
	item = new WStandardItem();

	// Query the list of OSes that support the current device!
	std::multimap<string,string> uKernel_mod_list; // (unique Kernel id, module name id)
	uKernel_mod_list = queryPcimapOsList(current_item->getVendor(), current_item->getDevice(),
										 current_item->getSubvendor(),current_item->getSubdevice(),
										 current_item->getClass(),current_item->getSubclass(),current_item->getProgif());

	// And search if the provided OS exists in the list of OSes that support the device..
	std::multimap<string,string>::iterator ukernel_mod_iter;
	ukernel_mod_iter = uKernel_mod_list.find( uKernelId_ );
	if( ukernel_mod_iter == uKernel_mod_list.end() ){ 	// NO module found for the Device with provided OS!!
	  item->setIcon( supportIcons[0] );
	}
	else{												// Modules found for the device..
	  item->setIcon( supportIcons[1] );
	}
	result.push_back(item);	  
  }
  
  return result;
}




/*
  Query Name of the Module having the provided ID.
*/
//! \relates database.cc
string queryModuleName( string mod_name_id );


void PcimapResultWidget::lspciTableRowSelected(WModelIndex index, WMouseEvent event)
{
  PciDevice* selected_item;
  selected_item = ( &lspci_list_.at(index.row()) ); 	// Put a pointer to the selected item on device list.
  fillDeviceDetails( *selected_item );					// Display codes of selected device.

  std::multimap<string,string> uKernel_mod_list; 		// (unique Kernel id, module name id)

  // Query the list of OSes that support the current device!
  uKernel_mod_list = queryPcimapOsList( selected_item->getVendor(), selected_item->getDevice(),
										selected_item->getSubvendor(),selected_item->getSubdevice(),
										selected_item->getClass(), selected_item->getSubclass(), selected_item->getProgif() );

  
  std::vector<string> os;								// Hold the full OS name,
  string module_name;									// And the name of the module in that OS.
  std::vector< std::vector<string> > os_list;			// Make a list of entries from the above two.

  
  // Iterate through the list of queried OSes!
  std::multimap<string,string>::iterator ukernel_iter;
  for ( ukernel_iter=uKernel_mod_list.begin(); ukernel_iter!=uKernel_mod_list.end(); ++ukernel_iter ){
	
  	os = queryOs( ukernel_iter->first ); 					// Retrieve full OS name from it's ID,
  	module_name = queryModuleName( ukernel_iter->second );	// and the module that supports the device.
	// Append the module to the list, so we know which module of what OS supports the device!
  	os.push_back(module_name);

  	os_list.push_back(os);									// Put the OS in list to be displayed.
  }

  osSupportModel_->removeRows(0, osSupportModel_->rowCount()); 	// Clear the source model of table.
  if( !os_list.empty() ){
	std::vector< std::vector<string> >::iterator os_iter;
	for( os_iter=os_list.begin(); os_iter!=os_list.end(); ++os_iter )
	  // Call osSupportRow() function with each OS entry to create a corresponding line
	  // and add each line to model source to be displayed.
	  osSupportModel_->appendRow( osToSupportRow( *os_iter ) );
  }

}



std::vector<WStandardItem*>
PcimapResultWidget::osToSupportRow( const std::vector<string>& os )
{
  std::vector<WStandardItem *> result; 			// Hold the line to be returned.
  WStandardItem *item;
  if( !os.empty() ){
	// column 0:
	item = new WStandardItem( os.at(0) ); 		// Put distribution name.
	result.push_back(item);

	// column 1: 
	item = new WStandardItem( os.at(1) ); 		// Put release name.
	result.push_back(item);
  
	// column 2: 
	item = new WStandardItem( os.at(2) ); 		// Put kernel version.
	result.push_back(item);

	// column 3:
	item = new WStandardItem( os.at(3) ); 		// Put architecture.
	result.push_back(item);

	// column 4:
	item = new WStandardItem( os.at(4) ); 		// Put module name.
	result.push_back(item);

  }

  return result;
}



void PcimapResultWidget::fillDeviceDetails( const PciDevice& selected_device )
{
  // Fill in the edit line with device codes.
  editVendor_->setText( selected_device.getVendor() );
  editDevice_->setText( selected_device.getDevice() );
  editSubvendor_->setText( selected_device.getSubvendor() );
  editSubdevice_->setText( selected_device.getSubdevice() );
  editClass_->setText( selected_device.getClass() );
  editSubclass_->setText( selected_device.getSubclass() );
  editProgif_->setText( selected_device.getProgif() );
}




void PcimapResultWidget::bSaveBoard_Click()
{
  dialogBoard_ = new WDialog("Save Mainboard");

  // Ask user for the mainboard name to be saved.
  new WText("Store Mainboard configuration on Database! <br />Model Name: ", dialogBoard_->contents() );
  editBoard_ = new WLineEdit( dialogBoard_->contents() );
  new WBreak( dialogBoard_->contents() );
  WPushButton* dialogBoardOk = new WPushButton("Ok", dialogBoard_->contents());
  WPushButton* dialogBoardCancel = new WPushButton("Cancel", dialogBoard_->contents());

  // Sets WDialog::DialogCode to Accepted when OK is clicked.
  (*dialogBoardOk).clicked().connect( SLOT(dialogBoard_, WDialog::accept) );

  // Sets WDialog::DialogCode to Rejected when Cancel is clicked.
  (*dialogBoardCancel).clicked().connect( SLOT(dialogBoard_, WDialog::reject) );

  dialogBoard_->show();

  // Trigger 'dialogBoard_Close' function on 'dialogBoard_' dialog window close.
  dialogBoard_->finished().connect(this, &PcimapResultWidget::dialogBoard_Close);
}


void PcimapResultWidget::dialogBoard_Close( WDialog::DialogCode dialog_code )
{
  if ( dialog_code == WDialog::Accepted ){ 				// if the user wants to save the mainboard.

	string board_name = editBoard_->text().narrow(); 	// retrieve the name given by the user.
	if( board_name != "" ){
	  storeMainBoard( board_name ); 					// store the mainboard configuration..
	  delete dialogBoard_;
	}
	else
	  dialogBoard_->show();
  }
}



/*
  Query ID of the mainboard with provided name.
*/
//! \relates database.cc
string queryBoardModelId( string board_name );

/*
  Store the provided mainboard in database.
*/
//! \relates database.cc
string insertBoardModel( string board_name );

/*
  Store devices of given mainboard in database.
*/
//! \relates database.cc
void insertBoardDevices( string board_id, std::vector<string>& device_id_list );

//! To temporarily store list of PCI devices on page refresh.
static std::vector<PciDevice> lspci_list_backup;


void PcimapResultWidget::storeMainBoard( string board_name )
{
  std::vector<string> uDevIdList; 						// Hold unique device IDs.
  string board_id = queryBoardModelId( board_name );	// Query to see if board already exists in database.
  
  if ( board_id  == "" ){								// Board does not exist in database..
	board_id = insertBoardModel( board_name );			// insert it in database and retrieve it's ID.

	uDevIdList = getUniqueDevIdList(); 					// Get unique IDs of devices in lspci_list_.
	
	insertBoardDevices( board_id, uDevIdList ); 		// Insert devices belonging to the board to database.

	// @TODO: !!! ALSO INSERT INTO DEV_MOD.. something like: checkDeviceModules( uDevIdList );
	//! \todo FUTURE WORK: Also insert into db `dev_mod' table.. something like: checkDeviceModules( uDevIdList );

	dialogWarn_->setWindowTitle("Mainboard");
	txtWarn_->setText("Mainboard Saved!");
	dialogWarn_->show();

	lspci_list_backup = lspci_list_; 			// Stash lspci_list_ so we can refresh the page with board name.

	// So remove the page, delete it and create a new instance providing currently saved board name.
	removeWidget( PcimapResultWidget::Instance() );
	delete PcimapResultWidget::Instance();
	selectWidget( PcimapResultWidget::Instance( lspci_list_backup, parent_, board_name) );
  }
  else{													// Board already exists, so we do nothing, just warn!
	dialogWarn_->setWindowTitle("Mainboard Exists");
	txtWarn_->setText("Mainboard with the given name already exists in database!");
	dialogWarn_->show();
  }
  
}



/*
  Check if the given PCI device is stored in database before.
*/
//! \relates database.cc
string checkPciSpcId( const PciDevice* const currentPciDevice );

/*
  Check if the given device is stored in database before.
  Might be any device depending on bus type, designed for future growth of database with different busses.
*/
//! \relates database.cc
string checkUniqueDeviceId( string dev_special_id, int bus_type = 1 );


std::vector<string> PcimapResultWidget::getUniqueDevIdList()
{
  std::vector<string> uDevIdList; 					// Hold unique device IDs of listed PCI Devices.
  string pciSpcId, uDevId;							// Hold PCI special ID and unique device ID.

  // Iterate through the listed PCI Devices.
  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list_.begin(); lspci_iter != lspci_list_.end(); ++lspci_iter ){

	pciSpcId = checkPciSpcId( &*lspci_iter ); // Returns PCI special ID from `pci_all' db table,adds if not exists
	uDevId = checkUniqueDeviceId( pciSpcId ); // Returns unique device ID from `all_devices', adds if not exists..
	
	uDevIdList.push_back( uDevId );
	
  }

  return uDevIdList;
}



void PcimapResultWidget::resetAll()
{
  removeWidget( PcimapResultWidget::Instance() );
  delete PcimapResultWidget::Instance();
}
