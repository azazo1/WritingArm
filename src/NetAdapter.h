//
// Created by azazo1 on 2024/9/5.
//

#ifndef NETADAPTER_H
#define NETADAPTER_H

#include <sche/Scheduler.h>
#include <tiny_websockets/server.hpp>

#include "ArmController.h"

#define BROADCAST_PORT 13301

/// 动笔.
#define TYPE_MOVE_PEN ("m")
/// 落笔.
#define TYPE_DROP_PEN ("d")
/// 提笔.
#define TYPE_LIFT_PEN ("l")
/// 机械臂执行动作序列.
#define TYPE_ACTION_SEQUENCE ("a")
/// 切换到 AP 模式.
#define TYPE_AP_MODE ("p")
/// 切换到 STA 模式.
#define TYPE_STA_MODE ("s")

#define RESPOND_CODE_OK ("Ok")
#define RESPOND_CODE_ERROR ("Error")

/// 负责处理来自 websockets 的控制输入.
class NetAdapter {
    bool alive = true;
    const int port;

    /// 会在服务器启动或者重启的时候触发.
    std::function<void()> onServerStart = []() {
    };
    sche::Scheduler *const scheduler = nullptr;
    websockets::WebsocketsServer *server = nullptr;
    WiFiUDP *broadcastServer = nullptr;
    std::vector<websockets::WebsocketsClient> clients;
    ArmController *arm;
    /// nullptr 表示模式未定义, 适配器未完全初始化或处于模式更换期间.
    /// true 表示为 AP 模式.
    /// false 表示为 STA 模式.
    bool *ap = nullptr;

    /// 重启 websockets 服务器.
    void restartServer();

    void closeServer();

    /// 对服务器和客户端事件进行处理, 不阻塞.
    void poll();

    /// 接受到客户端的消息.
    void messageCallback(websockets::WebsocketsClient &client,
                         const websockets::WebsocketsMessage &message);

    const websockets::MessageCallback messageCallback_ = [&](
        websockets::WebsocketsClient &c, const websockets::WebsocketsMessage &m
    ) {
        messageCallback(c, m);
    };

public:
    NetAdapter(NetAdapter &) = delete;

    NetAdapter(NetAdapter &&) = delete;

    /// 创建一个 NetAdapter, 但是默认不进入两个模式中的任意一个.
    /// \param arm 要控制的机械臂, 不控制生命周期.
    /// \param port websockets 服务器的端口.
    /// \param scheduler 用它来负责检测 wifi 状况, 进行网络数据的传输, 不控制生命周期.
    NetAdapter(ArmController *arm, int port, sche::Scheduler *scheduler);

    /// 关闭 websockets 服务器, 断开原有 WiFi 连接, 重启 websockets 服务器, 并切换到 AP 模式, 自身发起 WiFi.
    /// 返回是否成功进入 AP 模式.
    bool modeAP(const char *ssid, const char *pwd);

    /// 关闭 websockets 服务器, 断开原有  WiFi 连接, 重启 websockets 服务器, 并切换到 STA 模式, 将连接到某个 WiFi.
    /// \param cb 回调函数.
    void modeSTA(const char *ssid, const char *pwd,
                 const std::function<void(bool)> &cb = [](bool) {
                 });

    /// 关闭 websockets 服务器和已经连接的客户端.
    ~NetAdapter();

    /// 获取当前 ip 地址.
    static String ip();

    void setOnServerStartCallback(std::function<void()> cb);
};


#endif //NETADAPTER_H
