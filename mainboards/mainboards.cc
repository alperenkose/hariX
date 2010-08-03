/*!
  \file   mainboards.cc
  \date   Wed Jul 28 10:58:30 2010
  
  \brief  Implementation of MainboardsWidget Class
  
*/
#include <Wt/WTable>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WLogger>

#include "../harixApp.hpp"		// needed for wApp
#include "../pci_device.hpp"
#include "../lspci_query/pcimapResult.hpp"
#include "../div.hpp"
#include "mainboards.hpp"

using namespace Wt;


//! \relates harixApp.cc
void selectWidget( WContainerWidget* widget );


MainboardsWidget* MainboardsWidget::instance_ = NULL; // Pointer to the only instance of MainboardsWidget.



MainboardsWidget::MainboardsWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
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
  WAnchor *aMainboards;
  menulist->addWidget( aMainboards = new WAnchor("","Mainboards") );
  aMainboards->setRefInternalPath("/mainboards");
  aMainboards->setStyleClass("selected");

  /*
	Container 'page' holding contents shown in page apart from the Header Part.
	Anything else from the Header to be shown in page is added to 'page' container.
  */
  Div *page = new Div(content, "pagearea");
  
  /*
   * @}
   */

  

  WTable* layoutMainboard;

  page->addWidget( layoutMainboard = new WTable() );

  mainboards_ = new WStandardItemModel(this);
  fillMainboards(); 								// Fill in the source model for selection box of mainboards.


  boardsProxyModel_ = new WSortFilterProxyModel(this); // Setup a proxy model, to filter and sort on source model.
  boardsProxyModel_->setSourceModel(mainboards_); 	// Set the source model to mainboards_ model.
  // boardsProxyModel_->setDynamicSortFilter(true);
  boardsProxyModel_->sort(1);						// Set sorting on first column.

  
  layoutMainboard->columnAt(0)->setWidth( WLength(160) );
  layoutMainboard->elementAt(0,0)->setColumnSpan(2);
  layoutMainboard->elementAt(0,0)->addWidget( new WText("Mainboard Configuration:") );

  
  boardsSelection_ = new WSelectionBox(layoutMainboard->elementAt(1,0));
  boardsSelection_->setModel(boardsProxyModel_); 	// Set model for mainboards list box to boardsProxyModel_.
  boardsSelection_->setModelColumn(1);
  boardsSelection_->resize( 150, WLength() );
  boardsSelection_->setVerticalSize(10);


  WTable *layConfButtons;
  layoutMainboard->elementAt(2,0)->addWidget( layConfButtons = new WTable() );
  layConfButtons->columnAt(0)->setWidth( WLength(80) );
  layConfButtons->columnAt(1)->setWidth( WLength(70) );

  WPushButton* bBoardDevices;
  layConfButtons->elementAt(0,0)->addWidget( bBoardDevices = new WPushButton("Show") );

  // Trigger bBoardDevices_Click() function on 'Show' click.
  bBoardDevices->clicked().connect(this, &MainboardsWidget::bBoardDevices_Click);
	
  dialogWarn_ = 0;


  WPushButton *bDeleteBoard;
  layConfButtons->elementAt(0,1)->setContentAlignment(AlignRight);
  layConfButtons->elementAt(0,1)->addWidget( bDeleteBoard = new WPushButton("Delete") );

  // Trigger bDeleteBoard_Click() function on 'Delete' click.
  bDeleteBoard->clicked().connect(this, &MainboardsWidget::bDeleteBoard_Click);
  

  layoutMainboard->columnAt(1)->setWidth( WLength(80) );

  // Create the ComboBoxes for OS selection.
  layoutMainboard->columnAt(2)->setWidth( WLength(180) );
  layoutMainboard->elementAt(0,2)->addWidget( new WText("OS specific support?") );
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboOS_ = new WComboBox() );
  comboOS_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboRel_ = new WComboBox() );
  comboRel_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboKer_ = new WComboBox() );
  comboKer_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  
  layoutMainboard->elementAt(1,2)->addWidget( bCheckOs_ = new WPushButton("Check!") );

  // Trigger bCheckOs_Click() function on 'Check' click.
  bCheckOs_->clicked().connect(this, &MainboardsWidget::bCheckOs_Click);
  bCheckOs_->disable();

  if( boardsSelection_->count() == 0 ){		// if there is no mainboard saved yet, then disable all controls.
	bBoardDevices->disable();
	bDeleteBoard->disable();
	comboOS_->disable();
  }

  osModel_ = new WStandardItemModel(this); 	// Holds list of all Distributions.
  fillOsModel();							// Fill in the osModel_ from database.
  comboOS_->setModel(osModel_);				// Set the model for Distribution selection ComboBox to osModel_.
  comboOS_->setModelColumn(1);

  releaseModel_ = new WStandardItemModel(this); // Holds list of Releases of a Distribution.
  comboRel_->setModel( releaseModel_ ); 		// Set the model for Release selection ComboBox to releaseModel_.
  comboRel_->setModelColumn(1);

  kernelModel_ = new WStandardItemModel(this); // Holds list of Kernels of a Distribution Release.
  comboKer_->setModel( kernelModel_ );		   // Set the model for Kernel selection ComboBox to kernelModel_.
  comboKer_->setModelColumn(1);				   // shows kernelVersion and architecture together; 2.6.32 - i386

  
  // Trigger osSelectionChanged() function when selection changes in Distribution selection combobox.
  comboOS_->activated().connect( this, &MainboardsWidget::osSelectionChanged );
  
  // Trigger releaseSelectionChanged() function when selection changes in Release selection combobox.
  comboRel_->activated().connect( this, &MainboardsWidget::releaseSelectionChanged );

  comboRel_->disable();
  comboKer_->disable();

}


