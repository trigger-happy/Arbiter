#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>

using namespace Wt;

class HelloApp: public WApplication {
  public:
    HelloApp( const WEnvironment& env);
    ~HelloApp(){}
};

HelloApp::HelloApp(const WEnvironment& env) : WApplication(env)
{
  root()->addWidget(new WText("This is some tex."));
  root()->addWidget(new WText("This is another line of text."));
  root()->addWidget(new WBreak());
  root()->addWidget(new WText("This one is on another line."));
}

WApplication *createApplication(const WEnvironment& env) {
  return new HelloApp(env);
}

int main( int argc, char **argv) {
  return WRun( argc, argv, &createApplication);
}

