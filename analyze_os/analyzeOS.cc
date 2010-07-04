#include <fstream>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WFileUpload>
#include <Wt/WLineEdit>
#include <Wt/WGroupBox>

#include "analyzeOS.hpp"
#include "../home.hpp"
#include "../os_info.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

AnalyzeOsWidget* AnalyzeOsWidget::instance_ = NULL;
bool AnalyzeOsWidget::isOsUploaded_ = false;
bool AnalyzeOsWidget::isPcimapUploaded_ = false;

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

  // Upload when the button is clicked.
  bUpload_->clicked().connect(this, &AnalyzeOsWidget::bUpload_Click);

  // React to a succesfull upload.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);

  panelAnalyzeResult_ = new WPanel(this);
  panelAnalyzeResult_->hide();
  WTable* layoutResult;
  panelAnalyzeResult_->setCentralWidget( layoutResult = new WTable() );


  layoutResult->elementAt(0,0)->addWidget( groupDetectedOs_ = new WGroupBox("Detected Operating System") );
  WTable* layoutDetectedOs;
  groupDetectedOs_->addWidget( layoutDetectedOs = new WTable() );
  layoutDetectedOs->elementAt(0,0)->addWidget( new WText("OS Name:" ) );
  layoutDetectedOs->elementAt(0,1)->addWidget( editOs_ = new WLineEdit() );
  layoutDetectedOs->elementAt(1,0)->addWidget( new WText("Release:" ) );
  layoutDetectedOs->elementAt(1,1)->addWidget( editRel_ = new WLineEdit() );
  layoutDetectedOs->elementAt(2,0)->addWidget( new WText("Kernel :" ) );
  layoutDetectedOs->elementAt(2,1)->addWidget( editKer_ = new WLineEdit() );
  layoutDetectedOs->elementAt(3,0)->addWidget( new WText("Arch.  :" ) );
  layoutDetectedOs->elementAt(3,1)->addWidget( editArch_ = new WLineEdit() );
  layoutDetectedOs->elementAt(4,0)->addWidget( bCheckOs_ = new WPushButton("Check Record") );
  bCheckOs_->clicked().connect(this, &AnalyzeOsWidget::bCheckOs_Click);
  // @TODO: ADD EDIT BUTTON TO MANUALY CHANGE OS INFO

  layoutResult->columnAt(1)->setWidth( WLength(50) );

  layoutResult->elementAt(0,2)->addWidget( layoutCheckResult_ = new WTable() );
  layoutCheckResult_->elementAt(0,0)->setColumnSpan(2);
  layoutCheckResult_->elementAt(0,0)->addWidget( txtOsResult_ = new WText() );
  layoutCheckResult_->elementAt(1,0)->addWidget( bOsAddUpdate_ = new WPushButton() );
  layoutCheckResult_->elementAt(1,1)->addWidget( bOsCancel_ = new WPushButton("Cancel / Edit") );
  layoutCheckResult_->hide();

  bOsAddUpdate_->clicked().connect(this, &AnalyzeOsWidget::bOsAddUpdate_Click);
  bOsCancel_->clicked().connect(this, &AnalyzeOsWidget::bOsCancel_Click);

  addWidget(new WBreak());

  addWidget( bGoHome_ = new WPushButton("Go Home"));  
  bGoHome_->clicked().connect(this, &AnalyzeOsWidget::bGoHome_Click);

}

AnalyzeOsWidget* AnalyzeOsWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new AnalyzeOsWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  isOsUploaded_=false;
  isPcimapUploaded_=false;

  return instance_;

}

void AnalyzeOsWidget::bUpload_Click()
{
  uploadOs_->upload();
  uploadPcimap_->upload();
}

void AnalyzeOsWidget::osUploaded()
{
  os_file_ = uploadOs_->spoolFileName();
  if ( !uploadOs_->emptyFileName() ){
	isOsUploaded_ = true;
	uploadDone();
  }
}

void AnalyzeOsWidget::pcimapUploaded()
{
  pcimap_file_ = uploadPcimap_->spoolFileName();
  if ( !uploadPcimap_->emptyFileName() ){
	isPcimapUploaded_ = true;
	uploadDone();
  }
}

void AnalyzeOsWidget::uploadDone()
{
  if ( isOsUploaded_ && isPcimapUploaded_ ){
	readOs();
	panelAnalyze_->hide();
	panelAnalyzeResult_->show();
	isOsUploaded_=false;
	isPcimapUploaded_=false;
  }
	
}

void AnalyzeOsWidget::readOs()
{
  std::string os_name, release, kernel, architecture;
  std::ifstream distro(os_file_.c_str());
  getline(distro,os_name);
  getline(distro,release);
  getline(distro,kernel);
  getline(distro,architecture);

  osDist_ = new OsInfo(os_name,release,kernel,architecture);

  fillDetectedOs();
}

void AnalyzeOsWidget::fillDetectedOs()
{
  editOs_->setText( osDist_->getDistro() );
  editRel_->setText( osDist_->getRelease() );
  editKer_->setText( osDist_->getKernel() );
  editArch_->setText( osDist_->getArch() );
}

std::string queryOsKernelId(std::string os_name,std::string release,std::string kernel,std::string architecture);

void AnalyzeOsWidget::bCheckOs_Click()
{
  groupDetectedOs_->disable();
  std::string osKernelId;
  osKernelId=queryOsKernelId(osDist_->getDistro(),osDist_->getRelease(),osDist_->getKernel(),osDist_->getArch());
  if( osKernelId != ""){
	// OS Found!
	txtOsResult_->setText("Operating System Already in Database!");
	bOsAddUpdate_->setText("Update Pcimap!");
	layoutCheckResult_->show();
  }
  else{
	// Os not found..
	txtOsResult_->setText("Operating System NOT Found! <br /> Add Operating System to Database");
	bOsAddUpdate_->setText("Add OS!");
	layoutCheckResult_->show();
  }
}

void os_pci_module( OsInfo osInfo, std::string pcimapFile ); // external os_pci_module.cc

void AnalyzeOsWidget::bOsAddUpdate_Click()
{
  os_pci_module( *osDist_, pcimap_file_ );
  bOsCancel_Click();			// @test
  // @TODO: put a popup here, informing addition of OS or pcimap update..
}

void AnalyzeOsWidget::bOsCancel_Click()
{
  layoutCheckResult_->hide();
  groupDetectedOs_->enable();
}

void AnalyzeOsWidget::bGoHome_Click()
{
  HomeWidget* home_page;
  if ( (home_page = HomeWidget::Instance()) == NULL )
	selectWidget( HomeWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( home_page );

  resetAll();
}

void AnalyzeOsWidget::resetAll()
{
  // Also delete file names!!
  delete uploadOs_;
  delete uploadPcimap_;
  layoutAnalyze_->elementAt(2,0)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->elementAt(3,0)->addWidget( uploadPcimap_ = new WFileUpload() );
  // Upload when the button is clicked.
  bUpload_->clicked().connect(this, &AnalyzeOsWidget::bUpload_Click);
  // React to a succesfull upload.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);

  panelAnalyze_->show();
  panelAnalyzeResult_->hide();

  layoutCheckResult_->hide();
  groupDetectedOs_->enable();
}
