#if !defined ( _PCIMAPRESULT_ )
#define _PCIMAPRESULT_

#include <Wt/WContainerWidget>
// #include <Wt/WDialog>

namespace Wt{
  class WPanel;
  class WStandardItemModel;
  class WStandardItem;
  class WTableView;
  class WModelIndex;
  class WDialog;
}

class PciDevice;

class PcimapResultWidget : public Wt::WContainerWidget
{
private:
  PcimapResultWidget( std::vector<PciDevice>& lspci_list, WContainerWidget* parent );
  ~PcimapResultWidget();

  static PcimapResultWidget* instance_;
  static int instance_count;
  
  std::vector<PciDevice> lspci_list_;

  Wt::WPanel* panelTable_;
  Wt::WStandardItemModel* lspciModel_;
  Wt::WTableView* lspciTable_;

  Wt::WPanel* panelSupport_;
  Wt::WTable* layoutSupport_;
  Wt::WLineEdit *editVendor_,*editDevice_,*editSubvendor_,*editSubdevice_,*editClass_,*editSubclass_,*editProgif_;
  Wt::WStandardItemModel* osSupportModel_;
  Wt::WTableView* osSupportTable_;

  void lspciTableRowSelected(Wt::WModelIndex, Wt::WMouseEvent);
  std::vector<Wt::WStandardItem*> pciDeviceToRowItem( const PciDevice* current_item );
  std::vector<Wt::WStandardItem*> osToSupportRow( const std::vector<std::string>& os );
  void fillDeviceDetails( const PciDevice& selected_device );

  Wt::WDialog* dialogBoard_;
  Wt::WLineEdit* editBoard_;
  void dialogBoard_Close( Wt::WDialog::DialogCode dialog_code );
  void bSaveBoard_Click();

  void storeMainBoard( std::string board_name );
  std::vector<std::string> getUniqueDevIdList();
  
  void bGoHome_Click();
public:
  static PcimapResultWidget* Instance( std::vector<PciDevice> lspci_list, WContainerWidget* parent );
  static PcimapResultWidget* Instance();
  
};

#endif // _PCIMAPRESULT_
