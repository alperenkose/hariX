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
#include <Wt/WAnchor>
#include <string>

#include "../home.hpp"
#include "../harixApp.hpp"			// JUST FOR wApp!!
#include "pcimapResult.hpp"
#include "pcimapQuery.hpp"
#include "../pci_device.hpp"
#include "../div.hpp"

using namespace Wt;
using std::string;

void selectWidget( WContainerWidget* widget );
void removeWidget( WContainerWidget* widget );

std::vector<string> queryOs( string unique_kernel_id );

static const char *supportIcons[] = {
  "resources/button_cancel_16x16.png",
  "resources/button_ok_16x16.png"
};

PcimapResultWidget* PcimapResultWidget::instance_ = NULL;
int PcimapResultWidget::instance_count = 0;
PcimapResultWidget::PcimapResultWidget( std::vector<PciDevice>& lspci_list,
										WContainerWidget* parent,
										string board_name,
										string ukernel_id) : WContainerWidget(parent)
{
  ++instance_count;
  parent_ = parent;

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

  Div *page = new Div(result, "pagequeryresult");

  uKernelId_ = ukernel_id;
  // lspci_list_ = PcimapQueryWidget::getLspciList();
  lspci_list_ = lspci_list;

  // panelTable_ = new WPanel(this);
  page->addWidget( panelTable_ = new WPanel() );
  panelTable_->resize(1000,300);
  panelTable_->setCentralWidget( lspciTable_ = new WTableView() );


  lspciTable_->setSortingEnabled(false);
  lspciTable_->setSelectionMode(SingleSelection);
  lspciTable_->setAlternatingRowColors(true);

  if( ukernel_id == "" ){
	lspciModel_ = new WStandardItemModel( 0, 3, this );
	lspciModel_->setHeaderData(0, Horizontal, string("Class"));
	lspciModel_->setHeaderData(1, Horizontal, string("Vendor"));
	lspciModel_->setHeaderData(2, Horizontal, string("Device"));
	lspciTable_->setColumnWidth(0, WLength(240));
	lspciTable_->setColumnWidth(1, WLength(200));
	lspciTable_->setColumnWidth(2, WLength(500));
	if( board_name != "" ){
	  panelTable_->setTitle("Configuration of Mainboard: " + board_name );
	}
  }
  else {
	lspciModel_ = new WStandardItemModel( 0, 4, this );
	lspciModel_->setHeaderData(0, Horizontal, string("Class"));
	lspciModel_->setHeaderData(1, Horizontal, string("Vendor"));
	lspciModel_->setHeaderData(2, Horizontal, string("Device"));
	lspciModel_->setHeaderData(3, Horizontal, string("Support"));
	lspciTable_->setColumnWidth(0, WLength(200));
	lspciTable_->setColumnWidth(1, WLength(180));
	lspciTable_->setColumnWidth(2, WLength(500));
	lspciTable_->setColumnWidth(3, WLength(60));


	std::vector<string> current_os = queryOs( uKernelId_ );
	panelTable_->setTitle("OS: "+ current_os.at(0) + " " + current_os.at(1) + " " +
						  current_os.at(2) + "-" + current_os.at(3) +
						  " support for Mainboard: " + board_name + "" );
  }

  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list_.begin(); lspci_iter != lspci_list_.end(); ++lspci_iter ){
	lspciModel_->appendRow( pciDeviceToRowItem( &(*lspci_iter) ) );
  }

  lspciTable_->setModel(lspciModel_);

  lspciTable_->doubleClicked().connect(this, &PcimapResultWidget::lspciTableRowSelected);

  page->addWidget( new WBreak() );

  
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

  // panelSupport_ = new WPanel(this);
  page->addWidget( panelSupport_ = new WPanel() );
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


  // layoutSupport_->elementAt(3,0)->setColumnSpan(5);
  // layoutSupport_->elementAt(3,0)->setContentAlignment(AlignLeft);
  WPushButton *bSaveBoard;
  layoutSupport_->elementAt(3,0)->addWidget( bSaveBoard = new WPushButton("Save Mainboard!") );
  bSaveBoard->clicked().connect(this, &PcimapResultWidget::bSaveBoard_Click);
  
  // WPushButton* bGoHome;
  // layoutSupport_->elementAt(3,1)->setColumnSpan(13);
  // layoutSupport_->elementAt(3,1)->setContentAlignment(AlignRight);
  // layoutSupport_->elementAt(3,1)->addWidget( bGoHome = new WPushButton("Go Home") );
  // bGoHome->clicked().connect(this, &PcimapResultWidget::bGoHome_Click);

  osSupportTable_->setModel(osSupportModel_);
  osSupportTable_->resize(950,250);
  osSupportTable_->setColumnWidth(0, WLength(200));
  osSupportTable_->setColumnWidth(1, WLength(200));
  osSupportTable_->setColumnWidth(2, WLength(200));
  osSupportTable_->setColumnWidth(3, WLength(100));
  osSupportTable_->setColumnWidth(4, WLength(200));

  if( board_name != "" ){
	bSaveBoard->disable();
  }
  else {
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
  instance_ = new PcimapResultWidget(lspci_list, parent, board_name, ukernel_id);
  wApp->log("debug") << "Object initiated!!"; 			// @test

  wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;
  
}

