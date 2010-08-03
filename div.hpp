/* div.hpp --- 
 *
 * Copyright (C) 2010 Alp Eren Köse
 *
 * Author: Alp Eren Köse <alperenkose@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
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
