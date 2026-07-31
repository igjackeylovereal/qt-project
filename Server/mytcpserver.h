#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

// 服务端核心类（单例），负责监听端口、接纳连接、维护在线列表及消息转发。
// 每收到一个新连接，将其封装为 MyTcpSocket 并投递到线程池中运行。
class MyTcpServer : public QTcpServer {
  Q_OBJECT
public:
  static MyTcpServer& GetInstance();                   // 单例访问接口。

  // 重写 QTcpServer::incomingConnection，将新连接包装为 MyTcpSocket 并提交到线程池。
  void incomingConnection(qintptr handle) override;

  void RemoveSocket(MyTcpSocket* my_socket);           // 从在线列表中移除指定连接并延迟删除。
  void Resend(char* target_name, Pdu* pdu);            // 遍历在线连接，按用户名匹配转发 PDU。

  QThreadPool thread_pool_;                            // 线程池，每个连接占用一个线程处理收发。

private:
  MyTcpServer();
  MyTcpServer(const MyTcpServer& instance) = delete;
  MyTcpServer& operator=(const MyTcpServer&) = delete;

  QList<MyTcpSocket*> tcp_socket_list_;                // 当前所有在线客户端连接列表。
};

#endif // MYTCPSERVER_H
