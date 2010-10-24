#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WTabWidget>
#include <Wt/WFitLayout>
#include <boost/signals2.hpp>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WGridLayout>
#include <Wt/WPushButton>
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
				WLineEdit *oldPassword;
				WLineEdit *newPassword;
				WLineEdit *confirmPassword;
				WLabel *settingsMessage;
	
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
	WContainerWidget *mid = new WContainerWidget(settings);

	oldPassword = new WLineEdit(WString(""));
	newPassword = new WLineEdit(WString(""));
	confirmPassword = new WLineEdit(WString(""));
	settingsMessage = new WLabel(WString("Error Messages here"));

	WGridLayout *glayout = new WGridLayout();


	glayout->setVerticalSpacing(15);

	glayout->addWidget(new WLabel( WString("Enter Old Password:")), 0, 0);
	glayout->addWidget(oldPassword, 0, 1);
	glayout->addWidget(new WLabel( WString("Enter New Password:")), 1, 0);
	glayout->addWidget(newPassword, 1, 1);
	glayout->addWidget(new WLabel( WString("Confirm New Password:")), 2, 0);
	glayout->addWidget(confirmPassword, 2, 1);
	glayout->addWidget(settingsMessage, 3, 0);

	mid->setLayout(glayout);

	settings->addWidget(mid);
	settings->addWidget(new WBreak());
	settings->addWidget( new WPushButton( WString("Change Password")));
}

WApplication *createApplication(const WEnvironment& env) 
{
  return new ContestantClient(env);
}

int main( int argc, char **argv) 
{
  return WRun( argc, argv, &createApplication);
}