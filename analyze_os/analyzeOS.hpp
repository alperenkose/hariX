// Hey Emacs -*- C++ -*-

#if !defined( _ANALYZEOS_ )
#define _ANALYZEOS_

#include <Wt/WContainerWidget>

namespace Wt {
  class WPanel;
  class WTable;
}

class OsInfo;

class AnalyzeOsWidget : public Wt::WContainerWidget
{
private:
  AnalyzeOsWidget( Wt::WContainerWidget* parent);
  static AnalyzeOsWidget* instance_;

  OsInfo* osDist_;
  
  Wt::WContainerWidget* parent_;

  Wt::WPanel* panelAnalyze_;
  Wt::WTable* layoutAnalyze_;
  Wt::WPanel* panelAnalyzeResult_;
  Wt::WGroupBox* groupDetectedOs_;
  Wt::WLineEdit *editOs_, *editRel_, *editKer_, *editArch_;
  Wt::WPushButton *bCheckOs_;
  void bCheckOs_Click();

  Wt::WTable* layoutCheckResult_;

  Wt::WText* txtOsResult_;
  Wt::WPushButton *bOsAddUpdate_, *bOsCancel_;

  Wt::WFileUpload* uploadOs_, *uploadPcimap_;
  Wt::WPushButton* bUpload_;
  void bUpload_Click();

  Wt::WPushButton* bGoHome_;
  void bGoHome_Click();

  static bool isOsUploaded_, isPcimapUploaded_;
  void osUploaded();
  void pcimapUploaded();
  void uploadDone();

  void readOs();
  void fillDetectedOs();

  void resetAll();
  std::string os_file_, pcimap_file_; // @test
  
public:
  static AnalyzeOsWidget* Instance( WContainerWidget* parent = 0 );

};

#endif	// _ANALYZEOS_
