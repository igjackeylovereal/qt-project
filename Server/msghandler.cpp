#include "msghandler.h"
#include "mytcpserver.h"
#include "operatedb.h"
#include "server.h"
#include "stdlib.h"
#include "string.h"
#include <QDebug>
#include <QDir>

MsgHandler::MsgHandler() {
}

Pdu *MsgHandler::Regist() {
  char ca_name[32] = {'\0'};
  memcpy(ca_name, pdu_->data, 32);
  char ca_pwd[32] = {'\0'};
  memcpy(ca_pwd, pdu_->data+32, 32);
  qDebug() << "regist caName" << ca_name << "caPwd" << ca_pwd;
  bool ret = OperateDB::GetInstance().handleRegist(ca_name, ca_pwd);
  qDebug() << "regist ret" << ret;
  if (ret) {
    QDir dir;
    bool res = dir.mkdir(QString("%1/%2").arg(Server::GetInstance().str_root_path_).arg(ca_name));
    qDebug() << "创建用户文件夹 res" << res;
  }
  Pdu* respdu = MakePdu();
  memcpy(respdu->data, &ret, sizeof(bool));
  respdu->type = kRegistRespond;
  return respdu;
}

Pdu *MsgHandler::Login(QString &str_login_name) {
  char ca_name[32] = {'\0'};
  memcpy(ca_name, pdu_->data, 32);
  char ca_pwd[32] = {'\0'};
  memcpy(ca_pwd, pdu_->data+32, 32);
  qDebug() << "login caName" << ca_name << "caPwd" << ca_pwd;
  bool ret = OperateDB::GetInstance().handleLogin(ca_name, ca_pwd);
  qDebug() << "login ret" << ret;
  if (ret) {
    str_login_name = ca_name;
  }
  Pdu* respdu = MakePdu();
  memcpy(respdu->data, &ret, sizeof(bool));
  respdu->type = kLoginRespond;
  return respdu;
}

Pdu *MsgHandler::FindUser() {
  char ca_name[32] = {'\0'};
  memcpy(ca_name, pdu_->data, 32);
  qDebug() << "find user caName" << ca_name;
  int ret = OperateDB::GetInstance().handleFindUser(ca_name);
  qDebug() << "find user ret" << ret;
  Pdu* respdu = MakePdu();
  memcpy(respdu->data, &ret, sizeof(int));
  respdu->type = kFindUserRespond;
  return respdu;
}

Pdu *MsgHandler::OnlineUser() {
  QStringList res = OperateDB::GetInstance().handleOnlineUser();
  Pdu* respdu = MakePdu(res.size()*32);
  respdu->type = kOnlineUserRespond;
  for (int i=0; i<res.size(); i++) {
    memcpy(respdu->msg+i*32, res[i].toStdString().c_str(), 32);
  }
  return respdu;
}

Pdu *MsgHandler::AddFriend() {
  char ca_cur_name[32] = {'\0'};
  char ca_tar_name[32] = {'\0'};
  memcpy(ca_cur_name, pdu_->data, 32);
  memcpy(ca_tar_name, pdu_->data+32, 32);
  int ret = OperateDB::GetInstance().handleAddFriend(ca_cur_name, ca_tar_name);
  qDebug() << "addFriend ret" << ret;
  if (ret == 1) {
    pdu_->type = kAddFriendResend;
    MyTcpServer::GetInstance().Resend(ca_tar_name, pdu_);
    return nullptr;
  }
  Pdu* respdu = MakePdu();
  respdu->type = kAddFriendRespond;
  memcpy(respdu->data, &ret, sizeof (int));
  return respdu;
}

Pdu *MsgHandler::AddFriendAgree() {
  char ca_cur_name[32] = {'\0'};
  char ca_tar_name[32] = {'\0'};
  memcpy(ca_cur_name, pdu_->data, 32);
  memcpy(ca_tar_name, pdu_->data+32, 32);
  bool ret = OperateDB::GetInstance().handleAddFriendAgree(ca_cur_name, ca_tar_name);
  qDebug() << "addFriendAgree ret" << ret;
  Pdu* respdu = MakePdu();
  respdu->type = kAddFriendAgreeRespond;
  memcpy(respdu->data, &ret, sizeof (int));
  MyTcpServer::GetInstance().Resend(ca_cur_name, respdu);
  return respdu;
}

Pdu *MsgHandler::FlushFriend() {
  QStringList res = OperateDB::GetInstance().handleFlushFriend(pdu_->data);
  Pdu* respdu = MakePdu(res.size()*32);
  respdu->type = kFlushFriendRespond;
  for (int i=0; i<res.size(); i++) {
    memcpy(respdu->msg+i*32, res[i].toStdString().c_str(), 32);
  }
  return respdu;
}

