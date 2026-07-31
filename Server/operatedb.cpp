
// 数据库操作层实现：封装所有与 MariaDB 的交互，包括用户认证、好友管理等。
// 单例模式，通过 ODBC 驱动连接数据库。

#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperateDB &OperateDB::GetInstance() {
  static OperateDB instance;
  return instance;
}

// 使用 ODBC 驱动连接 MariaDB 数据库。
// 连接参数包括服务器地址、端口、数据库名及认证信息。
void OperateDB::connectSQL() {
  db_.setDatabaseName("DRIVER={MariaDB ODBC 3.2 Driver};"
                      "SERVER=localhost;"
                      "PORT=3306;"
                      "DATABASE=mydb;"
                      "USER=root;"
                      "PASSWORD=7766;");
  if (db_.open()) {
    qDebug() << "数据库连接成功";
  } else {
    qDebug() << "数据库连接失败" << db_.lastError().text();
  }
}

OperateDB::~OperateDB() {
  db_.close();
}

// 处理用户注册请求：先查重，再插入新记录。
// 若用户名已存在或 SQL 执行失败，返回 false。
bool OperateDB::handleRegist(const char *caName, const char *caPwd) {
  if (caName == nullptr || caPwd == nullptr) {
    return false;
  }

  // 第一步：检查用户名是否已被注册。
  QString sql = QString("select * from user_info where name='%1'").arg(caName);
  qDebug() << "查找用户是否存在 sql" << sql;
  QSqlQuery q;
  if (!q.exec(sql) || q.next()) {
    return false;  // 查询失败或用户已存在。
  }

  // 第二步：插入新用户记录。
  sql = QString("insert into user_info(name, pwd) values('%1', '%2')").arg(caName).arg(caPwd);
  qDebug() << "插入一条用户记录 sql" << sql;
  return q.exec(sql);
}

// 处理用户登录请求：验证用户名和密码，成功后更新在线状态。
// 验证通过返回 true，同时将 online 字段置 1。
bool OperateDB::handleLogin(const char *caName, const char *caPwd) {
  if (caName == nullptr || caPwd == nullptr) {
    return false;
  }

  // 第一步：验证用户名密码是否匹配。
  QString sql = QString("select * from user_info where name='%1' and pwd='%2'").arg(caName).arg(caPwd);
  qDebug() << "查找用户名和密码是否存在 sql" << sql;
  QSqlQuery q;
  if (!q.exec(sql) || !q.next()) {
    return false;  // 查询失败或用户名/密码不匹配。
  }

  // 第二步：登录成功，标记用户为在线状态。
  sql = QString("update user_info set online=1 where name='%1'").arg(caName);
  qDebug() << "将用户的online字段置为1 sql" << sql;
  return q.exec(sql);
}

// 将指定用户标记为离线（online = 0）。
void OperateDB::handleOffline(const char *caName) {
  if (caName == nullptr) {
    return;
  }
  QString sql = QString("update user_info set online=0 where name='%1'").arg(caName);
  qDebug() << "将用户的online字段置为0 sql" << sql;
  QSqlQuery q;
  bool ret = q.exec(sql);
  qDebug() << "handleOffline ret" << ret;
}

// 查找指定用户的在线状态。
// 返回值：
//   -1 — SQL 执行失败或参数为空。
//    0 — 用户不在线。
//    1 — 用户在线。
//    2 — 用户名不存在于 user_info 表中。
int OperateDB::handleFindUser(const char *caName) {
  if (caName == nullptr) {
    return -1;
  }
  QString sql = QString("select online from user_info where name='%1'").arg(caName);
  qDebug() << "查找用户的online字段 sql" << sql;
  QSqlQuery q;
  if (!q.exec(sql)) {
    return -1;  // SQL 执行失败。
  }
  if (q.next()) {
    return q.value(0).toInt();  // 返回 online 字段的实际值。
  }
  return 2;  // 查询结果为空，用户不存在。
}

