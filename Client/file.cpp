#include "client.h"
#include "file.h"
#include "ui_file.h"
#include "uploader.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::File) {
  ui_->setupUi(this);
  str_user_path_ = QString("%1/%2")
                       .arg(Client::GetInstance().str_root_path_)
                       .arg(Client::GetInstance().str_login_name_);
  str_cur_path_ = str_user_path_;
  FlushFile();
}

File::~File() {
  delete ui_;
  foreach (FileInfo* file_info, file_info_list_) {
    delete file_info;
  }
  file_info_list_.clear();
}

void File::FlushFile() {
  Pdu* pdu = MakePdu(str_cur_path_.toStdString().size() + 1);
  pdu->type = kFlushFileRequest;
  memcpy(pdu->msg, str_cur_path_.toStdString().c_str(),
         str_cur_path_.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}

void File::UpdateFileList(QList<FileInfo*> file_info_list) {
  foreach (FileInfo* file_info, file_info_list_) {
    delete file_info;
  }
  file_info_list_.clear();
  file_info_list_ = file_info_list;

  ui_->listWidget->clear();
  foreach (FileInfo* file_info, file_info_list) {
    QListWidgetItem* item = new QListWidgetItem;
    if (file_info->file_type == 0) {
      item->setIcon(QIcon(QPixmap(":/dir.png")));
    } else {
      item->setIcon(QIcon(QPixmap(":/file.png")));
    }
    item->setText(file_info->name);
    ui_->listWidget->addItem(item);
  }
}

void File::UploadFile() {
  Uploader* uploader = new Uploader(str_upload_file_path_);
  connect(uploader, &Uploader::ErrorMsg, this, &File::UploadErrorBox,
          Qt::QueuedConnection);
  connect(uploader, &Uploader::UploadPdu, &Client::GetInstance(),
          &Client::SendMsg, Qt::QueuedConnection);
  uploader->Start();
}

void File::UploadErrorBox(const QString& msg) {
  QMessageBox::information(this, "提示", msg);
}

void File::on_mkdir_PB_clicked() {
  QString str_dir_name = QInputDialog::getText(this, "新建文件夹", "新建文件夹名");
  if (str_dir_name.isEmpty() || str_dir_name.toStdString().size() > 32) {
    QMessageBox::information(this, "提示", "文件夹名长度非法");
    return;
  }
  Pdu* pdu = MakePdu(str_cur_path_.toStdString().size() + 1);
  pdu->type = kMkdirRequest;
  memcpy(pdu->data, str_dir_name.toStdString().c_str(), 32);
  memcpy(pdu->msg, str_cur_path_.toStdString().c_str(),
         str_cur_path_.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}

void File::on_flush_PB_clicked() {
  FlushFile();
}

void File::on_del_PB_clicked() {
  QListWidgetItem* item = ui_->listWidget->currentItem();
  if (!item) {
    return;
  }
  int ret = QMessageBox::question(this, "删除文件",
              QString("是否确认删除文件 %1").arg(item->text()));
  if (ret != QMessageBox::Yes) {
    return;
  }
  QString str_path = QString("%1/%2").arg(str_cur_path_).arg(item->text());
  Pdu* pdu = MakePdu(str_path.toStdString().size() + 1);
  pdu->type = kDelFileRequest;
  memcpy(pdu->msg, str_path.toStdString().c_str(),
         str_path.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}

void File::on_rename_PB_clicked() {
  QListWidgetItem* item = ui_->listWidget->currentItem();
  if (!item) {
    return;
  }
  QString str_new_name = QInputDialog::getText(this, "重命名", "新文件名");
  Pdu* pdu = MakePdu(str_cur_path_.toStdString().size() + 1);
  pdu->type = kRenameFileRequest;
  memcpy(pdu->data, item->text().toStdString().c_str(), 32);
  memcpy(pdu->data + 32, str_new_name.toStdString().c_str(), 32);
  memcpy(pdu->msg, str_cur_path_.toStdString().c_str(),
         str_cur_path_.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem* item) {
  foreach (FileInfo* file_info, file_info_list_) {
    if (file_info->name == item->text() && file_info->file_type != 0) {
      return;
    }
  }
  str_cur_path_ = QString("%1/%2").arg(str_cur_path_).arg(item->text());
  FlushFile();
}

void File::on_return_PB_clicked() {
  if (str_cur_path_ == str_user_path_) {
    return;
  }
  int index = str_cur_path_.lastIndexOf('/');
  str_cur_path_.remove(index, str_cur_path_.size() - index);
  FlushFile();
}

void File::on_upload_PB_clicked() {
  str_upload_file_path_.clear();
  str_upload_file_path_ = QFileDialog::getOpenFileName();
  qDebug() << "str_upload_file_path_" << str_upload_file_path_;
  if (str_upload_file_path_.isEmpty()) {
    return;
  }
  int index = str_upload_file_path_.lastIndexOf('/');
  QString str_file_name = str_upload_file_path_.right(
      str_upload_file_path_.size() - index - 1);
  QFile file(str_upload_file_path_);
  qint64 file_size = file.size();
  Pdu* pdu = MakePdu(str_cur_path_.toStdString().size() + 1);
  pdu->type = kUploadFileInitRequest;
  memcpy(pdu->data, str_file_name.toStdString().c_str(), 32);
  memcpy(pdu->data + 32, &file_size, sizeof(qint64));
  memcpy(pdu->msg, str_cur_path_.toStdString().c_str(),
         str_cur_path_.toStdString().size());
  Client::GetInstance().SendMsg(pdu);
}