MainboardsWidget* MainboardsWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // we didn't receive a parent and there is no instance of PcimapQueryWidget, so return NULL..
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new MainboardsWidget(parent);
	}
	// else don't care, we already have an instance we will return that.
  }
  return instance_;
}


MainboardsWidget::~MainboardsWidget()
{
  instance_ = NULL;				// reset static variable.
}



/*
  Retrieve all mainboards saved previously as ( boardID, boardName ) pairs.
*/
//! \relates database.cc
std::multimap<std::string, std::string> getBoardList();


void MainboardsWidget::fillMainboards()
{
  mainboard_list_ = getBoardList(); 		// Get list of Mainboards stored in database.

  std::vector<WStandardItem *> result; 		// Line to be added to mainboards_ model.
  WStandardItem *item;

  // Iterate through the list of retrieved mainboards.
  std::multimap<std::string,std::string>::iterator board_iter;
  for ( board_iter=mainboard_list_.begin(); board_iter!=mainboard_list_.end(); ++board_iter ){
	item = new WStandardItem( board_iter->first ); 	// boardID column
	result.push_back(item);
	item = new WStandardItem( board_iter->second ); // boardName column
	result.push_back(item);

	mainboards_->appendRow(result); 			// Add an entry to the model.
	result.clear();
  }
}



void MainboardsWidget::bBoardDevices_Click()
{
  std::string board_id = getBoardIdentifier(); 		// Retrieve mainboard ID.
  
  showBoardConfiguration( board_id ); 				// Call to switch to Query Devices Result page.
}



void MainboardsWidget::showBoardConfiguration( std::string board_id  )
{
  std::vector<PciDevice> device_list = getBoardDeviceList( board_id ); // Retrieve PCI Devices of the Mainboard.

  PcimapResultWidget* pcilist;
  if ( (pcilist = PcimapResultWidget::Instance()) == NULL ){

	// Switch to PcimapResultWidget with mainboard name passed as argument.
	selectWidget( PcimapResultWidget::Instance(device_list,parent_, boardsSelection_->currentText().narrow()) );

	// Remove and delete the current instance of MainboardsWidget.
	resetAll();
	
	WApplication::instance()->setInternalPath("/query_results"); // Set URL to point the new page.
  }
  else
	wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
}





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


