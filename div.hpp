#if !defined ( _DIV_ )
#define _DIV_

#include <Wt/WContainerWidget>

/* Shortcut for a <div class=""> */
class Div : public Wt::WContainerWidget
{
public:
  Div(WContainerWidget *parent, const std::string& style_class)
    : WContainerWidget(parent)
  {
	setStyleClass(style_class);
  }
};

#endif // _DIV_
