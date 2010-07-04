#include <Wt/WTextArea>
#include <Wt/WPanel>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
#include <Wt/WTableView>
#include <Wt/WLogger>

#include "harixApp.hpp"			// JUST FOR wApp!!
#include "pcimapResult.hpp"
#include "pcimapQuery.hpp"
#include "pci_device.hpp"

using namespace Wt;

std::vector<WStandardItem *>
rowItems(const std::string& classname,
		 const std::string& vendor,
		 const std::string& device);


PcimapResultWidget* PcimapResultWidget::instance_ = NULL;
int PcimapResultWidget::instance_count = 0;
PcimapResultWidget::PcimapResultWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  ++instance_count;

  lspci_list = PcimapQueryWidget::getLspciList();

  lspciModel_ = new WStandardItemModel( 0, 3, this );
  lspciModel_->setHeaderData(0, Horizontal, std::string("Class"));
  lspciModel_->setHeaderData(1, Horizontal, std::string("Vendor"));
  lspciModel_->setHeaderData(2, Horizontal, std::string("Device"));
  lspciModel_->appendRow( rowItems("0600","8086","29b0") ); // @test
  // lspciModel_->appendRow( rowItems("0604","8086","29b1") ); // @test

  std::vector<PciDevice>::iterator lspci_iter;
  for( lspci_iter = lspci_list.begin(); lspci_iter != lspci_list.end(); ++lspci_iter ){
	// @TODO: LSPCI_LISTDEN VERILERI ALARAK, TABLOYU DOLDUR..
	// SONRA DETAILS TUSU ILE HANGISI NE TARAFINDAN DESTEKLENIYO SORGULARSIN..	
  }

  panel = new WPanel(this);
  panel->resize(600,300);
  panel->setCentralWidget( lspciTable_ = new WTableView() );
  lspciTable_->setAlternatingRowColors(true); // @TODO: Requires CSS

  // lspcitable_->setRowHeight(25);
  lspciTable_->setModel(lspciModel_);

}


std::vector<WStandardItem *>
rowItems(const std::string& classname,
		 const std::string& vendor,
		 const std::string& device)

{
  std::vector<WStandardItem *> result;
  WStandardItem *item;
  
  // column 0: 
  item = new WStandardItem(classname);
  result.push_back(item);

  // column 1: 
  item = new WStandardItem(vendor);
  result.push_back(item);
  
  // column 2: 
  item = new WStandardItem(device);
  result.push_back(item);
  
  return result;
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
