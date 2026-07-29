#ifndef RESHANDLER_H_
#define RESHANDLER_H_

#include "protocol.h"

class ResHandler {
 public:
  Pdu* pdu_;
  ResHandler();
  void Regist();
  void Login();
  void FindUser();
  void OnlineUser();
  void AddFriend();
  void AddFriendResend();
  void AddFriendAgree();
  void FlushFriend();
  void DelFriend();
  void Chat();
  void Mkdir();
  void FlushFile();
  void DelFile();
  void RenameFile();
  void UploadFileInit();
  void UploadFileData();
};

#endif  // RESHANDLER_H_
