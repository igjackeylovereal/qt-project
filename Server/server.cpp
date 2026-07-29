// ==================== 头文件包含 ====================
#include "mytcpserver.h"    // TCP 服务器：监听端口、接收连接
#include "server.h"         // 自身的头文件，声明了 Server 类

#include <QFile>            // 读取配置文件
#include <QDebug>           // 调试打印

// ==================== 构造函数：服务端启动时执行一次 ====================
Server::Server(QWidget* parent)
    : QWidget(parent) {
  LoadConfig();                                                               // ① 读取配置文件（IP、端口、根目录）
  MyTcpServer::GetInstance().listen(QHostAddress(str_ip_), us_port_);         // ② 启动监听端口，等待客户端连接
}   // 构造函数写完，服务端就处于"等待连接"状态，不需要再主动做什么

// ==================== GetInstance：单例模式入口 ====================
Server& Server::GetInstance() {
  static Server instance;     // 静态局部变量：全局唯一
  return instance;
}

// ==================== 析构函数（当前无特殊清理） ====================
Server::~Server() {
}

// ==================== LoadConfig：读取配置文件（与 Client::LoadConfig 完全对称） ====================
void Server::LoadConfig() {
  QFile file(":/connect.config");                                 // 打开 Qt 资源文件
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "打开文件失败";
    return;
  }
  QByteArray ba_data = file.readAll();                             // 读全部内容
  QString str_data = QString(ba_data);
  QStringList str_list = str_data.split("\r\n");                    // 按换行切三行
  str_ip_ = str_list[0];                                           // 第一行：IP
  us_port_ = str_list[1].toUShort();                               // 第二行：端口
  str_root_path_ = str_list[2];                                     // 第三行：文件存储根目录
  qDebug() << "ip" << str_ip_ << "port" << us_port_
           << "strRootPath" << str_root_path_;

  file.close();
}