PcimapResultWidget* PcimapResultWidget::Instance()
{
  if ( !instance_ ){
	wApp->log("debug") << "no instance return NULL!!"; 	// @test
	return NULL;
  }

  wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;
}

PcimapResultWidget::~PcimapResultWidget()
{
  instance_ = NULL;
  instance_count = 0;
  lspci_list_.clear();
}


std::vector<string>
queryClassName(string class_code,string subclass_code,string progif_code);

std::vector<string>
queryDeviceName(string vendor_code,string device_code,string subvendor_code,string subdevice_code);

std::multimap<string, string>
queryPcimapOsList(string vendor_code, string device_code, string subvendor_code, string subdevice_code,
				  string class_code, string subclass_code, string progif_code);

string queryPciVendorName( string vendor_code );

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
  if( !full_class.empty() ){
	item = new WStandardItem( full_class.at(1) ); 		// Print just the Subclass name..
  }
  else
	item = new WStandardItem("Unknown Class"); 		// class-subclass pair doesnot exist in db!

  result.push_back(item);

  if( !full_device.empty() ){
	// column 1: 
	item = new WStandardItem( full_device.at(0) ); 		// Print the Vendor name..
	result.push_back(item);

	if( full_device.at(1) != "" )
	  // column 2: 
	  item = new WStandardItem( full_device.at(1) ); 		// Print the Device name..
	else
	  item = new WStandardItem( "Unknown Device" );
	result.push_back(item);	  

  }
  else{
	string vendor_name;
	if( (vendor_name = queryPciVendorName(current_item->getVendor())) == "" )
	  item = new WStandardItem( "Unknown Vendor" );
	else
	  item = new WStandardItem( vendor_name );
	result.push_back(item);
  	item = new WStandardItem( "Unknown Device" );
	result.push_back(item);
  }

  if( uKernelId_ != "" ){
	item = new WStandardItem();

	std::multimap<string,string> uKernel_mod_list; // (unique Kernel id, module name id)
	uKernel_mod_list = queryPcimapOsList(current_item->getVendor(), current_item->getDevice(),
										 current_item->getSubvendor(),current_item->getSubdevice(),
										 current_item->getClass(),current_item->getSubclass(),current_item->getProgif());

	std::multimap<string,string>::iterator ukernel_mod_iter;
	ukernel_mod_iter = uKernel_mod_list.find( uKernelId_ );
	if( ukernel_mod_iter == uKernel_mod_list.end() ){ // NO module found for the Device with given Kernel!!
	  item->setIcon( supportIcons[0] );
	}
	else{						// Modules found for the device..
	  item->setIcon( supportIcons[1] );
	}
	result.push_back(item);	  
  }
  
  return result;
}




string queryModuleName( string mod_name_id );

