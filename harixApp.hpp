#if !defined( _HARIXAPP_ )
#define _HARIXAPP_

#include <Wt/WApplication>

class HarixApp : public Wt::WApplication
{
private:
  Wt::WStackedWidget* content_;

public:
  HarixApp( const Wt::WEnvironment& env );
};

#endif // _HARIXAPP_
