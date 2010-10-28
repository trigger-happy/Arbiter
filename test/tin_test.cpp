#include<Wt/WApplication>
#include<Wt/WContainerWidget>
#include<Wt/WBreak>
#include<Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WGridLayout>
#include <Wt/WContainerWidget>

using namespace Wt;

class App : public WApplication
{
	public:
                App(const WEnvironment& env);
		~App();
                void add();
                void del();
        private:
                WLineEdit *username_;
                WLineEdit *password_;
                WLineEdit *teamname_;
                WContainerWidget *w;
};

WApplication *createApplication(const WEnvironment& env)
{
        return new App(env);
}

App::App(const WEnvironment& env): WApplication(env)
{
    setTitle("Admin Page");                               // application title

    WPushButton *b = new WPushButton("Add", root()); // create a button
    //b->setMargin(100, Right);                                 // add 5 pixels margin
    WPushButton *c = new WPushButton("Delete", root()); // create a button
    //root()->addWidget(new WBreak());

    //root()->addWidget(new WText("username : "));  // show some text
    username_ = new WLineEdit(root());               // allow text input
    username_->setFocus();
    //root()->addWidget(new WBreak());

    //root()->addWidget(new WText("password : "));  // show some text
    password_ = new WLineEdit(root());               // allow text input
    password_->setFocus();
    //root()->addWidget(new WBreak());

    //root()->addWidget(new WText("teamname : "));  // show some text
    teamname_ = new WLineEdit(root());               // allow text input
    teamname_->setFocus();

    WContainerWidget *w = new WContainerWidget(root());
    w->resize(400, 200);

    //b->clicked().connect(this, &App::add);
    //c->clicked().connect(this, &App::del);

    WGridLayout *layout = new WGridLayout();
    layout->addWidget(b, 0, 0);
    layout->addWidget(c, 0, 1);
    layout->addWidget(new WText("username: "), 1, 0);
    layout->addWidget(username_, 1, 1);
    layout->addWidget(new WText("password: "), 2, 0);
    layout->addWidget(password_, 2, 1);
    layout->addWidget(new WText("teamname: "), 3, 0);
    layout->addWidget(teamname_, 3, 1);

    w->setLayout(layout);
}

App::~App()
{
}
void App::add()
{
}
void App::del()
{
}
int main(int argc, char **argv)
{
        return WRun(argc, argv, &createApplication);
}
