/* home.cc --- 
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
  \file   home.cc
  \date   Thu Jul 15 16:04:25 2010
  
  \brief  Implementation of HomeWidget Class
  
  
*/
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WInPlaceEdit>
#include <Wt/WLineEdit>
#include <Wt/WDefaultLoadingIndicator>
#include <Wt/WOverlayLoadingIndicator>
#include <fstream>

#include "harixApp.hpp"
#include "home.hpp"
#include "div.hpp"				// Declaration of a WContainerWidget to represent an html <div>

using namespace Wt;

HomeWidget* HomeWidget::instance_ = NULL;

HomeWidget::HomeWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;

  /*
   * Header Part for navigation buttons
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

  /*
	Container 'page' holding contents shown in page apart from the Header Part.
	Anything else from the Header to be shown in page is added to 'page' container.
  */
  Div *page = new Div(content, "pagearea");

  Div *footer = new Div(result, "footr");
  footer->addWidget( new WText("<p>HARdware Information management<br />"
							   "This site is best viewed in a Firefox browser!</p>") );
  /*
   * @}
   */

  WAnchor *aManual;
  page->addWidget( new WText("Wellcome to the Linux Hardware Information Management Application, <br />"
							 "For usage instructions and what hariX is please see the ") );
  page->addWidget( aManual = new WAnchor("harixManual.pdf","User Manual.") );
  aManual->setTarget(TargetNewWindow);
  
  page->addWidget( new WText("<br/><br/>"
							 "Don't forget to update the PCI and Class Codes occasionally at here:"
							 "<br/>"));

  page->addWidget( bPciIds_ = new WPushButton("Update PCI IDs") );
  // Trigger bPciIds_Click function on 'Update PCI IDs' click.
  bPciIds_->clicked().connect(this, &HomeWidget::bPciIds_Click);


  /*
   * Settings Panel
   * @{
   */
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

  layoutSettings->rowAt(1)->setHeight( WLength(30) );
  layoutSettings->elementAt(1,1)->setContentAlignment(AlignMiddle);
  WAnchor *aDoc;
  layoutSettings->elementAt(1,1)->addWidget( aDoc = new WAnchor("doc/html/index.html",
																"--- Documentation of Source Code ---") );
  aDoc->setTarget(TargetNewWindow);
  /*
   * @}
   */


  readProxyFile();
  editProxy_->setText( settingsProxy_ );
  editProxy_->setEmptyText("No Proxy Set!");
  
  // Trigger changeProxy function on 'Save'(bSaveProxy) button click.
  bSaveProxy->clicked().connect(this, &HomeWidget::changeProxy);

}

HomeWidget* HomeWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // we didn't receive a parent and there is no instance of HomeWidget, so return NULL..
	  return NULL;
	}
  }
  else{							// We received a parent container
	if( !instance_ ){
	  instance_ = new HomeWidget(parent); // No instance of HomeWidget, lets create one.
	}
	// else don't care, we already have an instance we will return that.
  }
  return instance_;

}

//! Path to the "pci.ids" file to be downloaded and read.
static std::string pci_ids_file = "/tmp/pci.ids";

/*
  Global function from pci_ids/pci_ids.cc to update PCI IDs in database,
  gets 'pci.ids' file as parameter.
*/
//! \relates pci_ids.cc
int update_pci_ids(std::string pci_ids_file);