// 查询所有在线用户的用户名列表。
QStringList OperateDB::handleOnlineUser() {
  QString sql = QString("select name from user_info where online=1");
  QSqlQuery q;
  q.exec(sql);
  QStringList res;
  while (q.next()) {
    res.append(q.value(0).toString());
  }
  return res;
}

// 处理添加好友请求：检查双方是否已是好友、目标用户是否存在及在线状态。
// 返回值：
//   -1 — 参数为空或目标用户不存在。
//   -2 — 双方已经是好友关系。
//    0 — 目标用户不在线。
//    1 — 目标用户在线，可以发送好友请求。
int OperateDB::handleAddFriend(const char *caCurName, const char *caTarName) {
  if (caCurName == nullptr || caTarName == nullptr) {
    return -1;
  }

  // 第一步：检查双方是否已是好友（双向查询）。
  QString sql = QString(R"(
                          select * from friend where
                          (
                            user_id=(select id from user_info where name='%1')
                            and
                            friend_id==(select id from user_info where name='%2')
                          )
                          or
                          (
                            user_id=(select id from user_info where name='%2')
                            and
                            friend_id==(select id from user_info where name='%1')
                          )
                          )").arg(caCurName).arg(caTarName);
  qDebug() << "判断是否是好友 sql" << sql;
  QSqlQuery q;
  q.exec(sql);
  if (q.next()) {
    return -2;  // 已是好友，无需重复添加。
  }

  // 第二步：查询目标用户的在线状态。
  sql = QString("select online from user_info where name='%1'").arg(caTarName);
  qDebug() << "查询用户是否在线 sql" << sql;
  q.exec(sql);
  if (q.next()) {
    return q.value(0).toInt();
  }
  return -1;  // 目标用户不存在。
}

// 同意好友申请：向 friend 表中插入一条好友关系记录。
// 通过 user_info 表的 name 字段反查 id，再写入关联关系。
bool OperateDB::handleAddFriendAgree(const char *caCurName, const char *caTarName) {
  if (caCurName == nullptr || caTarName == nullptr) {
    return false;
  }
  QString sql = QString(R"(
                          insert into friend(user_id, friend_id)
                          select u1.id, u2.id
                          from user_info u1, user_info u2
                          where u1.name='%1' and u2.name='%2'
                          )").arg(caCurName).arg(caTarName);
  QSqlQuery q;
  return q.exec(sql);
}

// 刷新好友列表：查询当前用户的所有好友中在线的用户。
// 由于好友关系是单向存储的，需要分别查询 user_id 和 friend_id 两个方向并去重。
QStringList OperateDB::handleFlushFriend(const char *caCurName) {
  QStringList res;
  if (caCurName == nullptr) {
    return res;
  }
  QString sql = QString(R"(
                          select name from user_info
                          where id in (
                            select user_id from friend where friend_id=(select id from user_info where name='%1')
                            union
                            select friend_id from friend where user_id=(select id from user_info where name='%1')
                          ) and online=1;
                          )").arg(caCurName);
  QSqlQuery q;
  q.exec(sql);
  while (q.next()) {
    res.append(q.value(0).toString());
  }
  return res;
}

// 删除好友关系：从 friend 表中删除双方的关联记录。
// 无论 user_id / friend_id 的存储方向，一并清除。
bool OperateDB::handleDelFriend(const char *caCurName, const char *caTarName) {
  if (caCurName == nullptr || caTarName == nullptr) {
    return false;
  }
  QString sql = QString(R"(
                          delete from friend where
                          (
                            user_id=(select id from user_info where name='%1')
                            and
                            friend_id=(select id from user_info where name='%2')
                          )
                          or
                          (
                            friend_id=(select id from user_info where name='%1')
                            and
                            user_id=(select id from user_info where name='%2')
                          );
                          )").arg(caCurName).arg(caTarName);
  QSqlQuery q;
  return q.exec(sql);
}

// 构造函数：注册 QODBC 数据库驱动，后续通过 connectSQL() 发起实际连接。
OperateDB::OperateDB(QObject *parent) : QObject(parent) {
  db_ = QSqlDatabase::addDatabase("QODBC");
}
