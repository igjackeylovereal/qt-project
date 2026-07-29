#include "protocol.h"
#include "uploader.h"

#include <QFile>
#include <QThread>

Uploader::Uploader(const QString str_file_path) {
  str_upload_file_path_ = str_file_path;
}

void Uploader::Start() {
  QThread* thread = new QThread;
  this->moveToThread(thread);
  connect(thread, &QThread::started, this, &Uploader::UploadFile,
          Qt::QueuedConnection);
  connect(this, &Uploader::Finished, thread, &QThread::quit,
          Qt::QueuedConnection);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater,
          Qt::QueuedConnection);
  thread->start();
}

void Uploader::UploadFile() {
  QFile file(str_upload_file_path_);
  if (!file.open(QIODevice::ReadOnly)) {
    emit ErrorMsg("打开文件失败");
    emit Finished();
  }

  while (true) {
    Pdu* data_pdu = MakePdu(4096);
    data_pdu->type = kUploadFileDataRequest;
    qint64 ret = file.read(data_pdu->msg, 4096);
    if (ret == 0) {
      break;
    }
    if (ret < 0) {
      emit ErrorMsg("读取文件失败");
      break;
    }
    data_pdu->msg_len = ret;
    data_pdu->total_len = ret + sizeof(Pdu);
    emit UploadPdu(data_pdu);
  }
  file.close();
  emit Finished();
}
