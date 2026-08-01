#include "chat.h"
#include "client.h"
#include "protocol.h"
#include "ui_chat.h"

Chat::Chat(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::Chat) {
  ui_->setupUi(this);
}

Chat::~Chat() {
  delete ui_;
}

// 实现：将传入的消息文本追加到聊天显示区域的末尾。
void Chat::UpdateShowTe(QString str_msg) {
  ui_->show_TE->append(str_msg);
}

// 实现：读取输入框内容，校验非空后构造 kChatRequest PDU，
//      将发送者名填入 data[0:31]、目标好友名填入 data[32:63]，
//      消息内容存入 msg[] 柔性数组，最后通过 Client 单例发送给服务端。
void Chat::on_send_PB_clicked() {
  QString str_msg = ui_->input_LE->text();
  if (str_msg.isEmpty()) {
    return;
  }
  ui_->input_LE->clear();

  Pdu* pdu = MakePdu(str_msg.toStdString().size() + 1);
  pdu->type = kChatRequest;
  memcpy(pdu->data,
         Client::GetInstance().str_login_name_.toStdString().c_str(), 32);
  memcpy(pdu->data + 32,
         str_chat_name_.toStdString().c_str(), 32);
  memcpy(pdu->msg,
         str_msg.toStdString().c_str(),
         str_msg.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}
