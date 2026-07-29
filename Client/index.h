#ifndef INDEX_H_
#define INDEX_H_

#include "file.h"
#include "friend.h"

#include <QWidget>

namespace Ui {
class Index;
}

class Index : public QWidget {
  Q_OBJECT

 public:
  ~Index();
  Friend* GetFriend();
  File* GetFile();
  static Index& GetInstance();

 private slots:
  void on_friend_PB_clicked();
  void on_file_PB_clicked();

 private:
  Ui::Index* ui_;
  explicit Index(QWidget* parent = nullptr);
  Index(const Index& instance) = delete;
  Index& operator=(const Index&) = delete;
};

#endif  // INDEX_H_
