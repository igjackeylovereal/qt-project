#ifndef CHAT_H_
#define CHAT_H_

#include <QWidget>

namespace Ui {
class Chat;
}

// 聊天窗口类，负责与单个好友的点对点聊天交互。
// 每个聊天窗口通过 str_chat_name_ 标识目标好友，发送消息时构造 kChatRequest 协议包。
class Chat : public QWidget {
  Q_OBJECT

 public:
  explicit Chat(QWidget* parent = nullptr);
  ~Chat();

  QString str_chat_name_;                  // 当前聊天对象的好友用户名。

  // 在聊天显示区域追加一条消息。
  void UpdateShowTe(QString str_msg);

 private slots:
  // 发送按钮点击槽函数：读取输入框内容，构造 kChatRequest PDU 并发送给服务端。
  void on_send_PB_clicked();

 private:
  Ui::Chat* ui_;
};

#endif  // CHAT_H_
