/* analyzeOS.cc --- 
 *
 * Copyright (C) 2010 Alp Eren Köse
 *
 * Author: Alp Eren Köse <alperenkose@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*!
  \file   analyzeOS.cc
  \date   Mon Jul 19 10:58:44 2010
  
  \brief  Implementation of AnalyzeOsWidget Class
  
*/
#include <fstream>
#include <Wt/WTable>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WFileUpload>
#include <Wt/WInPlaceEdit>
#include <Wt/WGroupBox>

#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WOverlayLoadingIndicator>

#include "../harixApp.hpp"		// needed for WApplication::instance()
#include "analyzeOS.hpp"
#include "../os_info.hpp"
#include "../div.hpp"

using namespace Wt;

AnalyzeOsWidget* AnalyzeOsWidget::instance_ = NULL; // Pointer to the only instance of AnalyzeOsWidget.
bool AnalyzeOsWidget::isOsUploaded_ = false;		// Set to true if 'distro.txt' is uploaded.
bool AnalyzeOsWidget::isPcimapUploaded_ = false;	// Set to true if 'modules.pcimap' is uploaded.

AnalyzeOsWidget::AnalyzeOsWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;

  /*
   * Header Part for navigation
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
  aAnalyzeOs->setStyleClass("selected");
  WAnchor *aQueryDevices;
  menulist->addWidget( aQueryDevices = new WAnchor("","Query Devices") );
  aQueryDevices->setRefInternalPath("/query_devices");
  WAnchor *aMainboards;
  menulist->addWidget( aMainboards = new WAnchor("","Mainboards") );
  aMainboards->setRefInternalPath("/mainboards");
  
  /*
	Container 'page' holding contents shown in page apart from the Header Part.
	Anything else from the Header to be shown in page is added to 'page' container.
  */
  Div *page = new Div(content, "pagearea");
  /*
   * @}
   */



  /*
   * Upload Screen
   * @{
   */
  WAnchor *os_script_;

  page->addWidget( layoutAnalyze_ = new WTable() );

  layoutAnalyze_->resize(400,160);
  layoutAnalyze_->rowAt(0)->setHeight(20);
  
  // Put a download link for script.
  layoutAnalyze_->elementAt(0,0)->addWidget( os_script_ = new WAnchor("queryDistro.sh", "Get Script!") );
  os_script_->setTarget(TargetNewWindow);

  layoutAnalyze_->elementAt(1,0)->addWidget( new WText("Download and Run the script, <br />"
										    "there will be a distro.txt file generated on the folder,  <br />"
											"upload it together with modules.pcimap file of your kernel.. <br />"
											"*Given file names must match!"));
  layoutAnalyze_->elementAt(1,0)->setColumnSpan(2);
  layoutAnalyze_->elementAt(1,0)->setLineHeight(15);
  layoutAnalyze_->rowAt(1)->setHeight(60);

  layoutAnalyze_->rowAt(2)->setHeight(25);
  layoutAnalyze_->elementAt(2,0)->addWidget( new WText("File *distro.txt:") );
  
  // File Upload Area for 'distro.txt'
  layoutAnalyze_->elementAt(2,1)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->rowAt(3)->setHeight(25);
  
  // File Upload Area for 'modules.pcimap'
  layoutAnalyze_->elementAt(3,0)->addWidget( new WText("File *modules.pcimap:") );
  layoutAnalyze_->elementAt(3,1)->addWidget( uploadPcimap_ = new WFileUpload() );
  layoutAnalyze_->rowAt(4)->setHeight(25);
  layoutAnalyze_->elementAt(4,0)->addWidget( bUpload_ = new WPushButton("Upload") );
  
  // Trigger bUpload_Click() function on 'Upload' click. 
  bUpload_->clicked().connect(this, &AnalyzeOsWidget::bUpload_Click);

  // React to a succesfull upload.Call osUploaded() or pcimapUploaded() function when respective uploads are done.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);
  /*
   * @}
   */



  
  /*
   * Result Screen
   * @{
   */
  page->addWidget( layoutResult_ = new WTable() );
  layoutResult_->hide();

  layoutResult_->rowAt(1)->setHeight( WLength(20) );
  layoutResult_->columnAt(0)->setWidth( WLength(300) );

  // Create a group box to hold OS information edit regions.
  layoutResult_->elementAt(0,0)->addWidget( groupDetectedOs_ = new WGroupBox("Detected Operating System") );
  WTable* layoutDetectedOs;
  groupDetectedOs_->addWidget( layoutDetectedOs = new WTable() );

  // Editable text areas for OS information
  layoutDetectedOs->elementAt(0,0)->addWidget( new WText("OS Name:" ) );
  layoutDetectedOs->elementAt(0,1)->addWidget( editOs_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(1,0)->addWidget( new WText("Release:" ) );
  layoutDetectedOs->elementAt(1,1)->addWidget( editRel_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(2,0)->addWidget( new WText("Kernel :" ) );
  layoutDetectedOs->elementAt(2,1)->addWidget( editKer_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(3,0)->addWidget( new WText("Arch.  :" ) );
  layoutDetectedOs->elementAt(3,1)->addWidget( editArch_ = new WInPlaceEdit("") );
  layoutDetectedOs->elementAt(4,0)->addWidget( new WBreak() );
  
  layoutDetectedOs->elementAt(5,0)->addWidget( bCheckOs_ = new WPushButton("Check Record") );

  // Trigger bCheckOs_Click() function on 'Check Record' click.
  bCheckOs_->clicked().connect(this, &AnalyzeOsWidget::bCheckOs_Click);
  
  // Set CSS style for hover efect on edit regions
  editOs_->setStyleClass("inplace");
  editRel_->setStyleClass("inplace");
  editKer_->setStyleClass("inplace");
  editArch_->setStyleClass("inplace");

  // Call the corresponding functions on 'Save' click of edit regions.
  editOs_->valueChanged().connect( this, &AnalyzeOsWidget::changeOsName );
  editRel_->valueChanged().connect( this, &AnalyzeOsWidget::changeRelease );
  editKer_->valueChanged().connect( this, &AnalyzeOsWidget::changeKernel );
  editArch_->valueChanged().connect( this, &AnalyzeOsWidget::changeArch );
    
  layoutResult_->columnAt(1)->setWidth( WLength(50) );

  /*
	Area to show the results of OS Record Check.
	Consisting of a Text area for the message, an 'Add OS' or 'Update Pcimap' button
	and 'Cancel' button.
  */
  layoutResult_->columnAt(2)->setWidth( WLength(250) );
  layoutResult_->elementAt(0,2)->addWidget( layoutCheckResult_ = new WTable() );
  layoutCheckResult_->elementAt(0,0)->addWidget( new WBreak() );
  layoutCheckResult_->elementAt(1,0)->setColumnSpan(2);
  layoutCheckResult_->elementAt(1,0)->setContentAlignment(AlignCenter);
  layoutCheckResult_->elementAt(1,0)->addWidget( txtOsResult_ = new WText() );
  layoutCheckResult_->elementAt(2,0)->addWidget( new WBreak() );
  layoutCheckResult_->elementAt(3,0)->setContentAlignment(AlignCenter);
  layoutCheckResult_->elementAt(3,0)->addWidget( bOsAddUpdate_ = new WPushButton() );
  layoutCheckResult_->elementAt(3,1)->setContentAlignment(AlignCenter);
  layoutCheckResult_->elementAt(3,1)->addWidget( bOsCancel_ = new WPushButton("Cancel / Edit") );
  layoutCheckResult_->hide();

  // Trigger bOsAddUpdate_Click() function on 'Add OS' or 'Update Pcimap' click.
  bOsAddUpdate_->clicked().connect(this, &AnalyzeOsWidget::bOsAddUpdate_Click);

  // Trigger bOsCancel_Click() function on 'Cancel / Edit' click.
  bOsCancel_->clicked().connect(this, &AnalyzeOsWidget::bOsCancel_Click);

  layoutResult_->elementAt(1,0)->addWidget( new WText("*Fields above are editable!") );
  /*
   * @}
   */

}

AnalyzeOsWidget* AnalyzeOsWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // we didn't receive a parent and there is no instance of AnalyzeOsWidget, so return NULL..
	  return NULL;
	}
  }
  else{							// We received a parent container
	if( !instance_ ){
	  instance_ = new AnalyzeOsWidget(parent);
	}
	// else don't care, we already have an instance we will return that.
  }

  isOsUploaded_=false;
  isPcimapUploaded_=false;

  return instance_;
}


