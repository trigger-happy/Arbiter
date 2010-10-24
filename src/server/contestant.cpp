#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WTabWidget>
#include <Wt/WFitLayout>
#include <boost/signals2.hpp>

#include <string>

using namespace Wt;
using namespace std;

class ContestantClient: public WApplication
{
	public:
		WContainerWidget *topContainer;
			WLabel *title;
			WPushButton *logoutButton;
		WTabWidget *tabs;
			WContainerWidget *submit;
			WContainerWidget *clars;
			WContainerWidget *runs;
			WContainerWidget *settings;

		ContestantClient(const WEnvironment& env);
		~ContestantClient(){}

	private:
		void buildSubmitTab();
		void buildClarsTab();
		void buildRunsTab();
		void buildSettingsTab();
};

ContestantClient::ContestantClient(const WEnvironment& env) : WApplication(env)
{
	topContainer = new WContainerWidget(root());
	title = new WLabel("Arbiter Contestant Client");
	logoutButton = new WPushButton("Log out");
	tabs = new WTabWidget();

	root()->addWidget(topContainer);
	root()->addWidget(tabs);
	/**
	WFitLayout *foo = new WFitLayout();
	
	foo->addWidget(title);
	foo->addWidget(logoutButton);

	topContainer->setLayout(foo);
	*/

	topContainer->addWidget(title);
	topContainer->addWidget(logoutButton);

	buildSubmitTab();
	buildClarsTab();
	buildRunsTab();
	buildSettingsTab();

	tabs->addTab(submit, WString("Submit"));
	tabs->addTab(clars, WString("Clarifications"));
	tabs->addTab(runs, WString("Runs"));
	tabs->addTab(settings, WString("Settings"));
}

void ContestantClient::buildSubmitTab()
{
	submit = new WContainerWidget();
}

void ContestantClient::buildClarsTab()
{
	clars = new WContainerWidget();
}

void ContestantClient::buildRunsTab()
{
	runs = new WContainerWidget();
}

void ContestantClient::buildSettingsTab()
{
	settings = new WContainerWidget();
}

WApplication *createApplication(const WEnvironment& env) 
{
  return new ContestantClient(env);
}

int main( int argc, char **argv) 
{
  return WRun( argc, argv, &createApplication);
}