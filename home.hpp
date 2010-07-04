#if !defined ( _HOME_ )
#define  _HOME_

#include <Wt/WContainerWidget>

class HomeWidget : public Wt::WContainerWidget
{
private:
  HomeWidget( Wt::WContainerWidget* parent);
  static HomeWidget* instance_;
  
  Wt::WPushButton* bAnalyze_, *bQueryLspci_, *bListBoards_;
  Wt::WContainerWidget* parent_;
  void bAnalyze_Click();
  void bQueryLspci_Click();
  void bListBoards_Click();
  
public:
  static HomeWidget* Instance( WContainerWidget* parent = 0 );
};


#endif // _HOME_


