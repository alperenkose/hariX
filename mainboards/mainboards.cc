#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WSortFilterProxyModel>
#include <Wt/WSelectionBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WLogger>

#include "../harixApp.hpp"		// just for wApp
#include "../home.hpp"
#include "../pci_device.hpp"
#include "../lspci_query/pcimapResult.hpp"
#include "mainboards.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

MainboardsWidget* MainboardsWidget::instance_ = NULL;

MainboardsWidget::MainboardsWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;
  panelMainboard_ = new WPanel(this);
  panelMainboard_->resize(420, WLength());
  WTable* layoutMainboard;
  panelMainboard_->setCentralWidget( layoutMainboard = new WTable() );

  // WStandardItemModel* mainboards = new WStandardItemModel(this);
  mainboards = new WStandardItemModel(this);
  fillMainboards( *mainboards );

  // WSortFilterProxyModel *boardsProxyModel = new WSortFilterProxyModel(this);
  boardsProxyModel = new WSortFilterProxyModel(this);
  boardsProxyModel->setSourceModel(mainboards);
  // boardsProxyModel->setDynamicSortFilter(true);
  boardsProxyModel->sort(1);

  layoutMainboard->columnAt(0)->setWidth( WLength(160) );
  layoutMainboard->elementAt(0,0)->setColumnSpan(2);
  layoutMainboard->elementAt(0,0)->addWidget( new WText("Mainboard Configuration:") );
  
  boardsSelection_ = new WSelectionBox(layoutMainboard->elementAt(1,0));
  boardsSelection_->setModel(boardsProxyModel);
  boardsSelection_->setModelColumn(1);
  boardsSelection_->resize( 150, WLength() );
  boardsSelection_->setVerticalSize(10);

  // layoutMainboard->rowAt(2)->setHeight(20);
  // layoutMainboard->elementAt(1,0)->setColumnSpan(2);

  WTable *layConfButtons;
  layoutMainboard->elementAt(2,0)->addWidget( layConfButtons = new WTable() );
  layConfButtons->columnAt(0)->setWidth( WLength(80) );
  layConfButtons->columnAt(1)->setWidth( WLength(70) );
  
  WPushButton* bBoardDevices;
  layConfButtons->elementAt(0,0)->addWidget( bBoardDevices = new WPushButton("Show") );
  bBoardDevices->clicked().connect(this, &MainboardsWidget::bBoardDevices_Click);

  // layoutMainboard->elementAt(2,0)->addWidget( new WText("Selected uKernelID: ") );
  // layoutMainboard->elementAt(2,0)->addWidget( testText_ = new WText("none") );

  WPushButton *bDeleteBoard;
  layConfButtons->elementAt(0,1)->setContentAlignment(AlignRight);
  layConfButtons->elementAt(0,1)->addWidget( bDeleteBoard = new WPushButton("Delete") );
  bDeleteBoard->clicked().connect(this, &MainboardsWidget::bDeleteBoard_Click);

  layoutMainboard->columnAt(1)->setWidth( WLength(80) ); // @TODO..
  
  layoutMainboard->columnAt(2)->setWidth( WLength(180) );
  layoutMainboard->elementAt(0,2)->addWidget( new WText("OS specific support?") );
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboOS_ = new WComboBox() );
  comboOS_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboDist_ = new WComboBox() );
  comboDist_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( comboKer_ = new WComboBox() );
  comboKer_->resize(150,20);
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( new WBreak() );
  layoutMainboard->elementAt(1,2)->addWidget( bCheckOs_ = new WPushButton("Check!") );
  bCheckOs_->clicked().connect(this, &MainboardsWidget::bCheckOs_Click);
  bCheckOs_->disable();

  osModel_ = new WStandardItemModel(this);
  fillOsModel();
  comboOS_->setModel(osModel_);
  comboOS_->setModelColumn(1);

  releaseModel_ = new WStandardItemModel(this);
  comboDist_->setModel( releaseModel_ );
  comboDist_->setModelColumn(1);

  kernelModel_ = new WStandardItemModel(this);
  comboKer_->setModel( kernelModel_ );
  comboKer_->setModelColumn(1);	// show kernelVersion and architecture together; 2.6.32 - i386

  comboOS_->activated().connect( this, &MainboardsWidget::osSelectionChanged );
  comboDist_->disable();
  comboKer_->disable();
  comboDist_->activated().connect( this, &MainboardsWidget::releaseSelectionChanged );

  layoutMainboard->elementAt(2,2)->addWidget( bGoHome_ = new WPushButton("Go Home"));
  layoutMainboard->elementAt(2,2)->setContentAlignment(AlignRight);
  bGoHome_->clicked().connect(this, &MainboardsWidget::bGoHome_Click);

}

