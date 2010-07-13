#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WOverlayLoadingIndicator>
#include "harixApp.hpp"

#include "home.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "mainboards/mainboards.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

HomeWidget* HomeWidget::instance_ = NULL;

HomeWidget::HomeWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;

  addWidget( bAnalyze_ = new WPushButton("Analyze Client OS") ); 	// 
  bAnalyze_->clicked().connect(this, &HomeWidget::bAnalyze_Click);

  addWidget( new WBreak() );

  addWidget( bQueryLspci_ = new WPushButton("Query lspci list") );
  bQueryLspci_->clicked().connect(this, &HomeWidget::bQueryLspci_Click);

  addWidget( new WBreak() );

  addWidget( bListBoards_ = new WPushButton("List MainBoards") );
  bListBoards_->clicked().connect(this, &HomeWidget::bListBoards_Click);

  addWidget( new WBreak() );

  addWidget( bPciIds_ = new WPushButton("Update PCI IDS") );
  bPciIds_->clicked().connect(this, &HomeWidget::bPciIds_Click);



}

HomeWidget* HomeWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	// wApp->log("debug") << "parent is 0!"; 					// @test
	if ( !instance_ ){
	  // wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	// wApp->log("debug") << "received parent!"; 				// @test
	if( !instance_ ){
	  instance_ = new HomeWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

void HomeWidget::bAnalyze_Click()
{
  AnalyzeOsWidget* analyze_page;
  if ( ( analyze_page = AnalyzeOsWidget::Instance()) == NULL )
	selectWidget( AnalyzeOsWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( analyze_page );

  
}

void HomeWidget::bQueryLspci_Click()
{
  PcimapQueryWidget* pcimapquery_page;
  if ( (pcimapquery_page = PcimapQueryWidget::Instance()) == NULL )
	selectWidget( PcimapQueryWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( pcimapquery_page );
}

void HomeWidget::bListBoards_Click()
{
  MainboardsWidget* mainboards_page;
  if ( (mainboards_page = MainboardsWidget::Instance()) == NULL )
	selectWidget( MainboardsWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( mainboards_page );
  
}

static std::string pci_ids_file;

int update_pci_ids(std::string pci_ids_file);

void HomeWidget::bPciIds_Click()
{
  Wt::WApplication::instance()->setLoadingIndicator( loading_ = new Wt::WOverlayLoadingIndicator() );
  loading_->setMessage("Updating..");
  
  pci_ids_file = "/tmp/pci.ids";

  // @TODO: way to change proxy settings!!
  if(system("curl http://pciids.sourceforge.net/v2.2/pci.ids --proxy http://172.25.159.4:81 > /tmp/pci.ids")== 0){
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
	new WText("Failed to download pci.ids!!", dialogDownload_->contents() );
	new WBreak( dialogDownload_->contents() );
	WPushButton* dialogDownloadFail = new WPushButton("Close", dialogDownload_->contents() );
	dialogDownloadFail->clicked().connect( SLOT(dialogDownload_, WDialog::reject) );
	dialogDownload_->show();
  }

  dialogDownload_->finished().connect(this, &HomeWidget::dialogDownloadEnd);

}


void HomeWidget::dialogDownloadEnd( WDialog::DialogCode code )
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
	dialogPciIds_->finished().connect(this, &HomeWidget::dialogPciEnd);
  }

  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );

  delete dialogDownload_;
}


void HomeWidget::dialogPciEnd( WDialog::DialogCode code )
{
  delete dialogPciIds_;
}
