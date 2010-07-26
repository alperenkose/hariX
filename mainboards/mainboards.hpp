#if !defined( _MAINBOARDS_ )
#define  _MAINBOARDS_

#include <Wt/WContainerWidget>

namespace Wt {
  class WPanel;
  class WStandardItemModel;
  class WSortFilterProxyModel;
  class WDialog;
}

class PciDevice;

class MainboardsWidget : public Wt::WContainerWidget
{
private:
  MainboardsWidget( Wt::WContainerWidget* parent);
  ~MainboardsWidget();
  static MainboardsWidget* instance_;
  Wt::WContainerWidget* parent_;

  // Wt::WPanel* panelMainboard_;

  std::multimap<std::string, std::string> mainboard_list; // ( boardID, boardName )
  Wt::WStandardItemModel* mainboards;
  Wt::WSortFilterProxyModel *boardsProxyModel;
  Wt::WSelectionBox *boardsSelection_;
  void fillMainboards( Wt::WStandardItemModel& boardsModel );
  void bBoardDevices_Click();
  void bDeleteBoard_Click();
  void deleteBoard();
  Wt::WDialog *dialogWarn_;
  void dialogWarn_Close( Wt::WDialog::DialogCode code );
  void refreshPage();

  void showBoardConfiguration( std::string board_id  );
  std::vector<PciDevice> getBoardDeviceList( std::string board_id  );

  Wt::WComboBox *comboOS_, *comboDist_, *comboKer_;
  Wt::WStandardItemModel *osModel_, *releaseModel_, *kernelModel_;
  
  std::multimap<std::string,std::string> osList_; // @TODO: Check if we need this public???
  std::multimap<std::string,std::string> releaseList_; // @TODO: Check if we need this public???
  std::multimap<std::string,std::string> kernelList_; // @TODO: Check if we need this public???
  
  void fillOsModel();
  void fillReleaseModel( std::string os_id );
  void fillKernelModel( std::string release_id );
  
  void osSelectionChanged( int selected_index ); 		// Signal..
  void releaseSelectionChanged( int selected_index );	// Signal..
  Wt::WPushButton *bCheckOs_;
  void bCheckOs_Click();

  std::string getBoardIdentifier(); // Get the boardID of the selected board in SelectionBox!
  std::string getOsIdentifier(); // Get the selected uKernelID from the ComboBox selections!
  
  Wt::WText* testText_;
public:
  static MainboardsWidget* Instance( WContainerWidget* parent = 0 );
  void resetAll();

};


#endif //  _MAINBOARDS_
