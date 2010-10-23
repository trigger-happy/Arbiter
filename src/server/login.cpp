#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
using namespace Wt;

class LoginScreen: public WApplication {
  public:
	WLineEdit *username;
	WLineEdit *password;
	WPushButton *login;

    LoginScreen( const WEnvironment& env);
	void loginPressed();
    ~LoginScreen(){}
};

LoginScreen::LoginScreen(const WEnvironment& env) : WApplication(env)
{
	username = new WLineEdit( WString(""));
	password = new WLineEdit( WString(""));
	login = new WPushButton( WString("LOGIN"));
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
	username->setEnabled(false);
}
WApplication *createApplication(const WEnvironment& env) {
  return new LoginScreen(env);
}

int main( int argc, char **argv) {
  return WRun( argc, argv, &createApplication);
}

