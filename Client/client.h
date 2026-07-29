#ifndef CLIENT_H_
#define CLIENT_H_

#include "protocol.h"
#include "reshandler.h"

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

// 客户端核心类（单例），负责与服务器的 TCP 通信、PDU 收发及协议路由。
class Client : public QWidget {
  Q_OBJECT

 public:
  ~Client();
  void LoadConfig();               // 从资源文件读取服务器 IP、端口及文件根路径。
  static Client& GetInstance();    // 单例访问接口。

  // 根据服务端响应的 PDU 类型分发到对应的 UI 更新逻辑。
  void HandleMsg(Pdu* pdu);

  QString str_ip_;              // 服务器 IP 地址。
  quint16 us_port_;             // 服务器端口号。
  QString str_root_path_;       // 文件系统根目录路径。
  QTcpSocket socket_;           // TCP 通信套接字。
  QString str_login_name_;      // 当前登录用户名。
  ResHandler* prh_;             // 响应处理器，HandleMsg 将 PDU 注入后调用其各 UI 更新方法。
  QByteArray buffer_;           // 接收缓冲区，用于拼接粘包数据并逐个拆出完整 PDU。

 public slots:
  void ShowConnect();            // 连接成功时的提示槽函数。
  void RecvMsg();                // 由 socket 可读信号触发：拼接缓冲区，循环解析 PDU 并分发。
  void SendMsg(Pdu* pdu);        // 将 PDU 写入 socket，发送后释放其内存。

 private slots:
  void on_regist_PB_clicked();   // 注册按钮：校验输入、组装 PDU、发送。
  void on_login_PB_clicked();    // 登录按钮：校验输入、保存用户名、组装 PDU、发送。

 private:
  Ui::Client* ui_;
  Client(QWidget* parent = nullptr);
  Client(const Client& instance) = delete;
  Client& operator=(const Client&) = delete;
};
#endif  // CLIENT_H_
