
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
// #include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WTextArea>

#include "pci_device.hpp"

using namespace Wt;
using std::vector;

void destroy_lspci_list(void);

vector<PciDevice> lspci_list;

// ------------ TEST ------------
class QueryResult : public WContainerWidget
{
private:
  WTextArea* test_query;		// @test
public:
  QueryResult( WContainerWidget* parent );
  
};

QueryResult::QueryResult( WContainerWidget* parent = 0 ) : WContainerWidget(parent)
{
  test_query = new WTextArea("Results Page", parent); // @test
  test_query->setColumns(60);		  // @test
  test_query->setFocus();
  
}
// ------------- TEST -----------

class PcimapQueryApp : public WApplication
{
private:
  WTextArea* pcimapList_;
  WPushButton* bQuery_;

  
public:
  PcimapQueryApp(const WEnvironment& env);
  ~PcimapQueryApp();

  void getLspciList();
};

PcimapQueryApp::PcimapQueryApp(const WEnvironment& env) : WApplication(env)
{
  setTitle("Query Pcimaps");                               // application title

  pcimapList_ = new WTextArea(root());
  pcimapList_->setColumns(60);
  // pcimapList_->setRows(10);
  pcimapList_->setFocus();



  // root()->addWidget(new WBreak()); // requires WContainerWidget to be included
  new WBreak(root());

  bQuery_ = new WPushButton("Query Devices", root()); // create query button

  bQuery_->clicked().connect(this, &PcimapQueryApp::getLspciList);

}

void PcimapQueryApp::getLspciList()
{
  destroy_lspci_list();			// DESTROY PREVIOUS LIST!!
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
		// test_query->setText(test_query->text() +"Class: "+ pci_dev->getClass() + pci_dev->getSubclass() +"\n"); // @test
		break;
	  case 2:
		pci_dev->setVendor( get_s.substr(1,4) );
		// test_query->setText( test_query->text() + "vendor: " + pci_dev->getVendor() + "\n"  ); // @test
		break;
	  case 3:
		pci_dev->setDevice( get_s.substr(1,4) );
		// test_query->setText( test_query->text() + "device: " + pci_dev->getDevice() + "\n"  ); // @test
		break;
	  case 4:
		// test_query->setText( test_query->text() + "rev: " + get_s + "\n"  ); // @test
		break;
	  case 5:
		if( get_s.at(1) == 'p' ){
		  pci_dev->setProgif( get_s.substr(2,2) );
		  // test_query->setText( test_query->text() + "progif: " + pci_dev->getProgif() + "\n"  ); // @test
		}
		else {					// Prog-if is empty...
		  if ( get_s != "\"\"" ){
			pci_dev->setSubvendor( get_s.substr(1,4) );
			// test_query->setText( test_query->text() + "subvendor: " + pci_dev->getSubvendor() + "\n"); // @test
			++coloum_count;
		  } // else subvendor is empty..
		}
		break;
	  case 6:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubvendor( get_s.substr(1,4) );
		  // test_query->setText( test_query->text() + "subvendor: " + pci_dev->getSubvendor() + "\n"  ); // @test
		} // else subvendor is empty..
		break;
	  case 7:
		if ( get_s != "\"\"" ){
		  pci_dev->setSubdevice( get_s.substr(1,4) );
		  // test_query->setText( test_query->text() + "subdevice: " + pci_dev->getSubdevice() + "\n"  ); // @test
		} // else subdevice is empty..
		break;
	  }
	  ++coloum_count;
	}
	lspci_list.push_back(*pci_dev);
	delete pci_dev;
  }

  // @TODO: BASKA BI FONKDA LSPCI_LISTDEN VERILERI ALARAK, TABLOYU DOLDUR..
  // SONRA DETAILS TUSU ILE HANGISI NE TARAFINDAN DESTEKLENIYO SORGULARSIN..
  root()->clear();
  new QueryResult(root());


}

void destroy_lspci_list(void)
{
  int lspci_list_size = lspci_list.size();
  for ( int k=0; k<lspci_list_size; ++k )
	lspci_list.pop_back();
}

PcimapQueryApp::~PcimapQueryApp()
{
  destroy_lspci_list();
}


WApplication *createApplication(const WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */
  return new PcimapQueryApp(env);
}

int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  return WRun(argc, argv, &createApplication);
}