void PcimapResultWidget::lspciTableRowSelected(WModelIndex index, WMouseEvent event)
{
  PciDevice* selected_item;
  selected_item = ( &lspci_list_.at(index.row()) );
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

void PcimapResultWidget::bSaveBoard_Click()
{
  dialogBoard_ = new WDialog("Save Mainboard");

  new WText("Store Mainboard configuration on Database! <br />Model Name: ", dialogBoard_->contents() );
  editBoard_ = new WLineEdit( dialogBoard_->contents() );
  new WBreak( dialogBoard_->contents() );
  WPushButton* dialogBoardOk = new WPushButton("Ok", dialogBoard_->contents());
  WPushButton* dialogBoardCancel = new WPushButton("Cancel", dialogBoard_->contents());

  (*dialogBoardOk).clicked().connect( SLOT(dialogBoard_, WDialog::accept) );
  (*dialogBoardCancel).clicked().connect( SLOT(dialogBoard_, WDialog::reject) );

  dialogBoard_->show();

  dialogBoard_->finished().connect(this, &PcimapResultWidget::dialogBoard_Close);
}


void PcimapResultWidget::dialogBoard_Close( WDialog::DialogCode dialog_code )
{

  if ( dialog_code == WDialog::Accepted ){
	// editVendor_->setText( editBoard_->text().narrow() ); // @test
	string board_name = editBoard_->text().narrow();
	if( board_name != "" ){
	  storeMainBoard( board_name );
	  delete dialogBoard_;
	}
	else
	  dialogBoard_->show();
  }
}

string queryBoardModelId( string board_name );
string insertBoardModel( string board_name );
void insertBoardDevices( string board_id, std::vector<string>& device_id_list );
static std::vector<PciDevice> lspci_list_backup;

void PcimapResultWidget::storeMainBoard( string board_name )
{
  std::vector<string> uDevIdList;
  string board_id = queryBoardModelId( board_name );
  if ( board_id  == "" ){
	board_id = insertBoardModel( board_name );

	uDevIdList = getUniqueDevIdList(); // uDevID list of lspci_list_..
	
	insertBoardDevices( board_id, uDevIdList );

	// @TODO: !!! ALSO INSERT INTO DEV_MOD.. something like: checkDeviceModules( uDevIdList );

	dialogWarn_->setWindowTitle("Mainboard");
	txtWarn_->setText("Mainboard Saved!");
	dialogWarn_->show();

	lspci_list_backup = lspci_list_;
	removeWidget( PcimapResultWidget::Instance() );
	delete PcimapResultWidget::Instance();
	selectWidget( PcimapResultWidget::Instance( lspci_list_backup, parent_, board_name) );
  }
  else{							// Board already exists!!
	dialogWarn_->setWindowTitle("Mainboard Exists");
	txtWarn_->setText("Mainboard with the given name already exists in database!");
	dialogWarn_->show();
  }
  
}

string queryUniquePcisubId(string vendor_code, string device_code, string subvendor_code, string subdevice_code);
string queryUniqueProgifId(string class_code, string subclass_code, string progif_code);
string checkPciSpcId( const PciDevice* const currentPciDevice );
string checkUniqueDeviceId( string dev_special_id, int bus_type = 1 );
string insertUnknownPcisubId(string vendor_code,string device_code,string subvendor_code,string subdevice_code);
string insertUnknownProgifId(string class_code, string subclass_code, string progif_code);

std::vector<string> PcimapResultWidget::getUniqueDevIdList()
{
  std::vector<string> uDevIdList;
  string uPcisubId, uProgifId, pciSpcId, uDevId;

  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list_.begin(); lspci_iter != lspci_list_.end(); ++lspci_iter ){
  // 	uPcisubId = queryUniquePcisubId(lspci_iter->getVendor(), 	// Returns uPcisubID from pci_subsystems..
  // 									lspci_iter->getDevice(),
  // 									lspci_iter->getSubvendor(),
  // 									lspci_iter->getSubdevice());
  // 	if( uPcisubId == "" ){
  // 	  uPcisubId = insertUnknownPcisubId(lspci_iter->getVendor(), 	// Insert and Return uPcisubID
  // 										lspci_iter->getDevice(),
  // 										lspci_iter->getSubvendor(),
  // 										lspci_iter->getSubdevice());
  // 	}

  // 	uProgifId = queryUniqueProgifId(lspci_iter->getClass(), 	// Returns uProgifID from pci_prog_ifs.. 
  // 									lspci_iter->getSubclass(),
  // 									lspci_iter->getProgif());
  // 	if( uProgifId == "" ){
  // 	  // not sure what todo here, maybe better not to allow saving!!
  // 	  	// wApp->log("debug") << "Hmm class identifier not found!!!! " ; // @test
  // 	  	// uProgifId = insertUnknownProgifId(lspci_iter->getClass(), 	// insert and return uProgifID
  // 		// 								  lspci_iter->getSubclass(),
  // 		// 								  lspci_iter->getProgif());
  // 	}

  // pciSpcId = checkPciSpcId( uPcisubId, uProgifId ); // Returns pciSpcID from pci_all, adds if not exists..
	pciSpcId = checkPciSpcId( &*lspci_iter );
	uDevId = checkUniqueDeviceId( pciSpcId ); // Returns uDevID from all_devices, adds if not exists..
	uDevIdList.push_back( uDevId );
	wApp->log("debug") <<" !pciSpcId:"<< pciSpcId << " !uDevId:" << uDevId ; // @test

	
  }

  return uDevIdList;
}

// void PcimapResultWidget::bGoHome_Click()
// {
//   HomeWidget* home_page;
//   home_page = HomeWidget::Instance();
//   selectWidget( home_page );
//   // remove widget
//   removeWidget( PcimapResultWidget::Instance() );
//   delete PcimapResultWidget::Instance();
// }

void PcimapResultWidget::resetAll()
{
  removeWidget( PcimapResultWidget::Instance() );
  delete PcimapResultWidget::Instance();
}
