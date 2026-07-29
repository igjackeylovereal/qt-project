#ifndef ONLINEUSER_H_
#define ONLINEUSER_H_

#include <QListWidget>
#include <QWidget>

namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget {
  Q_OBJECT

 public:
  explicit OnlineUser(QWidget* parent = nullptr);
  ~OnlineUser();
  void UpdateLw(QStringList user_list);

 private slots:
  void on_listWidget_itemDoubleClicked(QListWidgetItem* item);

 private:
  Ui::OnlineUser* ui_;
};

#endif  // ONLINEUSER_H_
