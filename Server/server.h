#ifndef SERVER_H
#define SERVER_H

#include <QWidget>

class Server : public QWidget {
  Q_OBJECT

public:
  QString str_ip_;
  quint16 us_port_;
  QString str_root_path_;
  static Server& GetInstance();
  ~Server();
  void LoadConfig();

private:
  Server(QWidget* parent = nullptr);
};
#endif // SERVER_H
