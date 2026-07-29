#include "client.h"
#include "index.h"
#include "reshandler.h"
#include "string.h"
#include <QDebug>
#include <QMessageBox>
#include "chat.h"

ResHandler::ResHandler() {}

void ResHandler::Regist() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "regist ret" << ret;
  if (ret) {
    QMessageBox::information(&Client::GetInstance(), "提示", "注册成功");
  } else {
    QMessageBox::information(&Client::GetInstance(), "提示", "注册失败");
  }
}

void ResHandler::Login() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "login ret" << ret;
  if (ret) {
    Index::GetInstance().show();
    Client::GetInstance().hide();
  } else {
    QMessageBox::information(&Client::GetInstance(), "提示", "登录失败");
  }
}

void ResHandler::FindUser() {
  int ret;
  memcpy(&ret, pdu_->data, sizeof(int));
  qDebug() << "find user ret" << ret;
  if (ret == 0) {
    QMessageBox::information(&Index::GetInstance(), "提示", "该用户不在线");
  }
  if (ret == 1) {
    QMessageBox::information(&Index::GetInstance(), "提示", "该用户在线");
  }
  if (ret == 2) {
    QMessageBox::information(&Index::GetInstance(), "提示", "该用户不存在");
  } else if (ret == -1) {
    QMessageBox::information(&Index::GetInstance(), "提示", "查找失败");
  }
}

void ResHandler::OnlineUser() {
  uint ui_size = pdu_->msg_len / 32;
  char ca_tmp[32] = {'\0'};
  QStringList user_list;
  for (uint i = 0; i < ui_size; i++) {
    memcpy(ca_tmp, pdu_->msg + i * 32, 32);
    user_list.append(ca_tmp);
  }
  Index::GetInstance().GetFriend()->online_user_->UpdateLw(user_list);
}

void ResHandler::AddFriend() {
  int ret;
  memcpy(&ret, pdu_->data, sizeof(int));
  qDebug() << "addFriend ret" << ret;
  if (ret == 0) {
    QMessageBox::information(&Index::GetInstance(), "提示", "该用户不在线");
  }
  if (ret == -2) {
    QMessageBox::information(&Index::GetInstance(), "提示", "该用户已经是好友");
  } else if (ret == -1) {
    QMessageBox::information(&Index::GetInstance(), "提示", "服务器错误：联系开发人员");
  }
}

void ResHandler::AddFriendResend() {
  char name[32] = {'\0'};
  memcpy(name, pdu_->data, 32);
  int ret = QMessageBox::question(&Index::GetInstance(), "添加好友",
      QString("是否同意 %1 的添加好友请求？").arg(name));
  if (ret != QMessageBox::Yes) {
    return;
  }
  Pdu* respdu = MakePdu();
  memcpy(respdu->data, pdu_->data, 64);
  respdu->type = kAddFriendAgreeRequest;
  Client::GetInstance().SendMsg(respdu);
}

void ResHandler::AddFriendAgree() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "addFriendAgree ret" << ret;
  if (ret) {
    Index::GetInstance().GetFriend()->FlushFriend();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "添加好友失败");
  }
}

void ResHandler::FlushFriend() {
  QStringList friend_list;
  int i_size = pdu_->msg_len / 32;
  char ca_tmp[32] = {'\0'};
  for (int i = 0; i < i_size; i++) {
    memcpy(ca_tmp, pdu_->msg + i * 32, 32);
    friend_list.append(ca_tmp);
  }
  Index::GetInstance().GetFriend()->UpdateLw(friend_list);
}

void ResHandler::DelFriend() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "delFriend ret" << ret;
  if (ret) {
    Index::GetInstance().GetFriend()->FlushFriend();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "删除好友失败");
  }
}

void ResHandler::Chat() {
  ::Chat* c = Index::GetInstance().GetFriend()->chat_;
  if (c->isHidden()) {
    c->show();
  }
  char ca_chat_name[32] = {'\0'};
  memcpy(ca_chat_name, pdu_->data, 32);
  c->UpdateShowTe(QString("%1 : %2").arg(ca_chat_name).arg(pdu_->msg));
  c->str_chat_name_ = ca_chat_name;
}

void ResHandler::Mkdir() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "mkdir ret" << ret;
  if (ret) {
    Index::GetInstance().GetFile()->FlushFile();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "创建文件夹失败");
  }
}

void ResHandler::FlushFile() {
  int i_count = pdu_->msg_len / sizeof(FileInfo);

  QList<FileInfo*> file_info_list;
  for (int i = 0; i < i_count; i++) {
    FileInfo* file_info = new FileInfo;
    memcpy(file_info, pdu_->msg + i * sizeof(FileInfo), sizeof(FileInfo));
    file_info_list.append(file_info);
  }
  Index::GetInstance().GetFile()->UpdateFileList(file_info_list);
}

void ResHandler::DelFile() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "delFile ret" << ret;
  if (ret) {
    Index::GetInstance().GetFile()->FlushFile();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "删除文件失败");
  }
}

void ResHandler::RenameFile() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "renameFile ret" << ret;
  if (ret) {
    Index::GetInstance().GetFile()->FlushFile();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "重命名文件失败");
  }
}

void ResHandler::UploadFileInit() {
  bool ret;
  memcpy(&ret, pdu_->data, sizeof(bool));
  qDebug() << "uploadFileInit ret" << ret;
  if (ret) {
    Index::GetInstance().GetFile()->UploadFile();
  } else {
    QMessageBox::information(&Index::GetInstance(), "提示", "上传文件失败");
  }
}

void ResHandler::UploadFileData() {
  QMessageBox::information(&Index::GetInstance(), "提示", "上传文件完成");
  Index::GetInstance().GetFile()->FlushFile();
}
