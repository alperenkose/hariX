#include <Wt/WApplication>
#include "harixApp.hpp"
// #include <Wt/WLogger>


Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */

  // // Setup the logger
  // Wt::WLogger logger;
  // logger.addField("datetime", false);
  // logger.addField("session", false);
  // logger.addField("type", false);
  // logger.addField("message", true);
  // logger.addField("debug", true);

  // // logger.setFile("/tmp/mylog.txt");
  // logger.setStream(std::cout);

  // // Add an entry
  // Wt::WLogEntry entry = logger.entry();
  // entry << Wt::WLogger::timestamp << Wt::WLogger::sep
  // 		<< '[' << wApp->sessionId() << ']' << Wt::WLogger::sep
  // 		<< '[' << "notice" << ']' << Wt::WLogger::sep
  // 		<< "Succesfully started.";


  
  return new HarixApp(env);

}


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
