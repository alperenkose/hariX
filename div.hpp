/*!
  \file   div.hpp
  \date   Wed Jul 28 16:58:31 2010
  
  \brief  Div Class Implementation
  
*/
#if !defined ( _DIV_ )
#define _DIV_

#include <Wt/WContainerWidget>

//! HTML \<div\> implementation.
/*!
  Shortcut for a \<div class=""\> html element used through out the application.
*/
class Div : public Wt::WContainerWidget
{
public:

  //! Constructor.
  /*!
	Creates an html \<div\> element with the provided CSS style class name.
	
	\param parent Parent container
	\param style_class Name of a CSS style class
  */
  Div(WContainerWidget *parent, const std::string& style_class)
    : WContainerWidget(parent)
  {
	setStyleClass(style_class);
  }
};

#endif // _DIV_
