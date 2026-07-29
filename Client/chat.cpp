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

void Chat::UpdateShowTe(QString str_msg) {
  ui_->show_TE->append(str_msg);
}

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
