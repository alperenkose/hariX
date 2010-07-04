
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WPushButton>
#include <Wt/WTextArea>

#include "pcimapQuery.hpp"
#include "../pci_device.hpp"
#include "pcimapResult.hpp"
#include "../home.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

PcimapQueryWidget* PcimapQueryWidget::instance_ = NULL;
std::vector<PciDevice> PcimapQueryWidget::lspci_list;

PcimapQueryWidget::PcimapQueryWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;
  // setTitle("Query Pcimaps");                               // application title


  addWidget(pcimapList_ = new WTextArea());
  pcimapList_->setColumns(60);
  pcimapList_->setFocus();

  addWidget(new WBreak());

  addWidget(bQuery_ = new WPushButton("Query Devices")); 	// create query button

  bQuery_->clicked().connect(this, &PcimapQueryWidget::readLspciList);

  addWidget(new WBreak());

  addWidget( bGoHome_ = new WPushButton("Go Home"));  
  bGoHome_->clicked().connect(this, &PcimapQueryWidget::bGoHome_Click);

}

PcimapQueryWidget* PcimapQueryWidget::Instance( WContainerWidget* parent)
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
	selectWidget( PcimapResultWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( qresult );

}

std::vector<PciDevice>& PcimapQueryWidget::getLspciList()
{
  return lspci_list;
}

void PcimapQueryWidget::destroyLspciList()
{
  int lspci_list_size = lspci_list.size();
  for ( int k=0; k<lspci_list_size; ++k )
	lspci_list.pop_back();
}


void PcimapQueryWidget::bGoHome_Click()
{
  HomeWidget* home_page;
  if ( (home_page = HomeWidget::Instance()) == NULL )
	selectWidget( HomeWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( home_page );
  
}
