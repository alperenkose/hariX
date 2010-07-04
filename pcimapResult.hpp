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
  Wt::WTextArea* test_query;		// @test
  std::vector<PciDevice> lspci_list;
public:
  static PcimapResultWidget* Instance( WContainerWidget* parent = 0 );
  
};



#endif // _PCIMAPRESULT_