std::vector<PciDevice> MainboardsWidget::getBoardDeviceList( std::string board_id  )
{
  std::vector<PciDevice> device_list; 			// Hold list of PCI Devices to be returned.
  
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



void MainboardsWidget::bDeleteBoard_Click()
{
  dialogWarn_ = new WDialog();
  dialogWarn_->setWindowTitle("Mainboard");
  new WText("Sure you want to delete the selected Mainboard?", dialogWarn_->contents());
  new WBreak( dialogWarn_->contents() );
  WPushButton *dialogWarnYes = new WPushButton("Yes", dialogWarn_->contents());

  // Sets WDialog::DialogCode to Accepted when Yes is clicked.
  dialogWarnYes->clicked().connect( SLOT(dialogWarn_, WDialog::accept) );

  WPushButton *dialogWarnNo = new WPushButton("No", dialogWarn_->contents());
  
  // Sets WDialog::DialogCode to Rejected when No is clicked.
  dialogWarnNo->clicked().connect( SLOT(dialogWarn_, WDialog::reject) );

  dialogWarn_->show();

  // Trigger 'dialogWarn_Close' function on 'dialogWarn_' dialog window close.
  dialogWarn_->finished().connect(this, &MainboardsWidget::dialogWarn_Close);
}



/*
  Delete Mainboard with given ID.
*/
//! \relates database.cc
int deleteBoardModel( std::string board_id ); 


void MainboardsWidget::deleteBoard()
{
  std::string board_id = getBoardIdentifier(); 		// Get ID of the selected mainboard.
  
  dialogWarn_ = new WDialog();
  
  WText *txtWarn = new WText(dialogWarn_->contents());
  new WBreak( dialogWarn_->contents() );
  WPushButton *dialogWarnClose = new WPushButton("Close", dialogWarn_->contents());

  dialogWarnClose->clicked().connect( SLOT(dialogWarn_, WDialog::accept) );

  if( deleteBoardModel( board_id ) == 0 ){ 			// Delete the mainboard from database.
	dialogWarn_->setWindowTitle("Mainboard");
	txtWarn->setText("Mainboard Configuration deleted successfully!");
	dialogWarn_->show();
  }
  else {											// Failed to delete the mainboard.
  	dialogWarn_->setWindowTitle("Mainboard");
  	txtWarn->setText("There was a FAILURE during deletion of Mainboard!");
  	dialogWarn_->show();
  }

  refreshPage();				// Reload the page to show the changes.
}



/*
  Retrieve all OS Distributions.
*/
//! \relates database.cc
std::multimap<std::string,std::string> getOsList();


void MainboardsWidget::fillOsModel()
{
  std::multimap<std::string,std::string> osList;
  
  osList = getOsList();					// Get list of all OS Distributions.

  std::vector<WStandardItem *> result; 		// Line to be added to osModel_. 
  WStandardItem *item;

  item = new WStandardItem("0");
  result.push_back(item);
  item = new WStandardItem("Select OS");
  result.push_back(item);
  osModel_->appendRow(result);
  result.clear();

  // Iterate through the list of retrieved Distributions, while getting their IDs and Names.
  std::multimap<std::string,std::string>::iterator os_iter;
  for ( os_iter=osList.begin(); os_iter!=osList.end(); ++os_iter ){
	item = new WStandardItem( os_iter->first ); 	// osID column
	result.push_back(item);
	item = new WStandardItem( os_iter->second ); 	// osName column
	result.push_back(item);

	osModel_->appendRow(result);
	result.clear();
  }
  
}



/*
  Retrieve Releases of provided OS Distribution.
*/
//! \relates database.cc
std::multimap<std::string,std::string> queryOsReleases( std::string os_id );


void MainboardsWidget::fillReleaseModel( std::string os_id )
{
  releaseModel_->removeRows(0, releaseModel_->rowCount()); 	// reset model..

  std::multimap<std::string,std::string> releaseList;  
  
  releaseList = queryOsReleases( os_id ); 	// Get Release list of given Distribution ( releaseID, releaseName ).
  
  std::vector<WStandardItem *> result; 		// Line to be added to releaseModel_.
  WStandardItem *item;

  item = new WStandardItem("0");
  result.push_back(item);
  item = new WStandardItem("Select Release");
  result.push_back(item);
  releaseModel_->appendRow(result);
  result.clear();

  // Iterate through the list of retrieved Releases, while getting their IDs and Names.
  std::multimap<std::string,std::string>::iterator release_iter;
  for ( release_iter=releaseList.begin(); release_iter!=releaseList.end(); ++release_iter ){
	item = new WStandardItem( release_iter->first ); 	// releaseID column
	result.push_back(item);
	item = new WStandardItem( release_iter->second ); 	// releaseName column
	result.push_back(item);

	releaseModel_->appendRow(result);
	result.clear();
  }
}



/*
  Retrieve Kernels of provided Distribution Release.
*/
//! \relates database.cc
std::multimap<std::string,std::string> queryReleaseKernels( std::string release_id );


void MainboardsWidget::fillKernelModel( std::string release_id )
{
  kernelModel_->removeRows(0, kernelModel_->rowCount()); // reset model..

  std::multimap<std::string,std::string> kernelList;  

  // Get Kernel list of given Release (uKerneID,kernelVersion-machineHardware )
  kernelList = queryReleaseKernels( release_id ); 
  
  std::vector<WStandardItem *> result; 		// Line to be added to kernelModel_.
  WStandardItem *item;

  // Iterate through the list of retrieved Kernels, while getting their IDs and Version-Arch's.
  std::multimap<std::string,std::string>::iterator kernel_iter;
  for ( kernel_iter=kernelList.begin(); kernel_iter!=kernelList.end(); ++kernel_iter ){
	item = new WStandardItem( kernel_iter->first ); 	// uKernelID column
	result.push_back(item);
	item = new WStandardItem( kernel_iter->second ); 	// kernelVersion-machineHardware column
	result.push_back(item);

	kernelModel_->appendRow(result);
	result.clear();
  }
}


void MainboardsWidget::osSelectionChanged( int selected_index )
{
  std::stringstream ss;
  ss << selected_index;
  std::string index = ss.str();

  // Get ID of selected Distribution.
  WModelIndex i1 = osModel_->index(comboOS_->currentIndex(),0 );
  std::string os_id = (osModel_->itemFromIndex(i1))->text().narrow();

  if( os_id != "0" ){			// if a Distribution is selected
	comboRel_->enable();
	fillReleaseModel( os_id );	// Fill Releases of the selected Distribution.
	comboRel_->setCurrentIndex(0);
  }
  else{							// else 'Select OS' is selected..
	comboRel_->disable();
  }
  comboKer_->disable();
  comboKer_->setCurrentIndex(0);
  bCheckOs_->disable();
}



void MainboardsWidget::releaseSelectionChanged( int selected_index )
{
  std::stringstream ss;
  ss << selected_index;
  std::string index = ss.str();

  // Get ID of selected Release.
  WModelIndex i1 = releaseModel_->index(comboRel_->currentIndex(),0 );
  std::string release_id = (releaseModel_->itemFromIndex(i1))->text().narrow();

  if( release_id != "0" ){		// if a Release is selected
	comboKer_->enable();
	fillKernelModel( release_id ); // Fill Kernels of the selected Distribution Release.
	comboKer_->setCurrentIndex(0);
	bCheckOs_->enable();
  }
  else{							// else 'Select Release' is selected.
	comboKer_->disable();
	bCheckOs_->disable();
  }
}


std::string MainboardsWidget::getOsIdentifier()
{
  // Get the Unique Kernel ID, representing an OS(Distribution-Release-Kernel) ID.
  WModelIndex i1 = kernelModel_->index(comboKer_->currentIndex(),0 );
  std::string uKernel_id = (kernelModel_->itemFromIndex(i1))->text().narrow();

  if( uKernel_id == "0" )
	return "";
  return uKernel_id;
}


std::string MainboardsWidget::getBoardIdentifier()
{
  // Column 0 of the source model holds the IDs for mainboards so retrieve it.
  WModelIndex i1 = boardsProxyModel_->index(boardsSelection_->currentIndex(),0 );
  WModelIndex i2 = boardsProxyModel_->mapToSource( i1 );

  return ( (mainboards_->itemFromIndex(i2))->text().narrow() );
}



void MainboardsWidget::bCheckOs_Click()
{
  std::string board_id = getBoardIdentifier(); 		// Get ID of the selected mainboard.

  // Retrieve PCI devices of the mainboard.
  std::vector<PciDevice> device_list = getBoardDeviceList( board_id );

  PcimapResultWidget* pcilist;
  if ( (pcilist = PcimapResultWidget::Instance()) == NULL ){

	// Switch to PcimapResultWidget providing mainboard name and OS ID too..
	selectWidget( PcimapResultWidget::Instance( device_list, parent_,
												boardsSelection_->currentText().narrow(),
												getOsIdentifier()) );
	
	// Remove and delete the current instance of MainboardsWidget.
	resetAll();
	
	WApplication::instance()->setInternalPath("/query_results"); // Set URL to point the new page.
  }
  else
	wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
}



void MainboardsWidget::dialogWarn_Close( WDialog::DialogCode code )
{
  delete dialogWarn_;

  if ( code == WDialog::Accepted ){ 	// User confirmed deletion of mainboard.
	deleteBoard();
  }

}

void MainboardsWidget::refreshPage()
{
  resetAll();
  selectWidget( MainboardsWidget::Instance( parent_ ) );
}

//! \relates harixApp.cc
void removeWidget( WContainerWidget* widget );

void MainboardsWidget::resetAll()
{
  removeWidget( MainboardsWidget::Instance() );
  delete MainboardsWidget::Instance();
}
