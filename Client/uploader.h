#ifndef UPLOADER_H_
#define UPLOADER_H_

#include "protocol.h"

#include <QObject>

class Uploader : public QObject {
  Q_OBJECT

 public:
  Uploader(const QString str_file_path);
  QString str_upload_file_path_;
  void Start();

 public slots:
  void UploadFile();

 signals:
  void ErrorMsg(const QString& msg);
  void UploadPdu(Pdu* pdu);
  void Finished();
};

#endif  // UPLOADER_H_
