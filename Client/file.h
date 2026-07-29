#ifndef FILE_H_
#define FILE_H_

#include "protocol.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget {
  Q_OBJECT

 public:
  QString str_user_path_;          // 用户根目录（如 ./root/zhangsan），不可回退到此之上
  QString str_cur_path_;           // 当前浏览目录，进入子目录时追加路径
  QString str_upload_file_path_;   // 用户选择要上传的本地文件完整路径
  QList<FileInfo*> file_info_list_; // 当前目录下的文件/文件夹列表（每个 FileInfo 在堆上）
  explicit File(QWidget* parent = nullptr);
  ~File();
  void FlushFile();
  void UpdateFileList(QList<FileInfo*> file_info_list);
  void UploadFile();

 public slots:
  void UploadErrorBox(const QString& msg);

 private slots:
  void on_mkdir_PB_clicked();
  void on_flush_PB_clicked();
  void on_del_PB_clicked();
  void on_rename_PB_clicked();
  void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
  void on_return_PB_clicked();
  void on_upload_PB_clicked();

 private:
  Ui::File* ui_;
};

#endif  // FILE_H_
