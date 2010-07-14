#include <Wt/WStackedWidget>

#include <Wt/WLogger>

#include "harixApp.hpp"
#include "lspci_query/pcimapQuery.hpp"
#include "lspci_query/pcimapResult.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "mainboards/mainboards.hpp"
#include "home.hpp"

using namespace Wt;

HarixApp::HarixApp( const WEnvironment& env ) : WApplication(env)
{
  setTitle("HariX");
  setCssTheme("polished");
  useStyleSheet("resources/stylizr.css");

  // Common stack to hold any pages..
  stackedContent_ = new WStackedWidget(root());
  
  // PcimapQueryWidget* lspci_query = PcimapQueryWidget::Instance(stackedContent_);
  // stackedContent_->setCurrentWidget( lspci_query );

  HomeWidget* home_page = HomeWidget::Instance(stackedContent_);
  // stackedContent_->setCurrentWidget( home_page );
  WApplication::instance()->setInternalPath("/home",true);

  wApp->internalPathChanged().connect(SLOT(this, HarixApp::internalPathChanged));
}

void HarixApp::setWidget( WContainerWidget* widget )
{
  // if( stackedContent_->indexOf(widget) == -1  )
  // 	stackedContent_->addWidget( widget );
  stackedContent_->setCurrentWidget( widget );
}

void HarixApp::removeWidget( WContainerWidget* widget )
{
  // if( stackedContent_->indexOf(widget) == -1  )
  // 	stackedContent_->addWidget( widget );
  stackedContent_->removeWidget( widget );
}



void selectWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->setWidget(widget);
}

void removeWidget( WContainerWidget* widget )
{
  (dynamic_cast<HarixApp*>(WApplication::instance()))->removeWidget(widget);
}


void HarixApp::internalPathChanged(const std::string& path)
{
  HomeWidget* home_page;
  AnalyzeOsWidget* analyze_page;
  PcimapQueryWidget* querydev_page;
  PcimapResultWidget* queryresult_page;
  MainboardsWidget* mainboards_page;
  if ( (analyze_page = AnalyzeOsWidget::Instance()) != NULL ){
	wApp->log("debug") << "Ok THERE IS AN ANALYZE INSTANCE!!"; 			// @test
	analyze_page->resetAll();
  }
  if ( (querydev_page = PcimapQueryWidget::Instance()) != NULL ){
	wApp->log("debug") << "Ok THERE IS AN QUERY INSTANCE!!"; 			// @test
	querydev_page->resetAll();
  }
  if ( (queryresult_page = PcimapResultWidget::Instance()) != NULL ){
	wApp->log("debug") << "Ok THERE IS A QUERY RESULT INSTANCE!!"; 			// @test
	queryresult_page->resetAll();
	wApp->log("debug") << "QUERY RESULT INSTANCE DELETED!!"; 			// @test
  }
  if ( (mainboards_page = MainboardsWidget::Instance()) != NULL ){
	wApp->log("debug") << "Ok THERE IS A MAINBOARDS INSTANCE!!"; 			// @test
	mainboards_page->resetAll();
	wApp->log("debug") << "MAINBOARDS INSTANCE DELETED!!"; 			// @test
  }
  
  

  if( path == "/home"){
	if ( (home_page = HomeWidget::Instance()) != NULL )
	  selectWidget( home_page );
	else{
	  wApp->log("debug") << "THIS SHOULD NOT HAVE HAPPENED!!"; 			// @test
	  assert(0);					// this should not happen
	}
  }
  else if( path == "/analyze_os"){
  	wApp->log("debug") << "Ok LETS CHANGE to ANALYZE OS!!"; 			// @test
	if ( ( analyze_page = AnalyzeOsWidget::Instance()) == NULL )
	  selectWidget( AnalyzeOsWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it..
	else
	  selectWidget( analyze_page );
  }
  else if( path == "/query_devices"){
	if ( ( querydev_page = PcimapQueryWidget::Instance()) == NULL )
	  selectWidget( PcimapQueryWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it..
	else
	  selectWidget( querydev_page );
  }
  else if( path == "/query_results"){
	// @TODO: redirect to query!
	WApplication::instance()->setInternalPath("/query_devices",true);
  }
  else if( path == "/mainboards"){
	if ( (mainboards_page = MainboardsWidget::Instance()) == NULL )
	  selectWidget( MainboardsWidget::Instance(stackedContent_) ); // Add widget to StackedWidget and select it..
	else
	  selectWidget( mainboards_page );
  }
  
  
}
