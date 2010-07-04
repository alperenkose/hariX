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

  // layoutMainboard->elementAt(0,0)
  boardsSelection_ = new WSelectionBox(layoutMainboard->elementAt(0,0));
  boardsSelection_->setModel(boardsProxyModel);
  boardsSelection_->setModelColumn(1);
  boardsSelection_->resize( 150, WLength() );
  boardsSelection_->setVerticalSize(10);

  layoutMainboard->elementAt(0,1)->addWidget( new WText("Selected boardID: ") );
  layoutMainboard->elementAt(0,1)->addWidget( testText_ = new WText("test") );

  WPushButton* bBoardDevices;
  layoutMainboard->elementAt(1,0)->addWidget( bBoardDevices = new WPushButton("Show Configuration") );
  bBoardDevices->clicked().connect(this, &MainboardsWidget::bBoardDevices_Click);

  layoutMainboard->elementAt(1,1)->addWidget( bGoHome_ = new WPushButton("Go Home"));
  layoutMainboard->elementAt(1,1)->setContentAlignment(AlignRight);
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

std::multimap<std::string, std::string> queryBoardList();

void MainboardsWidget::fillMainboards( WStandardItemModel& boardsModel )
{
  // std::multimap<std::string, std::string> mainboard_list; // ( boardID, boardName )

  mainboard_list = queryBoardList();

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
  // @TODO: check if nothing is selected..
  // std::stringstream ss;
  // ss << boardsSelection_->currentIndex();
  // std::string index = ss.str();


  WModelIndex i1 = boardsProxyModel->index(boardsSelection_->currentIndex(),0 );
  WModelIndex i2 = boardsProxyModel->mapToSource( i1 );
  // WStadardItem *line = mainboards->itemFromIndex( i2 ); line->text();
  
  // WStandardItem::text()-> WString.narrow()
  std::string board_id = (mainboards->itemFromIndex(i2))->text().narrow(); 
  testText_->setText( board_id );
  
  showBoardConfiguration( board_id );
}

// return uDevID list of devices on given board
std::vector<std::string> queryBoardDeviceList( std::string board_id );
// return vendor,subvendor,progif,class ... codes IF PCI device..
std::vector<std::string> queryDeviceCodes( std::string udev_id, int bus_type = 1 );

void MainboardsWidget::showBoardConfiguration( std::string board_id  )
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

  PcimapResultWidget* pcilist;
  if ( (pcilist = PcimapResultWidget::Instance()) == NULL )
	selectWidget( PcimapResultWidget::Instance(device_list, boardsSelection_->currentText().narrow(),parent_) );
  else
	wApp->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";
}

void selectWidget( WContainerWidget* widget );
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
