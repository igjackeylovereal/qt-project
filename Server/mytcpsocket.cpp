#include "mytcpserver.h"
#include "mytcpsocket.h"
#include "operatedb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket() {
  pmh_ = new MsgHandler;
}

MyTcpSocket::~MyTcpSocket() {
  delete pmh_;
}

// 实现：先做空指针保护，再将整个 PDU 以字节流写入 socket，最后释放堆内存。
void MyTcpSocket::SendMsg(Pdu* pdu) {
  if (pdu == nullptr) {                    // 空指针保护：处理 HandleMsg 未匹配到协议类型的情况。
    return;
  }
  this->write((char*)pdu, pdu->total_len); // 按 PDU 协议头记录的 total_len 字段写入完整字节流。
  qDebug() << "send Msg respdu->total_len" << pdu->total_len
           << "pdu->msg_len" << pdu->msg_len
           << "pdu->type" << pdu->type
           << "pdu->data" << pdu->data
           << "pdu->data+32" << pdu->data + 32
           << "pdu->msg" << pdu->msg;
  free(pdu);                               // 业务处理函数通过 MakePdu 在堆上分配，此处负责释放。
  pdu = nullptr;                           // 防御性置空，防止函数后续扩展时意外复用已释放指针。
}

// 已废弃：原先每次读固定长度，无法处理粘包，现改用 RecvMsg() 的缓冲区方案。
// Pdu* MyTcpSocket::ReadMsg() {
//   qDebug() << "ReadMsg 接收消息长度" << this->bytesAvailable();
//   uint pdu_len = 0;
//   this->read((char*)&pdu_len, sizeof(uint));
//   uint msg_len = pdu_len - sizeof(Pdu);
//   Pdu* pdu = MakePdu(msg_len);
//   this->read((char*)pdu + sizeof(uint), pdu_len - sizeof(uint));
//   return pdu;
// }

// 实现：先将 pdu 注入业务处理器，再按类型走 switch-case 路由到具体方法。
Pdu* MyTcpSocket::HandleMsg(Pdu* pdu) {
  qDebug() << "HandleMsg pdu->total_len" << pdu->total_len
           << "pdu->msg_len" << pdu->msg_len
           << "pdu->type" << pdu->type
           << "pdu->data" << pdu->data
           << "pdu->data+32" << pdu->data + 32;
//         << "pdu->msg" << pdu->msg;
  Pdu* respdu = nullptr;                     // 初始为空：unknown 类型不回复。
  pmh_->pdu_ = pdu;                          // 注入请求 PDU，各业务方法从中读取 data/msg 字段。

  switch (pdu->type) {
  // ===== 注册 & 登录 =====
  case kRegistRequest: {
    respdu = pmh_->Regist();
    break;
  }
  case kLoginRequest: {
    respdu = pmh_->Login(str_login_name_);
    break;
  }
  // ===== 查找 & 在线用户 =====
  case kFindUserRequest: {
    respdu = pmh_->FindUser();
    break;
  }
  case kOnlineUserRequest: {
    respdu = pmh_->OnlineUser();
    qDebug() << "str_login_name_" << str_login_name_;
    break;
  }
  // ===== 好友管理 =====
  case kAddFriendRequest: {
    respdu = pmh_->AddFriend();
    break;
  }
  case kAddFriendAgreeRequest: {
    respdu = pmh_->AddFriendAgree();
    break;
  }
  case kFlushFriendRequest: {
    respdu = pmh_->FlushFriend();
    break;
  }
  case kDelFriendRequest: {
    respdu = pmh_->DelFriend();
    break;
  }
  // ===== 聊天 =====
  case kChatRequest: {
    respdu = pmh_->Chat();
    break;
  }
  // ===== 文件操作 =====
  case kMkdirRequest: {
    respdu = pmh_->Mkdir();
    break;
  }
  case kFlushFileRequest: {
    respdu = pmh_->FlushFile();
    break;
  }
  case kDelFileRequest: {
    respdu = pmh_->DelFile();
    break;
  }
  case kRenameFileRequest: {
    respdu = pmh_->RenameFile();
    break;
  }
  // ===== 文件上传 =====
  case kUploadFileInitRequest: {
    respdu = pmh_->UploadFileInit();
    break;
  }
  case kUploadFileDataRequest: {
    respdu = pmh_->UploadFileData();
    break;
  }
  default:                                   // 无法识别的协议类型，不回复。
    break;
  }

  return respdu;
}

// 实现：通过 QByteArray 缓冲区拼接数据，循环读取 PDU 头部先判断完整性，再拆包分发。
void MyTcpSocket::RecvMsg() {
  qDebug() << "RecvMsg 接收消息长度" << this->bytesAvailable();

  QByteArray data = this->readAll();             // 从 socket 一次性读出当前所有到达的字节。
  buffer_.append(data);                          // 追加到成员缓冲区，应对粘包场景。
  while (buffer_.size() >= int(sizeof(Pdu))) {   // 至少连 PDU 头部 12 字节都不够则退出。
    Pdu* pdu = (Pdu*)buffer_.data();             // 将缓冲区首地址强转为 Pdu 结构进行解析。
    if (buffer_.size() < int(pdu->total_len)) {  // 半包判断：数据还未收全，等下次 RecvMsg。
      break;
    }
    Pdu* respdu = HandleMsg(pdu);                // 分发到对应的业务处理函数。
    SendMsg(respdu);                             // 发送响应 PDU 并释放内存。
    buffer_.remove(0, pdu->total_len);           // 从缓冲区头部移除本条已处理完的 PDU。
  }
}

void MyTcpSocket::ClientOffline() {
  OperateDB::GetInstance().handleOffline(str_login_name_.toStdString().c_str());
  MyTcpServer::GetInstance().RemoveSocket(this);
}
