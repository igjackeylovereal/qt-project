#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QFile>
#include <QString>


class MsgHandler {
 public:
  Pdu* pdu_;
  qint64 upload_file_size_;
  qint64 upload_file_received_;
  QFile upload_file_;
  MsgHandler();
  Pdu* Regist();
  Pdu* Login(QString& str_login_name);
  Pdu* FindUser();
  Pdu* OnlineUser();
  Pdu* AddFriend();
  Pdu* AddFriendAgree();
  Pdu* FlushFriend();
  Pdu* DelFriend();
  Pdu* Chat();
  Pdu* Mkdir();
  Pdu* FlushFile();
  Pdu* DelFile();
  Pdu* RenameFile();
  Pdu* UploadFileInit();
  Pdu* UploadFileData();
};

#endif // MSGHANDLER_H
