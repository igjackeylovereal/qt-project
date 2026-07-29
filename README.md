<p align="center">
  <h1 align="center">局域网即时通讯与文件分发系统</h1>
  <p align="center">基于 C++ / Qt 5.14 的 C/S 架构即时通讯系统</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/language-C%2B%2B11-blue" alt="language">
  <img src="https://img.shields.io/badge/Qt-5.14.2-green" alt="Qt">
  <img src="https://img.shields.io/badge/database-MySQL-orange" alt="MySQL">
  <img src="https://img.shields.io/badge/platform-Windows-lightgrey" alt="platform">
  <img src="https://img.shields.io/badge/license-MIT-yellow" alt="license">
</p>

---

## 项目简介

本项目从零实现了一个类 QQ 的局域网即时通讯系统，采用 C/S 架构，包含完整的**注册登录、好友管理、私聊、在线用户列表和文件传输**功能。自定义二进制应用层协议解决 TCP 粘包/拆包问题，服务端使用线程池处理多客户端并发连接。

**技术栈**：C++11、Qt Network、MySQL、QThreadPool

---


## 系统架构

```
┌─────────────────────────────┐
│         Client 客户端         │
│  client | index | friend     │
│  chat | onlineuser | file    │
│  uploader | reshandler       │
└──────────────┬──────────────┘
               │  TCP (自定义 PDU 协议)
┌──────────────┴──────────────┐
│         Server 服务端         │
│  server | mytcpserver        │
│  mytcpsocket | msghandler    │
│  operatedb | clienttask      │
└──────────────┬──────────────┘
               │
┌──────────────┴──────────────┐
│           MySQL 数据库        │
└──────────────────────────────┘
```

---

## 快速开始

### 环境要求

- **Qt** 5.14.2（MinGW 7.3 64-bit）
- **MySQL** 5.7+
- **Windows** 10/11

### 构建

```bash
# 服务端
cd Server
qmake Server.pro
mingw32-make

# 客户端
cd Client
qmake Client.pro
mingw32-make
```

### 配置文件

`Client/connect.config` 和 `Server/connect.config` 格式相同：

```
127.0.0.1       # 服务端 IP
5000            # 端口号
D:/server_root  # 文件存储根目录
```

### 运行

1. 启动 MySQL，创建数据库和用户表
2. 启动 `Server.exe`
3. 启动 `Client.exe` → 注册 → 登录

---

## 功能清单

- 用户注册与登录（输入校验、状态管理）
- 查找用户（判断存在性、在线状态）
- 在线用户列表（实时刷新、双击添加好友）
- 好友管理（双向确认添加、删除、列表刷新）
- 私聊（一对一实时消息）
- 文件系统（目录浏览、新建文件夹、删除、重命名）
- 文件上传（分片传输、异步发送、跨线程安全）

---

## 协议设计

自定义二进制应用层协议，解决 TCP 字节流传输中的粘包与拆包问题。

### PDU 结构体

```cpp
struct PDU {
    uint uiTotalLen;    // 总长度 = sizeof(PDU) + uiMsgLen
    uint uiMsgLen;      // 柔性数组长度
    uint uiType;        // 消息类型（枚举值）
    char caData[64];    // 固定 64 字节参数区
    char caMsg[];       // 柔性数组，变长消息体
};
```

**设计要点**：
- `uiTotalLen` 用于接收端判断一条消息是否完整接收（粘包处理的核心）
- `caData[64]` 用于短参数（用户名、密码），`caMsg[]` 用于长数据（聊天内容、文件路径）
- 消息类型成对设计：`REQUEST`（客户端→服务端）与 `RESPOND`（服务端→客户端）

### 消息类型

| 业务 | REQUEST | RESPOND |
|------|---------|---------|
| 注册 | `REGIST_REQUEST` | `REGIST_RESPOND` |
| 登录 | `LOGIN_REQUEST` | `LOGIN_RESPOND` |
| 添加好友 | `ADD_FRIEND_REQUEST` | `ADD_FRIEND_RESPOND` |
| 聊天 | `CHAT_REQUEST` | `CHAT_RESEND` |
| 文件操作 | `MKDIR_REQUEST` 等 | `MKDIR_RESPOND` 等 |

---

## 客户端模块

| 模块 | 文件 | 职责 |
|------|------|------|
| 接入与会话 | `client.h/.cpp` | TCP 连接、收发消息、粘包/半包处理 |
| 响应处理 | `reshandler.h/.cpp` | 服务端回复分流到 UI 动作 |
| 主界面编排 | `index.h/.cpp` | 好友页/文件页导航切换 |
| 好友管理 | `friend.h/.cpp` | 查找用户、添加/删除好友 |
| 在线用户 | `onlineuser.h/.cpp` | 在线用户列表、双击添加好友 |
| 即时聊天 | `chat.h/.cpp` | 私聊窗口、消息收发 |
| 文件管理 | `file.h/.cpp` | 目录浏览、文件操作、上传 |
| 上传辅助 | `uploader.h/.cpp` | 异步分片发送 |

### 消息收发流程

```
on_xxx_clicked()      → 收集输入 → 构造 PDU → 设置 uiType
        ↓
sendMsg(pdu)          → socket.write() → free(pdu)
        ↓
    [TCP 传输]
        ↓
recvMsg()             → readAll → buffer.append → while 循环拆包
        ↓                         ↑
handleMsg(pdu)        → switch(uiType) → ResHandler::xxx()
        ↓
ResHandler            → 取结果 → 更新 UI
```

**粘包处理核心**：`recvMsg()` 中用 `QByteArray buffer` 积累数据，通过 `uiTotalLen` 判断单条消息是否完整，不完整则 `break` 等待下次触发。

---

## 服务端模块

| 模块 | 文件 | 职责 |
|------|------|------|
| 启动配置 | `server.h/.cpp` | 读配置、启动监听 |
| 连接接入 | `mytcpserver.h/.cpp` | 接收连接、维护在线列表、消息转发 |
| 连接会话 | `mytcpsocket.h/.cpp` | 每连接收发、拆包、业务分派 |
| 业务处理 | `msghandler.h/.cpp` | 注册、登录、好友、聊天、文件请求处理 |
| 数据库 | `operatedb.h/.cpp` | MySQL 增删改查 |
| 任务调度 | `clienttask.h/.cpp` | QRunnable + QThreadPool 并发 |

### 请求处理流程

```
incomingConnection()  → new MyTcpSocket → 加入在线列表 → 丢进线程池
        ↓
recvMsg()             → readAll → buffer → while 拆包
        ↓
handleMsg()           → switch(uiType) → MsgHandler::xxx()
        ↓
业务处理              → 查 MySQL / 操作文件系统
        ↓
构造 RESPOND PDU      → sendMsg 回复 / resend 转发
```



---



