#if !defined( _HARIXAPP_ )
#define _HARIXAPP_

#include <Wt/WApplication>

class QueryResult;

class HarixApp : public Wt::WApplication
{
private:
  Wt::WStackedWidget* stackedContent_;

public:
  HarixApp( const Wt::WEnvironment& env );

  void internalPathChanged(const std::string& path);
  
  void setWidget( Wt::WContainerWidget* widget);
  void removeWidget( Wt::WContainerWidget* widget );

};

#endif // _HARIXAPP_
