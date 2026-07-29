#include "operatedb.h"
#include "server.h"
#include <QApplication>


int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  OperateDB::GetInstance().connectSQL();
  Server::GetInstance();
//    w.show();
  return a.exec();
}
