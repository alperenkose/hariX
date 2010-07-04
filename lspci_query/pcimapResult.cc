#include <Wt/WTextArea>
#include <Wt/WPanel>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTableView>
#include <Wt/WLogger>
#include <Wt/WTable>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <string>

#include "../home.hpp"
#include "../harixApp.hpp"			// JUST FOR wApp!!
#include "pcimapResult.hpp"
#include "pcimapQuery.hpp"
#include "../pci_device.hpp"

using namespace Wt;
using std::string;

void selectWidget( WContainerWidget* widget );
void removeWidget( WContainerWidget* widget );

PcimapResultWidget* PcimapResultWidget::instance_ = NULL;
int PcimapResultWidget::instance_count = 0;
PcimapResultWidget::PcimapResultWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  ++instance_count;

  lspci_list = PcimapQueryWidget::getLspciList();

  lspciModel_ = new WStandardItemModel( 0, 3, this );
  lspciModel_->setHeaderData(0, Horizontal, string("Class"));
  lspciModel_->setHeaderData(1, Horizontal, string("Vendor"));
  lspciModel_->setHeaderData(2, Horizontal, string("Device"));

  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list.begin(); lspci_iter != lspci_list.end(); ++lspci_iter ){
	lspciModel_->appendRow( pciDeviceToRowItem( &(*lspci_iter) ) );
  }

  panelTable_ = new WPanel(this);
  panelTable_->resize(1000,300);
  panelTable_->setCentralWidget( lspciTable_ = new WTableView() );
  // lspciTable_->setAlternatingRowColors(true); // @TODO: Requires CSS - Modify CSS

  lspciTable_->setColumnWidth(0, WLength(240));
  lspciTable_->setColumnWidth(1, WLength(200));
  lspciTable_->setColumnWidth(2, WLength(500));
  lspciTable_->setSortingEnabled(false);
  lspciTable_->setSelectionMode(SingleSelection);
  lspciTable_->setModel(lspciModel_);

  lspciTable_->doubleClicked().connect(this, &PcimapResultWidget::lspciTableRowSelected);

  addWidget( new WBreak() );

  
  /**
   * osSupportModel_
   * 
   */
  osSupportModel_ = new WStandardItemModel( 0, 5, this );
  osSupportModel_->setHeaderData(0, Horizontal, string("OS"));
  osSupportModel_->setHeaderData(1, Horizontal, string("Release"));
  osSupportModel_->setHeaderData(2, Horizontal, string("Kernel"));
  osSupportModel_->setHeaderData(3, Horizontal, string("Arch."));
  osSupportModel_->setHeaderData(4, Horizontal, string("Driver"));

  panelSupport_ = new WPanel(this);
  panelSupport_->resize(1000, WLength() );
  panelSupport_->setTitle("Device Details");


  panelSupport_->setCentralWidget( layoutSupport_ = new WTable() );
  layoutSupport_->rowAt(0)->setHeight( WLength(25) );
  layoutSupport_->elementAt(0,0)->addWidget( new WText("Vendor:") );
  // (WText Degiskeni)->setStyleClass("Wt-text"); // html class for CSS can be given this way..
  layoutSupport_->elementAt(0,1)->addWidget( editVendor_ = new WLineEdit() );
  layoutSupport_->elementAt(0,2)->addWidget( new WText("Device:") );
  layoutSupport_->elementAt(0,3)->addWidget( editDevice_ = new WLineEdit() );
  layoutSupport_->elementAt(0,4)->addWidget( new WText("Subvendor:") );
  layoutSupport_->elementAt(0,5)->addWidget( editSubvendor_ = new WLineEdit() );
  layoutSupport_->elementAt(0,6)->addWidget( new WText("Subdevice:") );
  layoutSupport_->elementAt(0,7)->addWidget( editSubdevice_ = new WLineEdit() );
  layoutSupport_->elementAt(0,8)->addWidget( new WText("Class:") );
  layoutSupport_->elementAt(0,9)->addWidget( editClass_ = new WLineEdit() );
  layoutSupport_->elementAt(0,10)->addWidget( new WText("Subclass:") );
  layoutSupport_->elementAt(0,11)->addWidget( editSubclass_ = new WLineEdit() );
  layoutSupport_->elementAt(0,12)->addWidget( new WText("Progif:") );
  layoutSupport_->elementAt(0,13)->addWidget( editProgif_ = new WLineEdit() );

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
  
  layoutSupport_->elementAt(1,0)->setColumnSpan(14);
  layoutSupport_->elementAt(1,0)->addWidget( new WText("Operating System Support Details:") );
  layoutSupport_->rowAt(1)->setHeight( WLength(25) );
  
  layoutSupport_->elementAt(2,0)->setColumnSpan(14);
  layoutSupport_->elementAt(2,0)->addWidget( osSupportTable_ = new WTableView() );
  layoutSupport_->rowAt(2)->setHeight( WLength(250) );

  WPushButton* bGoHome;
  layoutSupport_->elementAt(3,0)->setColumnSpan(14);
  layoutSupport_->elementAt(3,0)->setContentAlignment(AlignRight);
  layoutSupport_->elementAt(3,0)->addWidget( bGoHome = new WPushButton("Go Home") );
  bGoHome->clicked().connect(this, &PcimapResultWidget::bGoHome_Click);

  osSupportTable_->setModel(osSupportModel_);
  osSupportTable_->resize(950,250);
  osSupportTable_->setColumnWidth(0, WLength(200));
  osSupportTable_->setColumnWidth(1, WLength(200));
  osSupportTable_->setColumnWidth(2, WLength(200));
  osSupportTable_->setColumnWidth(3, WLength(100));
  osSupportTable_->setColumnWidth(4, WLength(200));

}

