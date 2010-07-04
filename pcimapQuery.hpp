// Hey Emacs -*- C++ -*-

#if !defined( _PCIMAPQUERY_ )
#define _PCIMAPQUERY_

#include <Wt/WContainerWidget>
// #include "pci_device.hpp"
class PciDevice;

class PcimapQueryWidget : public Wt::WContainerWidget
{
private:
  Wt::WTextArea* pcimapList_;
  Wt::WPushButton* bQuery_;
  Wt::WContainerWidget* parent_;

  std::vector<PciDevice> lspci_list;	// @TODO: change all occurences to lspci_list_

  
public:
  PcimapQueryWidget( Wt::WContainerWidget* parent);
  ~PcimapQueryWidget();

  void getLspciList();
  void destroy_lspci_list();
};

// ------------ TEST ------------
class QueryResult : public Wt::WContainerWidget
{
private:
  QueryResult( WContainerWidget* parent );
  static QueryResult* instance_;
  Wt::WTextArea* test_query;		// @test
public:
  static QueryResult* Instance( WContainerWidget* parent = 0 );

};

// ------------- TEST -----------


#endif	// _PCIMAPQUERY_
