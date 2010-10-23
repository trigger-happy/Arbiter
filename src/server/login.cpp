#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <boost/signals2.hpp>
#include <string>

using namespace Wt;
using namespace std;

class LoginScreen: public WApplication {
  public:
	WLineEdit *username;
	WLineEdit *password;
	WPushButton *login;
	boost::signals2::signal<void (string,string)> *sig;
    LoginScreen( const WEnvironment& env);
	void loginPressed();
	boost::signals2::signal<void (string,string)>* getSignal();
    ~LoginScreen(){}
};

LoginScreen::LoginScreen(const WEnvironment& env) : WApplication(env)
{
	username = new WLineEdit( WString(""));
	password = new WLineEdit( WString(""));
	login = new WPushButton( WString("LOGIN"));
	sig = new  boost::signals2::signal<void (string,string)>();

	password->setEchoMode(WLineEdit::EchoMode::Password);
	login->clicked().connect(SLOT(this, LoginScreen::loginPressed));

	root()->addWidget(new WLabel( WString("Enter Username:")));
	root()->addWidget( username );
	root()->addWidget(new WBreak());
	root()->addWidget(new WLabel( WString("Enter Password:")));
	root()->addWidget( password );
	root()->addWidget(new WBreak());
	root()->addWidget(login);
}

void LoginScreen::loginPressed() {
	string user = username->text().toUTF8();
	string pass = password->text().toUTF8();

	(*sig)(user,pass);
}

boost::signals2::signal<void (string,string)>* LoginScreen::getSignal() {
	return sig;
}

WApplication *createApplication(const WEnvironment& env) {
  return new LoginScreen(env);
}

int main( int argc, char **argv) {
  return WRun( argc, argv, &createApplication);
}