MainboardsWidget* MainboardsWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new MainboardsWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

MainboardsWidget::~MainboardsWidget()
{
  instance_ = NULL;
  
}

std::multimap<std::string, std::string> getBoardList();

void MainboardsWidget::fillMainboards( WStandardItemModel& boardsModel )
{
  // std::multimap<std::string, std::string> mainboard_list; // ( boardID, boardName )

  mainboard_list = getBoardList();

  std::vector<WStandardItem *> result;
  WStandardItem *item;

  std::multimap<std::string,std::string>::iterator board_iter;
  for ( board_iter=mainboard_list.begin(); board_iter!=mainboard_list.end(); ++board_iter ){
	item = new WStandardItem( board_iter->first ); // boardID column
	result.push_back(item);
	item = new WStandardItem( board_iter->second ); // boardName column
	result.push_back(item);
	// wApp->log("debug") << "BOARD ID:" << board_iter->first; // @test
	// wApp->log("debug") << "BOARD Name:" << board_iter->second; // @test


	boardsModel.appendRow(result);
	result.clear();
  }
}


void MainboardsWidget::bBoardDevices_Click()
{
  std::string board_id = getBoardIdentifier();
  
  showBoardConfiguration( board_id );
}



// return uDevID list of devices on given board
std::vector<std::string> queryBoardDeviceList( std::string board_id );
// return vendor,subvendor,progif,class ... codes IF PCI device..
std::vector<std::string> queryDeviceCodes( std::string udev_id, int bus_type = 1 );

void MainboardsWidget::showBoardConfiguration( std::string board_id  )
{
  std::vector<PciDevice> device_list = getBoardDeviceList( board_id );

  PcimapResultWidget* pcilist;
  if ( (pcilist = PcimapResultWidget::Instance()) == NULL ){
	selectWidget( PcimapResultWidget::Instance(device_list,parent_, boardsSelection_->currentText().narrow()) );
	removeWidget( MainboardsWidget::Instance() );
	delete MainboardsWidget::Instance();	
  }
  else
	wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
}



