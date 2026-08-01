#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"

#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>

// 实现：初始化 UI、响应处理器，加载配置文件，连接服务器，绑定事件与槽函数。
Client::Client(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::Client) {
  ui_->setupUi(this);
  prh_ = new ResHandler;
  LoadConfig();
  socket_.connectToHost(QHostAddress(str_ip_), us_port_);
  connect(&socket_, &QTcpSocket::connected, this, &Client::ShowConnect);
  connect(&socket_, &QTcpSocket::readyRead, this, &Client::RecvMsg);
}

Client::~Client() {
  delete ui_;
  delete prh_;
}

// 实现：从 Qt 资源文件逐行解析 IP、端口及文件根路径。
void Client::LoadConfig() {
  QFile file(":/connect.config");
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "打开文件失败";
    return;
  }

  QByteArray ba_data = file.readAll();
  QString str_data = QString(ba_data);
  QStringList str_list = str_data.split("\r\n");
  str_ip_ = str_list[0];
  us_port_ = str_list[1].toUShort();
  str_root_path_ = str_list[2];
  qDebug() << "ip" << str_ip_ << "port" << us_port_
           << "strRootPath" << str_root_path_;

  file.close();
}

// 实现：C++11 函数局部静态变量，首次调用时构造，生命周期持续到程序结束。
Client& Client::GetInstance() {
  static Client instance;
  return instance;
}

// 实现：将 PDU 按 total_len 写入 socket，写入后释放堆内存。
void Client::SendMsg(Pdu* pdu) {
  socket_.write(reinterpret_cast<char*>(pdu), pdu->total_len);
  qDebug() << "send msg pdu->total_len" << pdu->total_len
           << "pdu->msg_len" << pdu->msg_len
           << "pdu->type" << pdu->type
           << "pdu->data" << pdu->data
           << "pdu->data+32" << pdu->data + 32;
  free(pdu);                               // 各业务函数通过 MakePdu 在堆上分配，此处负责释放。
  pdu = nullptr;                           // 防御性置空，防止函数后续扩展时意外复用已释放指针。
}

// 已废弃：原先每次读固定长度，无法处理粘包，现改用 RecvMsg() 的缓冲区方案。
// Pdu* Client::ReadMsg() {
//   qDebug() << "ReadMsg 接收消息长度" << socket_.bytesAvailable();
//   uint ui_pdu_len = 0;
//   socket_.read(reinterpret_cast<char*>(&ui_pdu_len), sizeof(uint));
//   uint msg_len = ui_pdu_len - sizeof(Pdu);
//   Pdu* pdu = MakePdu(msg_len);
//   socket_.read(reinterpret_cast<char*>(pdu) + sizeof(uint),
//                ui_pdu_len - sizeof(uint));
//   return pdu;
// }

// 实现：将响应 PDU 注入 ResHandler，按 type 走 switch-case 路由到各 UI 更新方法。
void Client::HandleMsg(Pdu* pdu) {
  qDebug() << "HandleMsg pdu->total_len" << pdu->total_len
           << "pdu->msg_len" << pdu->msg_len
           << "pdu->type" << pdu->type
           << "pdu->data" << pdu->data
           << "pdu->data+32" << pdu->data + 32
           << "pdu->msg" << pdu->msg;

  prh_->pdu_ = pdu;                        // 注入响应 PDU，各 UI 更新方法从中读取 data/msg 字段。
  switch (pdu->type) {
    case kRegistRespond: {
      prh_->Regist();
      break;
    }
    case kLoginRespond: {
      prh_->Login();
      break;
    }
    case kFindUserRespond: {
      prh_->FindUser();
      break;
    }
    case kOnlineUserRespond: {
      prh_->OnlineUser();
      break;
    }
    case kAddFriendRespond: {
      prh_->AddFriend();
      break;
    }
    case kAddFriendResend: {
      prh_->AddFriendResend();
      break;
    }
    case kAddFriendAgreeRespond: {
      prh_->AddFriendAgree();
      break;
    }
    case kFlushFriendRespond: {
      prh_->FlushFriend();
      break;
    }
    case kDelFriendRespond: {
      prh_->DelFriend();
      break;
    }
    case kChatResend: {
      prh_->Chat();
      break;
    }
    case kMkdirRespond: {
      prh_->Mkdir();
      break;
    }
    case kFlushFileRespond: {
      prh_->FlushFile();
      break;
    }
    case kDelFileRespond: {
      prh_->DelFile();
      break;
    }
    case kRenameFileRespond: {
      prh_->RenameFile();
      break;
    }
    case kUploadFileInitRespond: {
      prh_->UploadFileInit();
      break;
    }
    case kUploadFileDataRespond: {
      prh_->UploadFileData();
      break;
    }
    default:
      break;
  }
}

