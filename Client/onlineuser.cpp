#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::OnlineUser) {
  ui_->setupUi(this);
}

OnlineUser::~OnlineUser() {
  delete ui_;
}

void OnlineUser::UpdateLw(QStringList user_list) {
  ui_->listWidget->clear();
  ui_->listWidget->addItems(user_list);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  QString str_cur_name = Client::GetInstance().str_login_name_;
  QString str_tar_name = item->text();
  Pdu* pdu = MakePdu();
  pdu->type = kAddFriendRequest;
  memcpy(pdu->data, str_cur_name.toStdString().c_str(), 32);
  memcpy(pdu->data + 32, str_tar_name.toStdString().c_str(), 32);
  Client::GetInstance().SendMsg(pdu);
}
