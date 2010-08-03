/*!
  \file   pcimapQuery.cc
  \date   Thu Jul 22 15:51:35 2010
  
  \brief  Implementation of PcimapQueryWidget Class
  
*/
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WTextArea>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WLogger>

#include "../harixApp.hpp"		// needed for WApplication::instance()
#include "pcimapQuery.hpp"
#include "pcimapResult.hpp"
#include "../pci_device.hpp"
#include "../div.hpp"

using namespace Wt;


//! \relates harixApp.cc
void selectWidget( WContainerWidget* widget );


PcimapQueryWidget* PcimapQueryWidget::instance_ = NULL; // Pointer to the only instance of PcimapQueryWidget.
std::vector<PciDevice> PcimapQueryWidget::lspci_list_;	// To hold the list of PCI Devices read.



PcimapQueryWidget::PcimapQueryWidget( WContainerWidget* parent ) : WContainerWidget(parent)
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
  WAnchor *aQueryDevices;
  menulist->addWidget( aQueryDevices = new WAnchor("","Query Devices") );
  aQueryDevices->setRefInternalPath("/query_devices");
  aQueryDevices->setStyleClass("selected");
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
  

  WTable* layoutQuery;
  page->addWidget( layoutQuery = new WTable() );

  layoutQuery->elementAt(0,0)->setColumnSpan(2);
  layoutQuery->elementAt(0,0)->addWidget( new WText("Paste below the output of `lspci -mn`:") );
  
  layoutQuery->elementAt(1,0)->setColumnSpan(2);
  layoutQuery->elementAt(1,0)->setContentAlignment(AlignCenter);
  // Text area for user input of 'lspci -mn' command.
  layoutQuery->elementAt(1,0)->addWidget(pcimapList_ = new WTextArea());

  pcimapList_->resize(450, WLength());
  pcimapList_->setFocus();

  layoutQuery->elementAt(2,0)->addWidget( new WBreak() );
  layoutQuery->elementAt(3,0)->addWidget(bQuery_ = new WPushButton("Query Devices")); 	// 'Query Devices' button

  // Trigger readLspciList() function on 'Query Devices' click.
  bQuery_->clicked().connect(this, &PcimapQueryWidget::readLspciList);

}



PcimapQueryWidget* PcimapQueryWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // we didn't receive a parent and there is no instance of PcimapQueryWidget, so return NULL..
	  return NULL;
	}
  }
  else{							// We received a parent container
	if( !instance_ ){
	  instance_ = new PcimapQueryWidget(parent);
	}
	// else don't care, we already have an instance we will return that.
  }
  return instance_;
}



void PcimapQueryWidget::readLspciList()
{
  destroyLspciList();			// Destroy previous list!
  
  char column_count;			// Holding the column number we are currently reading in 'lspci -mn' output.
  
  
  std::string pcimapListContent = (pcimapList_->text()).narrow(); // Put contents of Text area in a string,
  std::stringstream ss( pcimapListContent );					  // and convert it to a stream.

  char lspciLine[53];
  while( ss.getline(lspciLine, 53) ){ 					// Read each line of 'lspci -mn' output

	std::istringstream s_in(lspciLine); 				// Convert the line into a stream.
	std::string get_s;

	PciDevice *pci_dev = new PciDevice();
	
	column_count = 0;
	while ( s_in >> get_s ) {							// Read each column of current line
	  
	  switch(column_count) {
	  case 0:
		// std::cout << get_s; // @test
		break;
		
	  case 1:
		pci_dev->setClass( get_s.substr(1,2) ); 		// Separate Class and Subclass info from Class column.
		pci_dev->setSubclass( get_s.substr(3,2) );
		break;
		
	  case 2:
		pci_dev->setVendor( get_s.substr(1,4) );
		break;
		
	  case 3:
		pci_dev->setDevice( get_s.substr(1,4) );
		break;
		
	  case 4:
		// std::cout << "rev: " + get_s; // @test
		
		if( get_s.at(1) == 'p' ){ 		// there might also be Progif info instead of Revision in this column.
		  pci_dev->setProgif( get_s.substr(2,2) );
		  ++column_count;				// if so, skip to the SubVendor column.
		}
		break;
		
	  case 5:
		// This column might hold Progif or Subvendor info, so act accordingly
		if( get_s.at(1) == 'p' ){
		  pci_dev->setProgif( get_s.substr(2,2) );
		}
		else {							// Prog-if is empty, so there should be the Subvendor column.
		  if ( get_s != "\"\"" ){
			pci_dev->setSubvendor( get_s.substr(1,4) );
			++column_count;
		  } // else subvendor code is empty..
		}
		break;
		
	  case 6:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubvendor( get_s.substr(1,4) );
		} // else subvendor code is empty..
		break;
		
	  case 7:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubdevice( get_s.substr(1,4) );
		} // else subdevice code is empty..
		break;
		
	  }
	  ++column_count;
	}
	lspci_list_.push_back(*pci_dev); 	// Populate the device list with the line read.
	delete pci_dev;
  }

  // Now lets switch to the PcimapResultWidget (Query Devices Result page).
  PcimapResultWidget* qresult;
  if ( (qresult = PcimapResultWidget::Instance()) == NULL ) 			// It should not exist!
	// so add it to StackedWidget and select, pass the PCI Device list as parameter.
	selectWidget( PcimapResultWidget::Instance(lspci_list_,parent_) );
  else
	WApplication::instance()->log("debug") << "PcimapResultWidget already exists! this should NOT happen!";

  pcimapList_->setText("");
  WApplication::instance()->setInternalPath("/query_results");

}


std::vector<PciDevice>& PcimapQueryWidget::getLspciList()
{
  return lspci_list_;
}


void PcimapQueryWidget::destroyLspciList()
{
  int lspci_list_size = lspci_list_.size();
  for ( int k=0; k<lspci_list_size; ++k )
  	lspci_list_.pop_back();
}


void PcimapQueryWidget::resetAll()
{
  pcimapList_->setText("");
  destroyLspciList();			// Clear the temporarily stored PCI Device list.
}
