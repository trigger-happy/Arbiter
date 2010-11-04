#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WEnvironment>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WText>
#include <Wt/WTreeView>
#include <Wt/WString>
#include "tree_test.h"

using namespace Wt;

TreeViewExample::TreeViewExample(WStandardItemModel *model,const WString& titleText): model_(model)
{
  item_ = model_->item(0, 0)->child(0, 0);

  new WText(titleText, this);

  WPanel *panel = new WPanel(this);
  panel->resize(600, 300);
  panel->setCentralWidget(treeView_ = new WTreeView());
  if (!WApplication::instance()->environment().ajax())
    treeView_->resize(WLength::Auto, 290);
  treeView_->setAlternatingRowColors(true);
  treeView_->setRowHeight(25);
  treeView_->setModel(model_);

  treeView_->setExpanded(model->index(0, 0), true);
  treeView_->setExpanded(model->index(0, 0, model->index(0, 0)), true);
}

WStandardItemModel *TreeViewExample::createModel(bool useInternalPath,
                                                 WObject *parent)
{
  WStandardItemModel *result = new WStandardItemModel(0, 4, parent);
  result->setHeaderData(0, Horizontal, std::string(""));

  WStandardItem *admin, *task;

  result->appendRow(admin = adminItem("General"));
  admin->appendRow(task= taskItem("Admin credentials"));
  admin->appendRow(task= taskItem("Accounts"));
  admin->appendRow(task= taskItem("Runners (Languages)"));
  admin->appendRow(task= taskItem("Other options"));

  result->appendRow(admin = adminItem("Contest config"));
  admin->appendRow(task= taskItem("Time/reset"));
  admin->appendRow(task= taskItem("Upload problem set"));
  admin->appendRow(task= taskItem("Team status"));

  result->appendRow(admin = adminItem("Contest"));
  admin->appendRow(task= taskItem("Runs"));
  admin->appendRow(task= taskItem("Clarifications"));

  result->appendRow(admin = adminItem("Logs"));
  result->appendRow(admin = adminItem("Generate report"));

  return result;
}

WStandardItem *TreeViewExample::adminItem(const std::string& admin)
{
  return new WStandardItem(admin);
}

WStandardItem *TreeViewExample::taskItem(const std::string& task)
{
  return new WStandardItem(task);
}


/*#include<Wt/WApplication>
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
*/
