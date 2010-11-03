#include <Wt/WApplication>
#include <Wt/WText>

using namespace Wt;

class TreeViewApplication: public WApplication
{
public:
  TreeViewApplication(const WEnvironment &env):
    WApplication(env)
  {
    WStandardItemModel *model = TreeViewExample::createModel(true, this);

    root()->addWidget(new TreeViewExample(model, WString::tr("Arbiter Admin Page")));

    /*
     * Stub for the drink info
     */
//     aboutDrink_ = new WText("", root());
    
  //  internalPathChanged().connect(this, &TreeViewApplication::handlePathChange);
  }
private:
  WText *aboutDrink_;

//   void handlePathChange() {
//     if (internalPathMatches("/drinks/")) {
//       std::string drink = internalPathNextPart("/drinks/");
//       aboutDrink_->setText(WString::tr("drink-" + drink));
//    }
//  }

};

WApplication *createApplication(const WEnvironment& env)
{
  WApplication *app = new TreeViewApplication(env);
  return app;
}

int main(int argc, char **argv)
{
  return WRun(argc, argv, &createApplication);
}
