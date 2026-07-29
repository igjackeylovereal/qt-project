#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <QTcpSocket>

// 自定义 QTcpSocket 子类，负责单个客户端连接的收发与业务路由。
class MyTcpSocket : public QTcpSocket {
  Q_OBJECT
public:
  MyTcpSocket();
  ~MyTcpSocket();

  QString str_login_name_;   // 当前连接对应的登录用户名。
  MsgHandler* pmh_;          // 业务处理器，HandleMsg 将 PDU 注入后调用其各业务方法。
  QByteArray buffer_;        // 接收缓冲区，用于拼接粘包数据并逐个拆出完整 PDU。

  // 将响应 PDU 写入 socket，并在发送后释放其内存。
  void SendMsg(Pdu* pdu);

  // 根据请求 PDU 的协议类型分发到对应的业务处理函数，返回响应 PDU。
  // 若协议类型无法识别，返回 nullptr 表示不回复。
  Pdu* HandleMsg(Pdu* pdu);

public slots:
  // 由 socket 可读信号触发：读取数据、拼接缓冲区、循环解析完整 PDU 并分发处理。
  void RecvMsg();
  // 由 socket 断开信号触发：更新数据库在线状态并从服务器列表中移除该连接。
  void ClientOffline();
};

#endif // MYTCPSOCKET_H
