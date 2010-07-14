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
  
  Wt::WPushButton *bPciIds_;
  Wt::WContainerWidget* parent_;
  void bPciIds_Click();

  Wt::WDialog *dialogPciIds_;
  Wt::WDialog *dialogDownload_;
  Wt::WDialog *dialogWarn_;
  Wt::WOverlayLoadingIndicator *loading_;
  void dialogPci_Close( Wt::WDialog::DialogCode code );
  void dialogDownload_Close( Wt::WDialog::DialogCode code );
  void dialogWarn_Close( Wt::WDialog::DialogCode code );

  void readProxyFile();
  std::string settingsProxy_;
  Wt::WLineEdit* editProxy_;
  void changeProxy();

public:
  static HomeWidget* Instance( WContainerWidget* parent = 0 );
};


#endif // _HOME_


