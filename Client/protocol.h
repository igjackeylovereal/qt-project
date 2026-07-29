#ifndef CLIENT_PROTOCOL_H_
#define CLIENT_PROTOCOL_H_

using uint = unsigned int;

enum MessageType {
  kMin = 0,
  kRegistRequest,
  kRegistRespond,
  kLoginRequest,
  kLoginRespond,
  kFindUserRequest,
  kFindUserRespond,
  kOnlineUserRequest,
  kOnlineUserRespond,
  kAddFriendRequest,
  kAddFriendRespond,
  kAddFriendResend,
  kAddFriendAgreeRequest,
  kAddFriendAgreeRespond,
  kFlushFriendRequest,
  kFlushFriendRespond,
  kDelFriendRequest,
  kDelFriendRespond,
  kChatRequest,
  kChatResend,
  kMkdirRequest,
  kMkdirRespond,
  kFlushFileRequest,
  kFlushFileRespond,
  kDelFileRequest,
  kDelFileRespond,
  kRenameFileRequest,
  kRenameFileRespond,
  kUploadFileInitRequest,
  kUploadFileInitRespond,
  kUploadFileDataRequest,
  kUploadFileDataRespond,

  kMax = 99999,
};

struct Pdu {
  uint total_len;
  uint msg_len;
  uint type;
  char data[64];
  char msg[];
};

struct FileInfo {
  char name[32];
  int file_type;
};

Pdu* MakePdu(uint msg_len = 0);

#endif  // CLIENT_PROTOCOL_H_
