#include <Wt/WStackedWidget>

#include "harixApp.hpp"
#include "pcimapQuery.hpp"

using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");

  // Common stack to hold any pages..
  content_ = new WStackedWidget(root());
  
  PcimapQueryWidget* lspci_query = new PcimapQueryWidget();
  content_->addWidget( lspci_query  );
  content_->setCurrentWidget( lspci_query );

  // @TODO: public static HarixApp::addWidget, setCurrentWidget fonk.lari
  // koyarak, sayfa degistiriyo gibi yapabilirsin..

}