Pdu *MsgHandler::DelFriend() {
  char cur_name[32] = {'\0'};
  char tar_name[32] = {'\0'};
  memcpy(cur_name, pdu_->data, 32);
  memcpy(tar_name, pdu_->data+32, 32);
  bool ret = OperateDB::GetInstance().handleDelFriend(cur_name, tar_name);
  qDebug() << "delFriend ret: " << ret;
  Pdu* respdu = MakePdu();
  respdu->type = kDelFriendRespond;
  memcpy(respdu->data, &ret, sizeof(bool));
  return respdu;
}

Pdu *MsgHandler::Chat() {
  char tar_name[32] = {'\0'};
  memcpy(tar_name, pdu_->data+32, 32);
  pdu_->type = kChatResend;
  MyTcpServer::GetInstance().Resend(tar_name, pdu_);
  return nullptr;
}

Pdu *MsgHandler::Mkdir() {
  QString str_path = QString("%1/%2").arg(pdu_->msg).arg(pdu_->data);
  qDebug() << "mkdir strPath" << str_path;
  QDir dir;
  bool ret = dir.mkdir(str_path);
  qDebug() << "mkdir ret: " << ret;
  Pdu* respdu = MakePdu();
  respdu->type = kMkdirRespond;
  memcpy(respdu->data, &ret, sizeof(bool));
  return respdu;
}

Pdu *MsgHandler::FlushFile() {
  QDir dir(pdu_->msg);
  QFileInfoList file_info_list = dir.entryInfoList();

  Pdu* respdu = MakePdu((file_info_list.size()-2) * sizeof(FileInfo));
  respdu->type = kFlushFileRespond;
  for (int i=0, j=0; i<file_info_list.size(); i++) {
    if (file_info_list[i].fileName() == "." || file_info_list[i].fileName() == "..") {
      continue;
    }
    FileInfo* file_info = reinterpret_cast<FileInfo*>(respdu->msg) + j++;
    if (file_info_list[i].isDir()) {
      file_info->file_type = 0;
    } else {
      file_info->file_type = 1;
    }
    memcpy(file_info->name, file_info_list[i].fileName().toStdString().c_str(), 32);
    qDebug() << "pFileInfo->name" << file_info->name;
  }
  return respdu;
}

Pdu *MsgHandler::DelFile() {
  QFileInfo file_info(pdu_->msg);
  int ret;
  if (file_info.isDir()) {
    QDir dir(pdu_->msg);
    ret = dir.removeRecursively();
  } else {
    QFile file;
    ret = file.remove(pdu_->msg);
  }
  qDebug() << "delFile ret" << ret;
  Pdu* respdu = MakePdu();
  respdu->type = kDelFileRespond;
  memcpy(respdu->data, &ret, sizeof(bool));
  return respdu;
}

Pdu *MsgHandler::RenameFile() {
  char ca_old_name[32] = {'\0'};
  char ca_new_name[32] = {'\0'};
  memcpy(ca_old_name, pdu_->data, 32);
  memcpy(ca_new_name, pdu_->data+32, 32);
  QString str_old_path = QString("%1/%2").arg(pdu_->msg).arg(ca_old_name);
  QString str_new_path = QString("%1/%2").arg(pdu_->msg).arg(ca_new_name);
  QDir dir;
  bool ret = dir.rename(str_old_path, str_new_path);
  Pdu* respdu = MakePdu();
  respdu->type = kRenameFileRespond;
  memcpy(respdu->data, &ret, sizeof(bool));
  return respdu;
}

Pdu *MsgHandler::UploadFileInit() {
  char ca_file_name[32] = {'\0'};
  memcpy(ca_file_name, pdu_->data, 32);
  upload_file_size_ = 0;
  memcpy(&upload_file_size_, pdu_->data+32, sizeof(qint64));
  QString str_path = QString("%1/%2").arg(pdu_->msg).arg(ca_file_name);
  upload_file_.setFileName(str_path);
  upload_file_received_ = 0;
  bool ret = upload_file_.open(QIODevice::WriteOnly);
  Pdu* respdu = MakePdu();
  respdu->type = kUploadFileInitRespond;
  memcpy(respdu->data, &ret, sizeof(bool));
  return respdu;
}

Pdu *MsgHandler::UploadFileData() {
  upload_file_.write(pdu_->msg, pdu_->msg_len);
  upload_file_received_ += pdu_->msg_len;
  if (upload_file_received_ < upload_file_size_) {
    return nullptr;
  }
  upload_file_.close();
  Pdu* respdu = MakePdu();
  respdu->type = kUploadFileDataRespond;
  return respdu;
}
