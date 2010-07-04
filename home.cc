#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>

#include "home.hpp"
#include "analyze_os/analyzeOS.hpp"
#include "lspci_query/pcimapQuery.hpp"

using namespace Wt;

void selectWidget( WContainerWidget* widget );

HomeWidget* HomeWidget::instance_ = NULL;

HomeWidget::HomeWidget( WContainerWidget* parent ) : WContainerWidget(parent)
{
  parent_ = parent;

  addWidget( bAnalyze_ = new WPushButton("Analyze Client OS") ); 	// 
  bAnalyze_->clicked().connect(this, &HomeWidget::bAnalyze_Click);

  addWidget( new WBreak() );

  addWidget( bQueryLspci_ = new WPushButton("Query lspci list") );
  bQueryLspci_->clicked().connect(this,  &HomeWidget::bQueryLspci_Click);



}

HomeWidget* HomeWidget::Instance( WContainerWidget* parent)
{
  if( parent == 0 ){
	// wApp->log("debug") << "parent is 0!"; 					// @test
	if ( !instance_ ){
	  // wApp->log("debug") << "no instance return NULL!!"; 	// @test
	  return NULL;
	}
  }
  else{
	// wApp->log("debug") << "received parent!"; 				// @test
	if( !instance_ ){
	  instance_ = new HomeWidget(parent);
	  // wApp->log("debug") << "Object initiated!!"; 			// @test
	}
  }
  // wApp->log("debug") << "return instance, No. " << instance_count; // @test
  return instance_;

}

void HomeWidget::bAnalyze_Click()
{
  AnalyzeOsWidget* analyze_page;
  if ( ( analyze_page = AnalyzeOsWidget::Instance()) == NULL )
	selectWidget( AnalyzeOsWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( analyze_page );

  
}

void HomeWidget::bQueryLspci_Click()
{
  PcimapQueryWidget* pcimapquery_page;
  if ( (pcimapquery_page = PcimapQueryWidget::Instance()) == NULL )
	selectWidget( PcimapQueryWidget::Instance(parent_) ); // Add widget to StackedWidget and select it..
  else
	selectWidget( pcimapquery_page );
}
