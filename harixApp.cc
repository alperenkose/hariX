#include <Wt/WStackedWidget>

#include "harixApp.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "home.hpp"

using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");

  // Common stack to hold any pages..
  content_ = new WStackedWidget(root());
  
  PcimapQueryWidget* lspci_query = PcimapQueryWidget::Instance(content_);
  // content_->setCurrentWidget( lspci_query );

  HomeWidget* home_page = HomeWidget::Instance(content_);
  content_->setCurrentWidget( home_page );
}

void HarixApp::setWidget( WContainerWidget* widget )
{
  // if( content_->indexOf(widget) == -1  )
  // 	content_->addWidget( widget );
  content_->setCurrentWidget( widget );
}

void HarixApp::removeWidget( WContainerWidget* widget )
{
  // if( content_->indexOf(widget) == -1  )
  // 	content_->addWidget( widget );
  content_->removeWidget( widget );
}


void selectWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->setWidget(widget);
}

void removeWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->removeWidget(widget);
}