std::vector<PciDevice> MainboardsWidget::getBoardDeviceList( std::string board_id  )
{
  std::vector<PciDevice> device_list;
  
  std::vector<std::string> udev_id_list;
  udev_id_list = queryBoardDeviceList( board_id ); // udev list

  std::vector<std::string> device_codes;
  // 0-vendor; 1-device; 2-subvendor; 3-subdevice; 4-class; 5-subclass; 6-progif

  PciDevice *pci_device;
  std::vector<std::string>::iterator udev_iter;
  for( udev_iter = udev_id_list.begin(); udev_iter != udev_id_list.end(); ++udev_iter ){
	device_codes = queryDeviceCodes( *udev_iter );

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

int deleteBoardModel( std::string board_id ); 

void MainboardsWidget::bDeleteBoard_Click()
{
  dialogWarn_ = new WDialog();
  dialogWarn_->setWindowTitle("Mainboard");
  new WText("Sure you want to delete the selected Mainboard?", dialogWarn_->contents());
  new WBreak( dialogWarn_->contents() );
  WPushButton *dialogWarnYes = new WPushButton("Yes", dialogWarn_->contents());
  dialogWarnYes->clicked().connect( SLOT(dialogWarn_, WDialog::accept) );
  WPushButton *dialogWarnNo = new WPushButton("No", dialogWarn_->contents());
  dialogWarnNo->clicked().connect( SLOT(dialogWarn_, WDialog::reject) );

  dialogWarn_->show();
	
  dialogWarn_->finished().connect(this, &MainboardsWidget::dialogWarn_Close);
}

void MainboardsWidget::deleteBoard()
{
  std::string board_id = getBoardIdentifier();
  
  dialogWarn_ = new WDialog();
  
  WText *txtWarn = new WText(dialogWarn_->contents());
  new WBreak( dialogWarn_->contents() );
  WPushButton *dialogWarnClose = new WPushButton("Close", dialogWarn_->contents());

  dialogWarnClose->clicked().connect( SLOT(dialogWarn_, WDialog::accept) );

  if( deleteBoardModel( board_id ) == 0 ){ // function at database.cc
	dialogWarn_->setWindowTitle("Mainboard");
	txtWarn->setText("Mainboard Configuration deleted successfully!");
	dialogWarn_->show();
  }
  else {
  	dialogWarn_->setWindowTitle("Mainboard");
  	txtWarn->setText("There was a FAILURE during deletion of Mainboard!");
  	dialogWarn_->show();
  }

  refreshPage();
}

std::multimap<std::string,std::string> getOsList();

void MainboardsWidget::fillOsModel()
{
  osList_ = getOsList();

  std::vector<WStandardItem *> result;
  WStandardItem *item;

  item = new WStandardItem("0");
  result.push_back(item);
  item = new WStandardItem("Select OS");
  result.push_back(item);
  osModel_->appendRow(result);
  result.clear();

  std::multimap<std::string,std::string>::iterator os_iter;
  for ( os_iter=osList_.begin(); os_iter!=osList_.end(); ++os_iter ){
	item = new WStandardItem( os_iter->first ); // osID column
	result.push_back(item);
	item = new WStandardItem( os_iter->second ); // osName column
	result.push_back(item);
	// wApp->log("debug") << "OS ID:" << os_iter->first; // @test
	// wApp->log("debug") << "OS Name:" << os_iter->second; // @test

	osModel_->appendRow(result);
	result.clear();
  }
  
}

std::multimap<std::string,std::string> queryOsReleases( std::string os_id );

void MainboardsWidget::fillReleaseModel( std::string os_id )
{
  releaseModel_->removeRows(0, releaseModel_->rowCount()); // reset model..
  
  releaseList_ = queryOsReleases( os_id ); // multimap( osID, osName )
  
  std::vector<WStandardItem *> result;
  WStandardItem *item;

  item = new WStandardItem("0");
  result.push_back(item);
  item = new WStandardItem("Select Release");
  result.push_back(item);
  releaseModel_->appendRow(result);
  result.clear();

  std::multimap<std::string,std::string>::iterator release_iter;
  for ( release_iter=releaseList_.begin(); release_iter!=releaseList_.end(); ++release_iter ){
	item = new WStandardItem( release_iter->first ); // releaseID column
	result.push_back(item);
	item = new WStandardItem( release_iter->second ); // releaseName column
	result.push_back(item);
	// wApp->log("debug") << "releaseID:" << release_iter->first; // @test
	// wApp->log("debug") << "releaseName:" << release_iter->second; // @test


	releaseModel_->appendRow(result);
	result.clear();
  }
}

std::multimap<std::string,std::string> queryReleaseKernels( std::string release_id );

void MainboardsWidget::fillKernelModel( std::string release_id )
{
  kernelModel_->removeRows(0, kernelModel_->rowCount()); // reset model..
  
  kernelList_ = queryReleaseKernels( release_id ); // multimap( uKerneID, kernelVersion-machineHardware )
  
  std::vector<WStandardItem *> result;
  WStandardItem *item;

  // item = new WStandardItem("0");
  // result.push_back(item);
  // item = new WStandardItem("Select Kernel");
  // result.push_back(item);
  // kernelModel_->appendRow(result);
  // result.clear();

  std::multimap<std::string,std::string>::iterator kernel_iter;
  for ( kernel_iter=kernelList_.begin(); kernel_iter!=kernelList_.end(); ++kernel_iter ){
	item = new WStandardItem( kernel_iter->first ); // uKernelID column
	result.push_back(item);
	item = new WStandardItem( kernel_iter->second ); // kernelVersion-machineHardware column
	result.push_back(item);
	// wApp->log("debug") << "uKernel ID:" << kernel_iter->first; // @test
	// wApp->log("debug") << "kernelVersion:" << kernel_iter->second; // @test


	kernelModel_->appendRow(result);
	result.clear();
  }
}

void MainboardsWidget::osSelectionChanged( int selected_index )
{
  std::stringstream ss;
  ss << selected_index;
  std::string index = ss.str();

  WModelIndex i1 = osModel_->index(comboOS_->currentIndex(),0 );
  std::string os_id = (osModel_->itemFromIndex(i1))->text().narrow();

  if( os_id != "0" ){
	comboDist_->enable();
	fillReleaseModel( os_id );
	comboDist_->setCurrentIndex(0);
  }
  else{
	comboDist_->disable();
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

  WModelIndex i1 = releaseModel_->index(comboDist_->currentIndex(),0 );
  std::string release_id = (releaseModel_->itemFromIndex(i1))->text().narrow();

  if( release_id != "0" ){
	comboKer_->enable();
	fillKernelModel( release_id );
	comboKer_->setCurrentIndex(0);
	bCheckOs_->enable();
  }
  else{
	comboKer_->disable();
	bCheckOs_->disable();
  }
}


std::string MainboardsWidget::getOsIdentifier()
{
  WModelIndex i1 = kernelModel_->index(comboKer_->currentIndex(),0 );
  std::string uKernel_id = (kernelModel_->itemFromIndex(i1))->text().narrow();

  if( uKernel_id == "0" )
	return "";
  return uKernel_id;
}

std::string MainboardsWidget::getBoardIdentifier()
{
  WModelIndex i1 = boardsProxyModel->index(boardsSelection_->currentIndex(),0 );
  WModelIndex i2 = boardsProxyModel->mapToSource( i1 );

  return ( (mainboards->itemFromIndex(i2))->text().narrow() ); 
}

void MainboardsWidget::bCheckOs_Click()
{
  // testText_->setText( getOsIdentifier() ); // @test
  std::string board_id = getBoardIdentifier();
  std::vector<PciDevice> device_list = getBoardDeviceList( board_id );

  PcimapResultWidget* pcilist;
  if ( (pcilist = PcimapResultWidget::Instance()) == NULL ){
	selectWidget( PcimapResultWidget::Instance( device_list, parent_,
											   boardsSelection_->currentText().narrow(),
											   getOsIdentifier()) );
	removeWidget( MainboardsWidget::Instance() );
	delete MainboardsWidget::Instance();	
  }
  else
	wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
}

void MainboardsWidget::dialogWarn_Close( WDialog::DialogCode code )
{
  delete dialogWarn_;

  if ( code == WDialog::Accepted ){
	deleteBoard();
  }

}

void MainboardsWidget::refreshPage()
{
  delete dialogWarn_;

  removeWidget( MainboardsWidget::Instance() );
  delete MainboardsWidget::Instance();
  selectWidget( MainboardsWidget::Instance( parent_ ) );
}

void removeWidget( WContainerWidget* widget );

void MainboardsWidget::bGoHome_Click()
{
  HomeWidget* home_page;
  home_page = HomeWidget::Instance();
  selectWidget( home_page );
  // remove widget
  removeWidget( MainboardsWidget::Instance() );
  delete MainboardsWidget::Instance();
}
