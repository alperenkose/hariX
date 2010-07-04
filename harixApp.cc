#include <Wt/WStackedWidget>

#include "harixApp.hpp"
#include "pcimapQuery.hpp"

using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");

  // Common stack to hold any pages..
  content_ = new WStackedWidget(root());
  
  PcimapQueryWidget* lspci_query = new PcimapQueryWidget(content_);
  // content_->addWidget( lspci_query  );
  content_->setCurrentWidget( lspci_query );

  // QueryResult* the_result = QueryResult::Instance(content_);
  // content_->setCurrentWidget( the_result );

}

void HarixApp::setWidget( WContainerWidget* widget )
{
  // if( content_->indexOf(widget) == -1  )
	// content_->addWidget( widget );
  content_->setCurrentWidget( widget );
}

void selectWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->setWidget(widget);
}

