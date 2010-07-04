#if !defined ( _PCIMAPRESULT_ )
#define _PCIMAPRESULT_

#include <Wt/WContainerWidget>

class PciDevice;

class PcimapResultWidget : public Wt::WContainerWidget
{
private:
  PcimapResultWidget( WContainerWidget* parent );
  static PcimapResultWidget* instance_;
  static int instance_count;
  
  std::vector<PciDevice> lspci_list;
  
  Wt::WStandardItemModel* lspciModel_;
  Wt::WTableView* lspciTable_;
  Wt::WPanel* panel;
public:
  static PcimapResultWidget* Instance( WContainerWidget* parent = 0 );
  
};



#endif // _PCIMAPRESULT_
