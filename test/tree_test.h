#ifndef TREEVIEWEXAMPLE_H_
#define TREEVIEWEXAMPLE_H_

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
  WStandardItem      *item_;
  WStandardItemModel *model_;
  WTreeView          *treeView_;

  static WStandardItem *adminItem(const std::string& admin);
  static WStandardItem *taskItem(const std::string& task);

};

#endif
