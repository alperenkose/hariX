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

  std::vector<PciDevice> lspci_list;	// @TODO: change all occurences to lspci_list_

  
public:
  PcimapQueryWidget( Wt::WContainerWidget* parent = 0 );
  ~PcimapQueryWidget();

  void getLspciList();
  void destroy_lspci_list();
};

#endif	// _PCIMAPQUERY_