PcimapResultWidget* PcimapResultWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	wApp->log("debug") << "parent is 0!"; 					// @test
	if ( !instance_ ){
	  wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new PcimapResultWidget(parent);
	  wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

PcimapResultWidget::~PcimapResultWidget()
{
  instance_ = NULL;
  instance_count = 0;
  lspci_list.clear();
}


std::vector<string>
queryClassName(string class_code,string subclass_code,string progif_code);

std::vector<string>
queryDeviceName(string vendor_code,string device_code,string subvendor_code,string subdevice_code);

std::vector<WStandardItem*>
PcimapResultWidget::pciDeviceToRowItem( const PciDevice* current_item )
{
  std::vector<string> full_class, full_device;
  
  std::vector<WStandardItem *> result;
  WStandardItem *item;

  full_class =  queryClassName(current_item->getClass(), 	// Returns Class,Subclass,Progif names..
							   current_item->getSubclass(),
							   current_item->getProgif());

  full_device = queryDeviceName(current_item->getVendor(), 	// Returns Vendor,Device,Subsys names..
								current_item->getDevice(),
								current_item->getSubvendor(),
								current_item->getSubdevice());
  // column 0:
  item = new WStandardItem( full_class.at(1) ); 		// Print just the Subclass name..
  result.push_back(item);

  // column 1: 
  item = new WStandardItem( full_device.at(0) ); 		// Print the Vendor name..
  result.push_back(item);
  
  // column 2: 
  item = new WStandardItem( full_device.at(1) ); 		// Print the Device name..
  result.push_back(item);
  
  return result;
}


std::multimap<string, string>
queryPcimapOsList(string vendor_code, string device_code, string subvendor_code, string subdevice_code,
				  string class_code, string subclass_code, string progif_code);

std::vector<string> queryOs( string unique_kernel_id );

string queryModuleName( string mod_name_id );

void PcimapResultWidget::lspciTableRowSelected(WModelIndex index, WMouseEvent event)
{
  PciDevice* selected_item;
  selected_item = ( &lspci_list.at(index.row()) );
  fillDeviceDetails( *selected_item );

  std::multimap<string,string> uKernel_mod_list; // (unique Kernel id, module name id)
  std::vector<string> os;
  std::vector< std::vector<string> > os_list;
  string module_name;

  uKernel_mod_list = queryPcimapOsList( selected_item->getVendor(), selected_item->getDevice(),
										selected_item->getSubvendor(),selected_item->getSubdevice(),
										selected_item->getClass(), selected_item->getSubclass(), selected_item->getProgif() );

  std::multimap<string,string>::iterator ukernel_iter;
  for ( ukernel_iter=uKernel_mod_list.begin(); ukernel_iter!=uKernel_mod_list.end(); ++ukernel_iter ){
  	os = queryOs( ukernel_iter->first );
  	module_name = queryModuleName( ukernel_iter->second );
  	os.push_back(module_name);

  	os_list.push_back(os);
  }

  osSupportModel_->removeRows(0, osSupportModel_->rowCount());
  if( !os_list.empty() ){
	std::vector< std::vector<string> >::iterator os_iter;
	for( os_iter=os_list.begin(); os_iter!=os_list.end(); ++os_iter )
	  osSupportModel_->appendRow( osToSupportRow( *os_iter ) );
  }
  else{
	wApp->log("debug") << "NOOOOOO OS'ES!"; 					// @test
  }

}

std::vector<WStandardItem*>
PcimapResultWidget::osToSupportRow( const std::vector<string>& os )
{
  std::vector<WStandardItem *> result;
  WStandardItem *item;
  if( !os.empty() ){
	// column 0:
	item = new WStandardItem( os.at(0) ); 		// 
	result.push_back(item);

	// column 1: 
	item = new WStandardItem( os.at(1) ); 		// 
	result.push_back(item);
  
	// column 2: 
	item = new WStandardItem( os.at(2) ); 		// 
	result.push_back(item);

	// column 3:
	item = new WStandardItem( os.at(3) ); 		// 
	result.push_back(item);

	// column 4:
	item = new WStandardItem( os.at(4) ); 		// 
	result.push_back(item);

  }

  return result;
}

void PcimapResultWidget::fillDeviceDetails( const PciDevice& selected_device )
{
  editVendor_->setText( selected_device.getVendor() );
  editDevice_->setText( selected_device.getDevice() );
  editSubvendor_->setText( selected_device.getSubvendor() );
  editSubdevice_->setText( selected_device.getSubdevice() );
  editClass_->setText( selected_device.getClass() );
  editSubclass_->setText( selected_device.getSubclass() );
  editProgif_->setText( selected_device.getProgif() );
}

void PcimapResultWidget::bGoHome_Click()
{
  HomeWidget* home_page;
  home_page = HomeWidget::Instance();
  selectWidget( home_page );
  // remove widget
  removeWidget( PcimapResultWidget::Instance() );
  delete PcimapResultWidget::Instance();
}
