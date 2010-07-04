// Hey Emacs -*- C++ -*-

#if !defined( _PCIMAPQUERY_ )
#define _PCIMAPQUERY_

#include <Wt/WContainerWidget>

class PciDevice;

class PcimapQueryWidget : public Wt::WContainerWidget
{
private:
  PcimapQueryWidget( Wt::WContainerWidget* parent);
  static PcimapQueryWidget* instance_;
  
  Wt::WTextArea* pcimapList_;
  Wt::WPushButton* bQuery_;
  Wt::WContainerWidget* parent_;

  static std::vector<PciDevice> lspci_list;	// @TODO: change all occurences to lspci_list_

  void readLspciList();
  void destroyLspciList();
  
public:
  ~PcimapQueryWidget();

  static PcimapQueryWidget* Instance( WContainerWidget* parent = 0 );
  static std::vector<PciDevice>& getLspciList();

};

#endif	// _PCIMAPQUERY_
