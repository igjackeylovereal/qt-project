#include "clienttask.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"

// 实现：C++11 函数局部静态变量保证线程安全的懒汉单例。
MyTcpServer& MyTcpServer::GetInstance() {
  static MyTcpServer instance;
  return instance;
}

// 实现：为每个新连接创建 MyTcpSocket、绑定句柄、加入在线列表，最后投递到线程池中运行。
void MyTcpServer::incomingConnection(qintptr handle) {
  qDebug() << "新客户端连接";
  MyTcpSocket* tcp_socket = new MyTcpSocket;              // 为本次连接创建 socket 封装对象。
  tcp_socket->setSocketDescriptor(handle);                 // 绑定操作系统分配的连接句柄。
  tcp_socket_list_.append(tcp_socket);                    // 加入在线列表，供后续转发时按用户名查找。
  ClientTask* task = new ClientTask(tcp_socket);           // 包装为 QRunnable，交由线程池调度。
  thread_pool_.start(task);                                // 线程池启动任务，socket 开始监听数据。
}

// 实现：先移除列表元素，再调用 deleteLater 让 Qt 在事件循环安全时机释放对象。
void MyTcpServer::RemoveSocket(MyTcpSocket* my_socket) {
  tcp_socket_list_.removeOne(my_socket);                  // 从在线列表中移除。
  my_socket->deleteLater();                               // 标记延迟删除，Qt 事件循环安全时机释放。
  my_socket = nullptr;                                    // 防御性置空，防止意外复用。
}

// 实现：遍历在线连接列表，通过 str_login_name_ 匹配目标用户，匹配后直接 write 发送 PDU。
void MyTcpServer::Resend(char* target_name, Pdu* pdu) {
  if (target_name == nullptr || pdu == nullptr) {         // 空指针保护。
    return;
  }
  for (int i = 0; i < tcp_socket_list_.size(); i++) {
    if (target_name == tcp_socket_list_[i]->str_login_name_) {
      tcp_socket_list_[i]->write((char*)pdu, pdu->total_len);
      qDebug() << "Resend ...";
      break;                                              // 找到目标用户后停止遍历。
    }
  }
  // 如果遍历完未找到匹配，说明目标用户不在线，消息丢弃（当前版本不缓存离线消息）。
}

// 实现：限制线程池最大并发数为 8，避免过多并发连接耗尽系统资源。
MyTcpServer::MyTcpServer() {
  thread_pool_.setMaxThreadCount(8);
}
