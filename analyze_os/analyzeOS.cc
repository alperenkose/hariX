#include <fstream>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WFileUpload>
#include <Wt/WInPlaceEdit>
#include <Wt/WGroupBox>
#include <Wt/WDialog>
#include <Wt/WSignalMapper>


#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WOverlayLoadingIndicator>
#include "../harixApp.hpp"		// needed for WApplication::instance()

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
  panelAnalyze_->resize(420,200);
  panelAnalyze_->setCentralWidget( layoutAnalyze_ = new WTable() );
  // panelAnalyze_->setStyleClass(".Wt-panel.body .Wt-outset .center");

  layoutAnalyze_->resize(400,160);
  layoutAnalyze_->rowAt(0)->setHeight(20);
  layoutAnalyze_->elementAt(0,0)->addWidget( os_script_ = new WAnchor("queryDistro.sh", "Get Script!") );
  os_script_->setTarget(TargetNewWindow);

  layoutAnalyze_->elementAt(1,0)->addWidget( new WText("Download and Run the script, <br />"
										    "there will be a distro.txt file generated on the folder,  <br />"
											"upload it together with modules.pcimap file of your kernel.. " ) );
  layoutAnalyze_->elementAt(1,0)->setColumnSpan(2);
  layoutAnalyze_->elementAt(1,0)->setLineHeight(15);
  layoutAnalyze_->rowAt(1)->setHeight(60);

  layoutAnalyze_->rowAt(2)->setHeight(25);
  layoutAnalyze_->elementAt(2,0)->addWidget( new WText("File distro.txt:") );
  layoutAnalyze_->elementAt(2,1)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->rowAt(3)->setHeight(25);
  layoutAnalyze_->elementAt(3,0)->addWidget( new WText("File modules.pcimap:") );
  layoutAnalyze_->elementAt(3,1)->addWidget( uploadPcimap_ = new WFileUpload() );
  layoutAnalyze_->rowAt(4)->setHeight(25);
  layoutAnalyze_->elementAt(4,0)->addWidget( bUpload_ = new WPushButton("Upload") );
  WPushButton* bGoHome1;
  layoutAnalyze_->elementAt(4,1)->addWidget( bGoHome1 = new WPushButton("Go Home") );
  layoutAnalyze_->elementAt(4,1)->setContentAlignment(AlignRight);
  bGoHome1->clicked().connect(this, &AnalyzeOsWidget::bGoHome_Click);

  // Upload when the button is clicked.
  bUpload_->clicked().connect(this, &AnalyzeOsWidget::bUpload_Click);

  // React to a succesfull upload.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);

  panelAnalyzeResult_ = new WPanel(this);
  panelAnalyzeResult_->hide();
  panelAnalyzeResult_->resize(600, WLength() );
  WTable* layoutResult;
  panelAnalyzeResult_->setCentralWidget( layoutResult = new WTable() );

  // layoutResult->rowAt(0)->setHeight( WLength(170) );
  layoutResult->rowAt(1)->setHeight( WLength(20) );
  layoutResult->columnAt(0)->setWidth( WLength(300) );
  layoutResult->elementAt(0,0)->addWidget( groupDetectedOs_ = new WGroupBox("Detected Operating System") );
  WTable* layoutDetectedOs;
  groupDetectedOs_->addWidget( layoutDetectedOs = new WTable() );
  layoutDetectedOs->elementAt(0,0)->addWidget( new WText("OS Name:" ) );
  layoutDetectedOs->elementAt(0,1)->addWidget( editOs_ = new WInPlaceEdit("") ); // @test
  layoutDetectedOs->elementAt(1,0)->addWidget( new WText("Release:" ) );
  layoutDetectedOs->elementAt(1,1)->addWidget( editRel_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(2,0)->addWidget( new WText("Kernel :" ) );
  layoutDetectedOs->elementAt(2,1)->addWidget( editKer_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(3,0)->addWidget( new WText("Arch.  :" ) );
  layoutDetectedOs->elementAt(3,1)->addWidget( editArch_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(4,0)->addWidget( bCheckOs_ = new WPushButton("Check Record") );
  bCheckOs_->clicked().connect(this, &AnalyzeOsWidget::bCheckOs_Click);
  // MANUALY CHANGE OS INFO
  editOs_->setStyleClass("inplace");
  editRel_->setStyleClass("inplace");
  editKer_->setStyleClass("inplace");
  editArch_->setStyleClass("inplace");
  editOs_->valueChanged().connect( this, &AnalyzeOsWidget::changeOsName );
  editRel_->valueChanged().connect( this, &AnalyzeOsWidget::changeRelease );
  editKer_->valueChanged().connect( this, &AnalyzeOsWidget::changeKernel );
  editArch_->valueChanged().connect( this, &AnalyzeOsWidget::changeArch );
    
  layoutResult->columnAt(1)->setWidth( WLength(50) );

  layoutResult->columnAt(2)->setWidth( WLength(250) );
  layoutResult->elementAt(0,2)->addWidget( layoutCheckResult_ = new WTable() );
  layoutCheckResult_->elementAt(0,0)->setColumnSpan(2);
  layoutCheckResult_->elementAt(0,0)->addWidget( txtOsResult_ = new WText() );
  layoutCheckResult_->elementAt(1,0)->addWidget( bOsAddUpdate_ = new WPushButton() );
  layoutCheckResult_->elementAt(1,1)->addWidget( bOsCancel_ = new WPushButton("Cancel / Edit") );
  layoutCheckResult_->hide();

  bOsAddUpdate_->clicked().connect(this, &AnalyzeOsWidget::bOsAddUpdate_Click);
  bOsCancel_->clicked().connect(this, &AnalyzeOsWidget::bOsCancel_Click);

  WPushButton* bGoHome2;
  layoutResult->elementAt(1,2)->addWidget( bGoHome2 = new WPushButton("Go Home") );
  layoutResult->elementAt(1,2)->setContentAlignment(AlignRight);
  addWidget(new WBreak());

  bGoHome2->clicked().connect(this, &AnalyzeOsWidget::bGoHome_Click);

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
  WOverlayLoadingIndicator *loading;
  WApplication::instance()->setLoadingIndicator( loading = new WOverlayLoadingIndicator() );
  loading->setMessage("Analyzing..");
}

void AnalyzeOsWidget::changeOsName( WString os_name )
{
  osDist_->setDistro(os_name.narrow());
}
void AnalyzeOsWidget::changeRelease( WString release )
{
  osDist_->setRelease(release.narrow());
}
void AnalyzeOsWidget::changeKernel( WString kernel )
{
  osDist_->setKernel(kernel.narrow());
}
void AnalyzeOsWidget::changeArch( WString arch )
{
  osDist_->setArch(arch.narrow());
}

void AnalyzeOsWidget::bOsCancel_Click()
{
  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );
  layoutCheckResult_->hide();
  groupDetectedOs_->enable();
}


int os_pci_module( OsInfo osInfo, std::string pcimapFile ); // external os_pci_module.cc


void AnalyzeOsWidget::bOsAddUpdate_Click()
{
  dialogResult_ = new WDialog("OS Analysis Result"); // @TODO: SINIFA KOYABILIRSIN BOLECE SILERSIN..
  
  if( os_pci_module( *osDist_, pcimap_file_ ) == 0 ){
	new WText("OS succesfully added/updated! <br /> Now Directing to homepage", dialogResult_->contents() );
	new WBreak( dialogResult_->contents() );
	WPushButton* dialogResultOk = new WPushButton("Ok", dialogResult_->contents());

	(*dialogResultOk).clicked().connect( SLOT(dialogResult_, WDialog::accept) );

	dialogResult_->show();
  }
  else{
  	new WText("FAILED to add/update OS! <br /> "
  			  "Probably because of an insert failure into pcimap database", dialogResult_->contents() );
  	new WBreak( dialogResult_->contents() );
  	WPushButton* dialogResultFail = new WPushButton("Close", dialogResult_->contents());

	(*dialogResultFail).clicked().connect( SLOT(dialogResult_, WDialog::reject) );
	
  	dialogResult_->show();
  }
  dialogResult_->finished().connect(this, &AnalyzeOsWidget::redirectAndDestroyDialog);
}

void AnalyzeOsWidget::redirectAndDestroyDialog( WDialog::DialogCode code )
{
  if ( code == WDialog::Accepted )
	bGoHome_Click();
  else
	bOsCancel_Click();
  delete dialogResult_;
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

  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );
  // Also delete file names!!
  delete uploadOs_;
  delete uploadPcimap_;
  layoutAnalyze_->elementAt(2,1)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->elementAt(3,1)->addWidget( uploadPcimap_ = new WFileUpload() );
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
