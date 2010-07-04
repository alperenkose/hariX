#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WFileUpload>

#include "analyzeOS.hpp"
#include "home.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

AnalyzeOsWidget* AnalyzeOsWidget::instance_ = NULL;

AnalyzeOsWidget::AnalyzeOsWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;
  WAnchor *os_script_;

  panelAnalyze_ = new WPanel(this);
  panelAnalyze_->resize(1000,300);
  panelAnalyze_->setCentralWidget( layoutAnalyze_ = new WTable() );

  layoutAnalyze_->elementAt(0,0)->addWidget( os_script_ = new WAnchor("queryDistro.sh", "Get Script!") );
  os_script_->setTarget(TargetNewWindow);

  layoutAnalyze_->elementAt(1,0)->addWidget( new WText("Download and Run the script" ) );

  layoutAnalyze_->elementAt(2,0)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->elementAt(3,0)->addWidget( uploadPcimap_ = new WFileUpload() );
  layoutAnalyze_->elementAt(4,0)->addWidget( bUpload_ = new WPushButton("Upload") );

  // ------- @test {
  layoutAnalyze_->elementAt(4,1)->addWidget( os_file = new WText() );
  layoutAnalyze_->elementAt(4,2)->addWidget( pcimap_file = new WText() );
  // ------- @test }

  // Upload when the button is clicked.
  bUpload_->clicked().connect(SLOT(uploadOs_, Wt::WFileUpload::upload));
  // bUpload_->clicked().connect(uploadOs_, Wt::WFileUpload::upload);
  bUpload_->clicked().connect(SLOT(uploadPcimap_, Wt::WFileUpload::upload));

  // React to a succesfull upload.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);

  

  addWidget(new WBreak());

  addWidget( bGoHome_ = new WPushButton("Go Home"));  
  bGoHome_->clicked().connect(this, &AnalyzeOsWidget::bGoHome_Click);

}

AnalyzeOsWidget* AnalyzeOsWidget::Instance( WContainerWidget* parent)
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
	  instance_ = new AnalyzeOsWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

void AnalyzeOsWidget::osUploaded()
{
  os_file->setText(uploadOs_->spoolFileName()); // @test
}

void AnalyzeOsWidget::pcimapUploaded()
{
  pcimap_file->setText(uploadPcimap_->spoolFileName()); // @test
}


void AnalyzeOsWidget::bGoHome_Click()
{
  HomeWidget* home_page;
  if ( (home_page = HomeWidget::Instance()) == NULL )
	selectWidget( HomeWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( home_page );
}
