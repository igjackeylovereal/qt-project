#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QThreadPool>

class MyTcpServer : public QTcpServer {
  Q_OBJECT
public:
  static MyTcpServer& GetInstance();
  void incomingConnection(qintptr handle) override;
  void RemoveSocket(MyTcpSocket* my_socket);
  void Resend(char* target_name, Pdu* pdu);
  QThreadPool thread_pool_;

private:
  MyTcpServer();
  MyTcpServer(const MyTcpServer& instance) = delete;
  MyTcpServer& operator=(const MyTcpServer&) = delete;
  QList<MyTcpSocket*> tcp_socket_list_;
};

#endif // MYTCPSERVER_H
