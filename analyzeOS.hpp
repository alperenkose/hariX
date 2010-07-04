// Hey Emacs -*- C++ -*-

#if !defined( _ANALYZEOS_ )
#define _ANALYZEOS_

#include <Wt/WContainerWidget>

namespace Wt {
  class WPanel;
  class WTable;
}

class AnalyzeOsWidget : public Wt::WContainerWidget
{
private:
  AnalyzeOsWidget( Wt::WContainerWidget* parent);
  static AnalyzeOsWidget* instance_;
  
  Wt::WContainerWidget* parent_;

  Wt::WPanel* panelAnalyze_;
  Wt::WTable* layoutAnalyze_;

  Wt::WFileUpload* uploadOs_, *uploadPcimap_;
  Wt::WPushButton* bUpload_;

  Wt::WPushButton* bGoHome_;
  void bGoHome_Click();

  void osUploaded();
  void pcimapUploaded();

  Wt::WText* os_file, *pcimap_file; // @test
  
public:
  static AnalyzeOsWidget* Instance( WContainerWidget* parent = 0 );

};

#endif	// _ANALYZEOS_
