#include "client.h"
#include "friend.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::Friend) {
  ui_->setupUi(this);
  online_user_ = new OnlineUser;
  chat_ = new Chat;
  FlushFriend();
}

Friend::~Friend() {
  delete ui_;
  delete online_user_;
  delete chat_;
}

void Friend::FlushFriend() {
  Pdu* pdu = MakePdu();
  pdu->type = kFlushFriendRequest;
  memcpy(pdu->data,
         Client::GetInstance().str_login_name_.toStdString().c_str(), 32);
  Client::GetInstance().SendMsg(pdu);
}

void Friend::UpdateLw(QStringList friend_list) {
  ui_->listWidget->clear();
  ui_->listWidget->addItems(friend_list);
}

void Friend::on_findUser_PB_clicked() {
  QString str_name = QInputDialog::getText(this, "提示", "用户名：");
  if (str_name.isEmpty() || str_name.toStdString().size() > 32) {
    QMessageBox::critical(this, "提示", "长度非法");
    return;
  }
  Pdu* pdu = MakePdu();
  memcpy(pdu->data, str_name.toStdString().c_str(), 32);
  pdu->type = kFindUserRequest;
  Client::GetInstance().SendMsg(pdu);
}

void Friend::on_onlineUser_PB_clicked() {
  if (online_user_->isHidden()) {
    online_user_->show();
  }
  Pdu* pdu = MakePdu();
  pdu->type = kOnlineUserRequest;
  Client::GetInstance().SendMsg(pdu);
}

void Friend::on_flush_PB_clicked() {
  FlushFriend();
}

void Friend::on_del_PB_clicked() {
  QListWidgetItem* item = ui_->listWidget->currentItem();
  if (!item) {
    return;
  }
  QString str_tar_name = item->text();
  int ret = QMessageBox::question(this, "删除好友",
              QString("是否确定删除好友 %1").arg(str_tar_name));
  if (ret != QMessageBox::Yes) {
    return;
  }
  Pdu* pdu = MakePdu(0);
  pdu->type = kDelFriendRequest;
  QString str_cur_name = Client::GetInstance().str_login_name_;
  memcpy(pdu->data, str_cur_name.toStdString().c_str(),
         str_cur_name.size());
  memcpy(pdu->data + 32, str_tar_name.toStdString().c_str(),
         str_tar_name.size());
  Client::GetInstance().SendMsg(pdu);
}

void Friend::on_chat_PB_clicked() {
  QListWidgetItem* item = ui_->listWidget->currentItem();
  if (!item) {
    return;
  }
  if (chat_->isHidden()) {
    chat_->show();
  }
  chat_->str_chat_name_ = item->text();
}
