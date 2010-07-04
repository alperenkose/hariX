#if !defined( _HARIXAPP_ )
#define _HARIXAPP_

#include <Wt/WApplication>

class QueryResult;

class HarixApp : public Wt::WApplication
{
private:
  Wt::WStackedWidget* content_;

public:
  HarixApp( const Wt::WEnvironment& env );

  void setWidget( Wt::WContainerWidget* widget);

};

#endif // _HARIXAPP_
