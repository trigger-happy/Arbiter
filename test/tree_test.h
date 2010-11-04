#ifndef TREETEST_H_
#define TREETEST_H_

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WTreeView>
#include <Wt/WText>
#include <string>

class TreeViewExample : public Wt::WContainerWidget
{
public:
  TreeViewExample(Wt::WStandardItemModel *model, const Wt::WString& titleText);
  Wt::WTreeView *treeView() const { return treeView_; }
  static Wt::WStandardItemModel *createModel(bool useInternalPath, WObject *parent);

private:
  Wt::WStandardItem      *item_;
  Wt::WStandardItemModel *model_;
  Wt::WTreeView          *treeView_;

  static Wt::WStandardItem *adminItem(const std::string& admin);
  static Wt::WStandardItem *taskItem(const std::string& task);

};

#endif
