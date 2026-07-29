#include "index.h"
#include "ui_index.h"

Index::Index(QWidget* parent)
    : QWidget(parent)
    , ui_(new Ui::Index) {
  ui_->setupUi(this);
}

Index::~Index() {
  delete ui_;
}

Friend* Index::GetFriend() {
  return ui_->friendPage;
}

File* Index::GetFile() {
  return ui_->filePage;
}

Index& Index::GetInstance() {
  static Index instance;
  return instance;
}

void Index::on_friend_PB_clicked() {
  ui_->stackedWidget->setCurrentIndex(0);
}

void Index::on_file_PB_clicked() {
  ui_->stackedWidget->setCurrentIndex(1);
}