void HomeWidget::bPciIds_Click()
{
  // Change server busy indicator.
  Wt::WApplication::instance()->setLoadingIndicator( loading_ = new Wt::WOverlayLoadingIndicator() );
  loading_->setMessage("Updating..");
  

  /*
   * Set the download command appropriately depending on proxy address existance.
   * @{
   */
  std::string download_command;
  if( settingsProxy_ != "" )
	download_command =
	  "curl http://pciids.sourceforge.net/v2.2/pci.ids --proxy "+ settingsProxy_ +" > " + pci_ids_file;
  else
	download_command =
	  "curl http://pciids.sourceforge.net/v2.2/pci.ids > " + pci_ids_file;
  /*
   * @}
   */
  

  if( system( download_command.c_str()  ) == 0 ){ 		// Try to download the 'pci.ids' file from net.
	dialogDownload_ = new WDialog("PCI IDs Download");
	new WText("Downloaded pci.ids file! <br />"
			  "Now We will update the PCI IDs database", dialogDownload_->contents() );
	new WBreak( dialogDownload_->contents() );
	WPushButton* dialogDownloadOk = new WPushButton("Ok", dialogDownload_->contents() );

	// Sets WDialog::DialogCode to Accepted if OK is clicked.
	dialogDownloadOk->clicked().connect( SLOT(dialogDownload_, WDialog::accept) );
	
	WPushButton* dialogDownloadCancel = new WPushButton("Cancel", dialogDownload_->contents() );

	// Sets WDialog::DialogCode to Rejected if Cancel is clicked.
	dialogDownloadCancel->clicked().connect( SLOT(dialogDownload_, WDialog::reject) );
	dialogDownload_->show();
  }
  else {												// 'pci.ids' Download failed!
	dialogDownload_ = new WDialog("PCI IDs Download");
	new WText("Failed to download pci.ids!!<br />"
			  "Check proxy settings..", dialogDownload_->contents() );
	new WBreak( dialogDownload_->contents() );
	WPushButton* dialogDownloadFail = new WPushButton("Close", dialogDownload_->contents() );

	// Sets WDialog::DialogCode to Rejected when Close is clicked.
	dialogDownloadFail->clicked().connect( SLOT(dialogDownload_, WDialog::reject) );
	dialogDownload_->show();
  }

  // Trigger 'dialogDownload_Close' function on 'dialogDownload_' dialog window close.
  dialogDownload_->finished().connect(this, &HomeWidget::dialogDownload_Close);

}


void HomeWidget::dialogDownload_Close( WDialog::DialogCode code )
{
  if ( code == WDialog::Accepted ){ 	// if download is successful and user not rejected update.
	dialogPciIds_ = new WDialog("PCI IDs Update");
  
	if( update_pci_ids( pci_ids_file ) == 0 ){ 		// Try to update PCI IDs.
	  new WText("PCI IDs succesfully updated!", dialogPciIds_->contents() );
	  new WBreak( dialogPciIds_->contents() );
	  WPushButton* dialogPciIdsOk = new WPushButton("Ok", dialogPciIds_->contents());

	  (*dialogPciIdsOk).clicked().connect( SLOT(dialogPciIds_, WDialog::accept) );

	  dialogPciIds_->show();
	}
	else{											// PCI IDs update FAILED.
	  new WText("FAILED to update PCI IDs!!", dialogPciIds_->contents() );
	  new WBreak( dialogPciIds_->contents() );
	  WPushButton* dialogPciIdsFail = new WPushButton("Close", dialogPciIds_->contents());

	  (*dialogPciIdsFail).clicked().connect( SLOT(dialogPciIds_, WDialog::reject) );
	
	  dialogPciIds_->show();
	}
	dialogPciIds_->finished().connect(this, &HomeWidget::dialogPci_Close);
  }

  // revert back the server busy indicator to its default.
  WApplication::instance()->setLoadingIndicator( new WDefaultLoadingIndicator() );

  delete dialogDownload_;
}


void HomeWidget::dialogPci_Close( WDialog::DialogCode code )
{
  delete dialogPciIds_;
}

void HomeWidget::changeProxy()
{
  settingsProxy_ = editProxy_->text().narrow(); // Put the given proxy by user into settingsProxy_ variable.
  
  std::string save_command = "echo "+ settingsProxy_ +" > /tmp/proxy.harix"; // Command string to save proxy.
  
  if( system( save_command.c_str()  ) != 0 ){ 	// Try to store the proxy address in server for later use.
	// Warn in case of save failure! Proxy address couldn't be saved in file.
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
  getline( proxy_file, settingsProxy_ ); // read previously stored proxy address into settingsProxy_ variable.
}
