#ifndef CHAT_H_
#define CHAT_H_

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget {
  Q_OBJECT

 public:
  explicit Chat(QWidget* parent = nullptr);
  ~Chat();
  QString str_chat_name_;
  void UpdateShowTe(QString str_msg);

 private slots:
  void on_send_PB_clicked();

 private:
  Ui::Chat* ui_;
};

#endif  // CHAT_H_
