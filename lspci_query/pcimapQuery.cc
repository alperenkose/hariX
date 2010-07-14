
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WPushButton>
#include <Wt/WTextArea>
#include <Wt/WPanel>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <string>

#include "../harixApp.hpp"		// needed for WApplication::instance()
#include "pcimapQuery.hpp"
#include "../pci_device.hpp"
#include "pcimapResult.hpp"
#include "../home.hpp"
#include "../div.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

PcimapQueryWidget* PcimapQueryWidget::instance_ = NULL;
std::vector<PciDevice> PcimapQueryWidget::lspci_list;

PcimapQueryWidget::PcimapQueryWidget( WContainerWidget* parent ) : WContainerWidget(parent)
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

  Div *page = new Div(content, "pagearea");
  

  
  // setTitle("Query Pcimaps");                               // application title
  // WPanel *panelQuery;
  // // panelQuery = new WPanel(this);
  // page->addWidget( panelQuery = new WPanel() );
  // panelQuery->resize(470, WLength());
  WTable* layoutQuery;
  // panelQuery->setCentralWidget( layoutQuery = new WTable() );
  page->addWidget( layoutQuery = new WTable() );

  layoutQuery->elementAt(0,0)->setColumnSpan(2);
  layoutQuery->elementAt(0,0)->addWidget( new WText("Paste below the output of `lspci -mn`:") );
  
  layoutQuery->elementAt(1,0)->setColumnSpan(2);
  layoutQuery->elementAt(1,0)->setContentAlignment(AlignCenter);
  layoutQuery->elementAt(1,0)->addWidget(pcimapList_ = new WTextArea());
  // pcimapList_->setColumns(60);
  pcimapList_->resize(450, WLength());
  pcimapList_->setFocus();

  layoutQuery->elementAt(2,0)->addWidget( new WBreak() );
  layoutQuery->elementAt(3,0)->addWidget(bQuery_ = new WPushButton("Query Devices")); 	// create query button

  bQuery_->clicked().connect(this, &PcimapQueryWidget::readLspciList);

  // layoutQuery->elementAt(3,1)->setContentAlignment(AlignRight);
  // layoutQuery->elementAt(3,1)->addWidget( bGoHome_ = new WPushButton("Go Home"));  
  // bGoHome_->clicked().connect(this, &PcimapQueryWidget::bGoHome_Click);

}

PcimapQueryWidget* PcimapQueryWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	if ( !instance_ ){
	  // wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	if( !instance_ ){
	  instance_ = new PcimapQueryWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

PcimapQueryWidget::~PcimapQueryWidget()
{
  destroyLspciList();
}


void PcimapQueryWidget::readLspciList()
{
  destroyLspciList();			// DESTROY PREVIOUS LIST!!
  char coloum_count;
  char lspciLine[53];
  std::string pcimapListContent = (pcimapList_->text()).narrow();
  std::stringstream ss( pcimapListContent );
  
  while( ss.getline(lspciLine, 53) ){
	// test_query->setText( lspciLine ); // @test
	std::istringstream s_in(lspciLine);
	std::string get_s;

	PciDevice *pci_dev = new PciDevice();
	coloum_count = 0;
	while ( s_in >> get_s ) {
	  switch(coloum_count) {
	  case 0:
		// test_query->setText( get_s + "\n" ); // @test
		break;
	  case 1:
		pci_dev->setClass( get_s.substr(1,2) );
		pci_dev->setSubclass( get_s.substr(3,2) );
		break;
	  case 2:
		pci_dev->setVendor( get_s.substr(1,4) );
		break;
	  case 3:
		pci_dev->setDevice( get_s.substr(1,4) );
		break;
	  case 4:
		// test_query->setText( test_query->text() + "rev: " + get_s + "\n"  ); // @test
		if( get_s.at(1) == 'p' ){
		  pci_dev->setProgif( get_s.substr(2,2) );
		  ++coloum_count;
		}
		break;
	  case 5:
		if( get_s.at(1) == 'p' ){
		  pci_dev->setProgif( get_s.substr(2,2) );
		}
		else {					// Prog-if is empty...
		  if ( get_s != "\"\"" ){
			pci_dev->setSubvendor( get_s.substr(1,4) );
			++coloum_count;
		  } // else subvendor is empty..
		}
		break;
	  case 6:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubvendor( get_s.substr(1,4) );
		} // else subvendor is empty..
		break;
	  case 7:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubdevice( get_s.substr(1,4) );
		} // else subdevice is empty..
		break;
	  }
	  ++coloum_count;
	}
	lspci_list.push_back(*pci_dev);
	delete pci_dev;
  }

  PcimapResultWidget* qresult;
  if ( (qresult = PcimapResultWidget::Instance()) == NULL )
	selectWidget( PcimapResultWidget::Instance(lspci_list,parent_) ); // Add widget to StackedWidget and select..
  else
	selectWidget( qresult );

  pcimapList_->setText("");
  WApplication::instance()->setInternalPath("/query_results");

}

std::vector<PciDevice>& PcimapQueryWidget::getLspciList()
{
  return lspci_list;
}

void PcimapQueryWidget::destroyLspciList()
{
  // lspci_list.clear();
  int lspci_list_size = lspci_list.size();
  for ( int k=0; k<lspci_list_size; ++k )
  	lspci_list.pop_back();
}


void PcimapQueryWidget::resetAll()
{
  pcimapList_->setText("");
  destroyLspciList();
}