// 实现：通过 QByteArray 缓冲区拼接数据，循环读取 PDU 头部先判断完整性，再拆包分发。
void Client::RecvMsg() {
  qDebug() << "RecvMsg 接收消息长度" << socket_.bytesAvailable();

  QByteArray data = socket_.readAll();           // 从 socket 一次性读出当前所有到达的字节。
  buffer_.append(data);                          // 追加到成员缓冲区，应对粘包场景。

  while (buffer_.size() >= int(sizeof(Pdu))) {   // 至少连 PDU 头部 12 字节都不够则退出。
    Pdu* pdu = reinterpret_cast<Pdu*>(buffer_.data()); // 将缓冲区首地址强转为 Pdu 结构进行解析。
    if (buffer_.size() < int(pdu->total_len)) {  // 半包判断：数据还未收全，等下次 RecvMsg。
      break;
    }
    HandleMsg(pdu);                              // 分发到 UI 更新逻辑。
    buffer_.remove(0, pdu->total_len);           // 从缓冲区头部移除本条已处理完的 PDU。
  }
}

// 实现：连接成功后的调试输出，可用于后续状态栏更新或重连逻辑扩展。
void Client::ShowConnect() {
  qDebug() << "连接成功";
}

// 实现：校验输入合法性，组装注册 PDU（用户名+密码填入 data[64]），发送。
void Client::on_regist_PB_clicked() {
  QString str_name = ui_->name_LE->text();
  QString str_pwd = ui_->pwd_LE->text();
  if (str_name.isEmpty() || str_pwd.isEmpty()
      || str_name.toStdString().size() > 32
      || str_pwd.toStdString().size() > 32) {
    QMessageBox::critical(this, "注册", "用户名或密码长度非法");
    return;
  }

  Pdu* pdu = MakePdu();                                      // 创建空 PDU（msg_len = 0）。
  memcpy(pdu->data, str_name.toStdString().c_str(), 32);     // 用户名写入 data[0~31]。
  memcpy(pdu->data + 32, str_pwd.toStdString().c_str(), 32); // 密码写入 data[32~63]。
  pdu->type = kRegistRequest;
  SendMsg(pdu);
}

// 实现：校验输入合法性，保存登录用户名，组装登录 PDU 并发送。
void Client::on_login_PB_clicked() {
  QString str_name = ui_->name_LE->text();
  QString str_pwd = ui_->pwd_LE->text();
  if (str_name.isEmpty() || str_pwd.isEmpty()
      || str_name.toStdString().size() > 32
      || str_pwd.toStdString().size() > 32) {
    QMessageBox::critical(this, "登录", "用户名或密码长度非法");
    return;
  }

  str_login_name_ = str_name;                                // 保存登录名，供后续界面使用。
  Pdu* pdu = MakePdu();
  memcpy(pdu->data, str_name.toStdString().c_str(), 32);
  memcpy(pdu->data + 32, str_pwd.toStdString().c_str(), 32);
  pdu->type = kLoginRequest;
  SendMsg(pdu);
}