void AnalyzeOsWidget::bUpload_Click()
{
  uploadOs_->upload();			// This will trigger uploaded event of uploadOs_ file upload area.
  uploadPcimap_->upload();		// This will trigger uploaded event of uploadPcimap_ file upload area.
}


void AnalyzeOsWidget::osUploaded()
{
  // Get temporary name of uploaded 'distro.txt' file at Server. Files are given random names on upload.
  os_file_ = uploadOs_->spoolFileName();
  if ( !uploadOs_->emptyFileName() ){
	if( uploadOs_->clientFileName().narrow() == "distro.txt" ){
	  isOsUploaded_ = true;
	  uploadDone();	  
	}
  }
}


void AnalyzeOsWidget::pcimapUploaded()
{
  // Get temporary name of uploaded 'modules.pcimap' file at Server. Files are given random names on upload.
  pcimap_file_ = uploadPcimap_->spoolFileName();
  if ( !uploadPcimap_->emptyFileName() ){
	if( uploadPcimap_->clientFileName().narrow() == "modules.pcimap" ){
	  isPcimapUploaded_ = true;
	  uploadDone();
	}
  }
}


void AnalyzeOsWidget::uploadDone()
{
  if ( isOsUploaded_ && isPcimapUploaded_ ){ 	// Check if both 'distro.txt' and 'modules.pcimap' are uploaded.
	readOs();					// Read OS information from 'distro.txt' file.
	layoutAnalyze_->hide();
	layoutResult_->show();		// Switch to the Result screen.
	isOsUploaded_=false;
	isPcimapUploaded_=false;
  }
	
}

