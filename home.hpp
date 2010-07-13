#if !defined ( _HOME_ )
#define  _HOME_

#include <Wt/WContainerWidget>
#include <Wt/WDialog>

namespace Wt {
  class WOverlayLoadingIndicator;
}

class HomeWidget : public Wt::WContainerWidget
{
private:
  HomeWidget( Wt::WContainerWidget* parent);
  static HomeWidget* instance_;
  
  Wt::WPushButton* bAnalyze_, *bQueryLspci_, *bListBoards_, *bPciIds_;
  Wt::WContainerWidget* parent_;
  void bAnalyze_Click();
  void bQueryLspci_Click();
  void bListBoards_Click();
  void bPciIds_Click();

  Wt::WDialog *dialogPciIds_;
  Wt::WDialog *dialogDownload_;
  Wt::WOverlayLoadingIndicator *loading_;
  void dialogPciEnd( Wt::WDialog::DialogCode code );
  void dialogDownloadEnd( Wt::WDialog::DialogCode code );
public:
  static HomeWidget* Instance( WContainerWidget* parent = 0 );
};


#endif // _HOME_


