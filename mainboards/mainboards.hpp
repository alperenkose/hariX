#if !defined( _MAINBOARDS_ )
#define  _MAINBOARDS_

#include <Wt/WContainerWidget>

namespace Wt {
  class WPanel;
  class WStandardItemModel;
  class WSortFilterProxyModel;
}

class MainboardsWidget : public Wt::WContainerWidget
{
private:
  MainboardsWidget( Wt::WContainerWidget* parent);
  ~MainboardsWidget();
  static MainboardsWidget* instance_;
  Wt::WContainerWidget* parent_;

  Wt::WPanel* panelMainboard_;

  std::multimap<std::string, std::string> mainboard_list; // ( boardID, boardName )
  Wt::WStandardItemModel* mainboards;
  Wt::WSortFilterProxyModel *boardsProxyModel;
  Wt::WSelectionBox *boardsSelection_;
  void fillMainboards( Wt::WStandardItemModel& boardsModel );
  void bBoardDevices_Click();

  void showBoardConfiguration( std::string board_id  );
  
  Wt::WPushButton *bGoHome_;
  void bGoHome_Click();

  Wt::WText* testText_;
public:
  static MainboardsWidget* Instance( WContainerWidget* parent = 0 );

};


#endif //  _MAINBOARDS_
