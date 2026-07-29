#include "clienttask.h"
#include "mytcpsocket.h"

#include <QTcpSocket>

ClientTask::ClientTask(MyTcpSocket* socket) : QObject(socket) {
  socket_ = socket;
}

void ClientTask::run() {
  connect(socket_, &QTcpSocket::readyRead, socket_, &MyTcpSocket::RecvMsg);
  connect(socket_, &QTcpSocket::disconnected, socket_, &MyTcpSocket::ClientOffline);
}
