#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WInPlaceEdit>
#include <Wt/WLineEdit>
#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WOverlayLoadingIndicator>
#include "harixApp.hpp"
#include <Wt/WLogger>			// @test
#include <fstream>

#include "home.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "mainboards/mainboards.hpp"
#include "div.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

HomeWidget* HomeWidget::instance_ = NULL;

HomeWidget::HomeWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
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
  aHome->setStyleClass("selected");
  WAnchor *aAnalyzeOs;
  menulist->addWidget( aAnalyzeOs = new WAnchor("","Analyze OS") );
  aAnalyzeOs->setRefInternalPath("/analyze_os");
  WAnchor *aQueryDevices;
  menulist->addWidget( aQueryDevices = new WAnchor("","Query Devices") );
  aQueryDevices->setRefInternalPath("/query_devices");
  WAnchor *aMainboards;
  menulist->addWidget( aMainboards = new WAnchor("","Mainboards") );
  aMainboards->setRefInternalPath("/mainboards");

  Div *page = new Div(content, "pagearea");

  Div *footer = new Div(result, "footr");
  footer->addWidget( new WText("<p>HARdware Information management<br />"
							   "This site is best viewed in a Firefox browser!</p>") );


  page->addWidget( bPciIds_ = new WPushButton("Update PCI IDS") );
  bPciIds_->clicked().connect(this, &HomeWidget::bPciIds_Click);

    WPanel *panelSettings;
  WTable *layoutSettings;
  page->addWidget( panelSettings = new WPanel() );
  panelSettings->setTitle("Settings");
  panelSettings->setCollapsible(true);
  panelSettings->collapse();
  panelSettings->setCentralWidget( layoutSettings = new WTable() );

  layoutSettings->columnAt(0)->setWidth( WLength(100) );
  layoutSettings->rowAt(0)->setHeight( WLength(40) );
  layoutSettings->elementAt(0,0)->addWidget( new WText("<p>HTTP Proxy:</p>") );
  layoutSettings->elementAt(0,1)->addWidget( new WText("Proxy form -> http://[proxy_address]:[port]") );
  layoutSettings->elementAt(0,1)->addWidget( new WBreak() );
  layoutSettings->elementAt(0,1)->addWidget( editProxy_ = new WLineEdit() );
  editProxy_->resize( WLength(200), WLength() );
  WPushButton *bSaveProxy;
  layoutSettings->elementAt(0,1)->addWidget( bSaveProxy = new WPushButton("Save") );

  readProxyFile();
  editProxy_->setText( settingsProxy_ );
  editProxy_->setEmptyText("No Proxy Set!");
  bSaveProxy->clicked().connect(this, &HomeWidget::changeProxy);

}

HomeWidget* HomeWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new HomeWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}


static std::string pci_ids_file;

int update_pci_ids(std::string pci_ids_file);

void HomeWidget::bPciIds_Click()
{
  Wt::WApplication::instance()->setLoadingIndicator( loading_ = new Wt::WOverlayLoadingIndicator() );
  loading_->setMessage("Updating..");
  
  pci_ids_file = "/tmp/pci.ids";
  std::string download_command;
  if( settingsProxy_ != "" )
	download_command =
	  "curl http://pciids.sourceforge.net/v2.2/pci.ids --proxy "+ settingsProxy_ +" > /tmp/pci.ids";
  else
	download_command =
	  "curl http://pciids.sourceforge.net/v2.2/pci.ids > /tmp/pci.ids";
  
  if( system( download_command.c_str()  ) == 0 ){
	dialogDownload_ = new WDialog("PCI IDs Download");
	new WText("Downloaded pci.ids file! <br />"
			  "Now We will update the PCI IDs database", dialogDownload_->contents() );
	new WBreak( dialogDownload_->contents() );
	WPushButton* dialogDownloadOk = new WPushButton("Ok", dialogDownload_->contents() );
	dialogDownloadOk->clicked().connect( SLOT(dialogDownload_, WDialog::accept) );
	WPushButton* dialogDownloadCancel = new WPushButton("Cancel", dialogDownload_->contents() );
	dialogDownloadCancel->clicked().connect( SLOT(dialogDownload_, WDialog::reject) );
	dialogDownload_->show();
  }
  else {
	dialogDownload_ = new WDialog("PCI IDs Download");
	new WText("Failed to download pci.ids!!<br />"
			  "Check proxy settings..", dialogDownload_->contents() );
	new WBreak( dialogDownload_->contents() );
	WPushButton* dialogDownloadFail = new WPushButton("Close", dialogDownload_->contents() );
	dialogDownloadFail->clicked().connect( SLOT(dialogDownload_, WDialog::reject) );
	dialogDownload_->show();
  }

  dialogDownload_->finished().connect(this, &HomeWidget::dialogDownload_Close);

}


void HomeWidget::dialogDownload_Close( WDialog::DialogCode code )
{
  if ( code == WDialog::Accepted ){
	dialogPciIds_ = new WDialog("PCI IDs Update");
  
	if( update_pci_ids( pci_ids_file ) == 0 ){
	  new WText("PCI IDs succesfully updated!", dialogPciIds_->contents() );
	  new WBreak( dialogPciIds_->contents() );
	  WPushButton* dialogPciIdsOk = new WPushButton("Ok", dialogPciIds_->contents());

	  (*dialogPciIdsOk).clicked().connect( SLOT(dialogPciIds_, WDialog::accept) );

	  dialogPciIds_->show();
	}
	else{
	  new WText("FAILED to update PCI IDs!!", dialogPciIds_->contents() );
	  new WBreak( dialogPciIds_->contents() );
	  WPushButton* dialogPciIdsFail = new WPushButton("Close", dialogPciIds_->contents());

	  (*dialogPciIdsFail).clicked().connect( SLOT(dialogPciIds_, WDialog::reject) );
	
	  dialogPciIds_->show();
	}
	dialogPciIds_->finished().connect(this, &HomeWidget::dialogPci_Close);
  }

  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );

  delete dialogDownload_;
}


void HomeWidget::dialogPci_Close( WDialog::DialogCode code )
{
  delete dialogPciIds_;
}

void HomeWidget::changeProxy()
{
  settingsProxy_ = editProxy_->text().narrow();
  std::string save_command = "echo "+ settingsProxy_ +" > /tmp/proxy.harix";
  if( system( save_command.c_str()  ) != 0 ){
	dialogWarn_ = new WDialog("Proxy Configuration");
	new WText("Oops we failed to save file /tmp/proxy.harix!! <br /> "
			  "Proxy settings will be valid only for the current session!", dialogWarn_->contents() );
	new WBreak( dialogWarn_->contents() );
	WPushButton* dialogWarnFail = new WPushButton("Close", dialogWarn_->contents() );
	dialogWarnFail->clicked().connect( SLOT(dialogWarn_, WDialog::reject) );
	dialogWarn_->show();
  	dialogWarn_->finished().connect(this, &HomeWidget::dialogWarn_Close);
  }

}

void HomeWidget::dialogWarn_Close( Wt::WDialog::DialogCode code )
{
  delete dialogWarn_;
}

void HomeWidget::readProxyFile()
{
  std::ifstream proxy_file( "/tmp/proxy.harix" );
  getline( proxy_file, settingsProxy_ );
}