void AnalyzeOsWidget::readOs()
{
  std::string os_name, release, kernel, architecture;
  std::ifstream distro(os_file_.c_str()); 	// Open 'distro.txt' file.
  getline(distro,os_name);
  getline(distro,release);
  getline(distro,kernel);
  getline(distro,architecture);

  // Create an OsInfo object with the OS information read.
  osDist_ = new OsInfo(os_name,release,kernel,architecture);

  // Fill in the edit regions at Result screen.
  fillDetectedOs();
}


void AnalyzeOsWidget::fillDetectedOs()
{
  editOs_->setText( osDist_->getDistro() );
  editRel_->setText( osDist_->getRelease() );
  editKer_->setText( osDist_->getKernel() );
  editArch_->setText( osDist_->getArch() );
}


/*
  Global function from database.cc to query uKernelID of
  the given operating system.
*/
//! \relates database.cc
std::string queryOsKernelId(std::string os_name,std::string release,std::string kernel,std::string architecture);


void AnalyzeOsWidget::bCheckOs_Click()
{
  groupDetectedOs_->disable();
  
  std::string osKernelId;
  // Query unique kernel id.
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


/*
 * Update corresponding parts of the osDist_, OsInfo object
 * on 'Save' button click of InPlaceEdit regions..
 * @{
 */
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
/*
 * @}
 */



void AnalyzeOsWidget::bOsCancel_Click()
{
  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );
  layoutCheckResult_->hide();
  groupDetectedOs_->enable();
}



/*
  Global function from pci_module/os_pci_module.cc to store OS along with
  it's 'modules.pcimap' list in database, or just to update the 'modules.pcimap' list
  if the given OS exists.
*/
//! \relates os_pci_module.cc
int os_pci_module( OsInfo osInfo, std::string pcimapFile );



void AnalyzeOsWidget::bOsAddUpdate_Click()
{
  dialogResult_ = new WDialog("OS Analysis Result");
  
  if( os_pci_module( *osDist_, pcimap_file_ ) == 0 ){ 		// Add/Update OS-modules.pcimap in database.
	new WText("OS succesfully added/updated! <br /> Now Directing to homepage", dialogResult_->contents() );
	new WBreak( dialogResult_->contents() );
	WPushButton* dialogResultOk = new WPushButton("Ok", dialogResult_->contents());

	// Sets WDialog::DialogCode to Accepted when OK is clicked which happens on successful update.
	(*dialogResultOk).clicked().connect( SLOT(dialogResult_, WDialog::accept) );

	dialogResult_->show();
  }
  else{
  	new WText("FAILED to add/update OS! <br /> "
  			  "Probably because of an insert failure into pcimap database", dialogResult_->contents() );
  	new WBreak( dialogResult_->contents() );
  	WPushButton* dialogResultFail = new WPushButton("Close", dialogResult_->contents());

	// Sets WDialog::DialogCode to Rejected when Close is clicked which happens on failure of update.
	(*dialogResultFail).clicked().connect( SLOT(dialogResult_, WDialog::reject) );
	
  	dialogResult_->show();
  }

  // Trigger 'redirectAndDestroyDialog' function on 'dialogResult_' dialog window close.
  dialogResult_->finished().connect(this, &AnalyzeOsWidget::redirectAndDestroyDialog);
}

void AnalyzeOsWidget::redirectAndDestroyDialog( WDialog::DialogCode code )
{
  if ( code == WDialog::Accepted ) 	// if OS-pcimap update is successful.
	goHome();
  else								// There was a failure on update.
	bOsCancel_Click();
  delete dialogResult_;
}


void AnalyzeOsWidget::goHome()
{
  /*
	set URL to /application_path.wt#/home,
	also trigger WApplication::internalPathChanged(),
	which is binded to HarixApp::internalPathChanged() function.
  */
  WApplication::instance()->setInternalPath("/home",true);
}


void AnalyzeOsWidget::resetAll()
{
  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );

  // Delete File Upload areas to refresh page, and delete files on server..
  delete uploadOs_;
  delete uploadPcimap_;
  
  layoutAnalyze_->elementAt(2,1)->addWidget( uploadOs_ = new WFileUpload() );
  layoutAnalyze_->elementAt(3,1)->addWidget( uploadPcimap_ = new WFileUpload() );
  
  // Refresh bindings..
  bUpload_->clicked().connect(this, &AnalyzeOsWidget::bUpload_Click);
  
  // React to a succesfull upload.
  uploadOs_->uploaded().connect(this, &AnalyzeOsWidget::osUploaded);
  uploadPcimap_->uploaded().connect(this, &AnalyzeOsWidget::pcimapUploaded);

  // Change to Upload Screen.
  layoutAnalyze_->show();
  layoutResult_->hide();

  layoutCheckResult_->hide();
  groupDetectedOs_->enable();

}
