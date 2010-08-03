/* main.cc --- 
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
  \file   main.cc
  \date   Wed Jul 14 17:09:38 2010
  
  \brief  Starting point of the application.
  
  Here we run the WApplication Server.
*/
#include <Wt/WApplication>
#include "harixApp.hpp"

//! Creates the WApplication instance.
/*!
  Initiates an HarixApp instance which inherits WApplication.

  The environment provides information on the client, and gives access to startup arguments.
  
  \param env variable to access the client environment.
  
  \return The application server instance.
*/
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */

  return new HarixApp(env);

}

//! Well here we start, everyone knows main :)
/*!
  The createApplication argument is a function pointer to create new application instances
  for each new user surfing to the application.
  
  \return Success status to the OS.
*/
int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  return WRun(argc, argv, &createApplication);
}
