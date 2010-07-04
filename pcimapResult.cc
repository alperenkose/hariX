#include <Wt/WTextArea>
#include <Wt/WLogger>

#include "harixApp.hpp"			// JUST FOR wApp!!
#include "pcimapResult.hpp"
#include "pcimapQuery.hpp"
#include "pci_device.hpp"

using namespace Wt;

PcimapResultWidget* PcimapResultWidget::instance_ = NULL;
int PcimapResultWidget::instance_count = 0;
PcimapResultWidget::PcimapResultWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  // test_query = new Wt::WTextArea("Results Page", parent);
  addWidget( test_query = new WTextArea("Results Page...") );
  test_query->setColumns(60);
  test_query->setFocus();
  ++instance_count;

  // @TODO: COOL SEEMS TO WORK!! CONTINUE HERE!!
  lspci_list = PcimapQueryWidget::getLspciList();
  
  // @TODO: LSPCI_LISTDEN VERILERI ALARAK, TABLOYU DOLDUR..
  // SONRA DETAILS TUSU ILE HANGISI NE TARAFINDAN DESTEKLENIYO SORGULARSIN..

}

PcimapResultWidget* PcimapResultWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	wApp->log("debug") << "parent is 0!"; 					// @test
	if ( !instance_ ){
	  wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	wApp->log("debug") << "received parent!"; 				// @test
	if( !instance_ ){
	  instance_ = new PcimapResultWidget(parent);
	  wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}
