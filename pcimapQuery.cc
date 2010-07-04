
#include <Wt/WApplication>
// #include <Wt/WBreak>
// #include <Wt/WContainerWidget>
// #include <Wt/WLineEdit>
// #include <Wt/WPushButton>
#include <Wt/WTextArea>

using namespace Wt;

class PcimapQueryApp : public WApplication
{
private:
  WTextArea* pcimapList_;
public:
  PcimapQueryApp(const WEnvironment& env);
};

PcimapQueryApp::PcimapQueryApp(const WEnvironment& env) : WApplication(env)
{
  setTitle("Query Pcimaps");                               // application title

  pcimapList_ = new WTextArea(root());
  pcimapList_->setColumns(60);
  // pcimapList_->setFocus();
}
  

WApplication *createApplication(const WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */
  return new PcimapQueryApp(env);
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







