// ==================== 头文件包含 ====================
#include "clienttask.h"     // 连接任务：把每连接的处理挂到线程池
#include "mytcpserver.h"    // 自身的头文件
#include "mytcpsocket.h"    // 每连接的 Socket 封装（收消息、发消息、处理业务）

// ==================== GetInstance：单例模式入口 ====================
MyTcpServer& MyTcpServer::GetInstance() {
  static MyTcpServer instance;
  return instance;
}

// ==================== incomingConnection：有客户端连接进来时，Qt 框架自动调用此函数 ====================
// 参数 handle 是操作系统分配的"连接编号"（文件描述符），每个新连接都是唯一的
void MyTcpServer::incomingConnection(qintptr handle) {
  qDebug() << "新客户端连接";
  MyTcpSocket* tcp_socket = new MyTcpSocket;                // ① 为这个连接 new 一个 Socket 对象
  tcp_socket->setSocketDescriptor(handle);                   // ② 把操作系统给的连接编号绑定到 Socket 对象上
  tcp_socket_list_.append(tcp_socket);                        // ③ 加入在线连接列表（后面转发消息要靠它找人）
  ClientTask* task = new ClientTask(tcp_socket);             // ④ 创建连接任务（包装成 QRunnable，放进线程池）
  thread_pool_.start(task);                                    // ⑤ 线程池启动任务 → Socket 开始接收数据
}   // 此后这个客户端的所有收发、业务处理全部在 MyTcpSocket 里完成

// ==================== RemoveSocket：客户端断开时，从列表移除并清理 ====================
void MyTcpServer::RemoveSocket(MyTcpSocket* my_socket) {
  tcp_socket_list_.removeOne(my_socket);    // 从在线列表中移除
  my_socket->deleteLater();                // 标记延迟删除（Qt 事件循环会在安全时机真正 delete）
  my_socket = nullptr;                        // 指针置空
}

// ==================== Resend：消息转发——把 PDU 发给指定的在线用户 ====================
void MyTcpServer::Resend(char* target_name, Pdu* pdu) {
  if (target_name == nullptr || pdu == nullptr) {     // 参数校验
    return;
  }
  for (int i = 0; i < tcp_socket_list_.size(); i++) {                  // 遍历所有在线连接
    if (target_name == tcp_socket_list_[i]->str_login_name_) {      // ★ 找到用户名匹配的连接
      tcp_socket_list_[i]->write((char*)pdu, pdu->total_len); //     直接 write 把数据发过去
      qDebug() << "Resend ...";                                //     打印调试信息
      break;                                                   //     找到了就不继续找了
    }
  }
  // 如果遍历完都没找到，说明目标用户不在线，消息丢弃（当前版本不缓存离线消息）
}

// ==================== 构造函数：设置线程池最大线程数 ====================
MyTcpServer::MyTcpServer() {
  thread_pool_.setMaxThreadCount(8);    // 最多同时处理 8 个连接的 Socket 读写
}
