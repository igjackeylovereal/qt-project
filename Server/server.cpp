#include "mytcpserver.h"
#include "server.h"

#include <QFile>
#include <QDebug>

// 实现：读取配置文件后，启动 MyTcpServer 监听指定 IP 和端口。
Server::Server(QWidget* parent)
    : QWidget(parent) {
  LoadConfig();
  MyTcpServer::GetInstance().listen(QHostAddress(str_ip_), us_port_); // 启动监听，服务端进入等待连接状态。
}

// 实现：C++11 函数局部静态变量保证线程安全的懒汉单例。
Server& Server::GetInstance() {
  static Server instance;
  return instance;
}

Server::~Server() {
}

// 实现：从 Qt 资源文件逐行解析 IP、端口及文件根路径，与 Client::LoadConfig 完全对称。
void Server::LoadConfig() {
  QFile file(":/connect.config");                               // 打开 Qt 资源文件中内嵌的配置文件。
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "打开文件失败";
    return;
  }
  QByteArray ba_data = file.readAll();
  QString str_data = QString(ba_data);
  QStringList str_list = str_data.split("\r\n");                  // 按 Windows 风格换行符切分三行。
  str_ip_ = str_list[0];                                         // 第一行：监听 IP 地址。
  us_port_ = str_list[1].toUShort();                             // 第二行：监听端口号。
  str_root_path_ = str_list[2];                                   // 第三行：文件存储根目录路径。
  qDebug() << "ip" << str_ip_ << "port" << us_port_
           << "strRootPath" << str_root_path_;

  file.close();
}
