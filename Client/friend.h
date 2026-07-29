#ifndef FRIEND_H_
#define FRIEND_H_

#include "chat.h"
#include "onlineuser.h"

#include <QWidget>

namespace Ui {
class Friend;
}

class Friend : public QWidget {
  Q_OBJECT

 public:
  explicit Friend(QWidget* parent = nullptr);
  ~Friend();
  OnlineUser* online_user_;
  Chat* chat_;
  void FlushFriend();
  void UpdateLw(QStringList friend_list);

 private slots:
  void on_findUser_PB_clicked();
  void on_onlineUser_PB_clicked();
  void on_flush_PB_clicked();
  void on_del_PB_clicked();
  void on_chat_PB_clicked();

 private:
  Ui::Friend* ui_;
};

#endif  // FRIEND_H_
