#if !defined ( _PCIMAPRESULT_ )
#define _PCIMAPRESULT_

#include <Wt/WContainerWidget>

namespace Wt{
  class WPanel;
  class WStandardItemModel;
  class WStandardItem;
  class WTableView;
  class WModelIndex;
}

class PciDevice;

class PcimapResultWidget : public Wt::WContainerWidget
{
private:
  PcimapResultWidget( WContainerWidget* parent );
  ~PcimapResultWidget();

  static PcimapResultWidget* instance_;
  static int instance_count;
  
  std::vector<PciDevice> lspci_list;

  Wt::WPanel* panelTable_;
  Wt::WStandardItemModel* lspciModel_;
  Wt::WTableView* lspciTable_;

  Wt::WPanel* panelSupport_;
  Wt::WTable* layoutSupport_;
  Wt::WLineEdit *editVendor_,*editDevice_,*editSubvendor_,*editSubdevice_,*editClass_,*editSubclass_,*editProgif_;
  Wt::WStandardItemModel* osSupportModel_;
  Wt::WTableView* osSupportTable_;

  // static void fillDeviceList();
  void lspciTableRowSelected(Wt::WModelIndex, Wt::WMouseEvent);
  std::vector<Wt::WStandardItem*> pciDeviceToRowItem( const PciDevice* current_item );
  std::vector<Wt::WStandardItem*> osToSupportRow( const std::vector<std::string>& os );
  void fillDeviceDetails( const PciDevice& selected_device );

  void bGoHome_Click();
public:
  static PcimapResultWidget* Instance( WContainerWidget* parent = 0 );
  
};



#endif // _PCIMAPRESULT_
