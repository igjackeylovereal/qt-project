#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

// 服务端启动类（单例），负责读取配置并启动 QTcpServer 监听。
// 本身不处理业务逻辑，所有网络管理交给 MyTcpServer 完成。
class Server : public QWidget {
  Q_OBJECT

public:
  QString str_ip_;             // 监听 IP 地址，从配置文件读取。
  quint16 us_port_;            // 监听端口号，从配置文件读取。
  QString str_root_path_;      // 文件系统根目录路径，从配置文件读取。

  static Server& GetInstance(); // 单例访问接口。
  ~Server();
  void LoadConfig();            // 从 Qt 资源文件读取 IP、端口及文件根路径。

private:
  Server(QWidget* parent = nullptr);
};

#endif // SERVER_H
