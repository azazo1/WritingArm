//
// Created by azazo1 on 2024/9/5.
//

#include "NetAdapter.h"

#include <sche/DelaySchedulable.h>
#include <sche/Scheduler.h>
#include <sche/SequenceSchedulable.h>
#include <ArduinoJson.h>

NetAdapter::NetAdapter(ArmController *arm, const int port, sche::Scheduler *scheduler)
    : port(port), scheduler(scheduler), arm(arm) {
    scheduler->addSchedule(new sche::SchedulableFromLambda([&](sche::mtime_t) {
        poll();
        return alive;
    }));
}

void NetAdapter::restartServer() {
    closeServer();
    server = new websockets::WebsocketsServer();
    server->listen(port);
}

void NetAdapter::closeServer() {
    delete server;
    server = nullptr;
    clients.clear();
}

bool NetAdapter::modeAP(const char *ssid, const char *pwd) {
    closeServer();
    WiFi.disconnect();
    delete ap;
    ap = nullptr;
    if (WiFi.softAP(ssid, pwd)) {
        restartServer();
        ap = new bool(true);
        return true;
    }
    return false;
}

void NetAdapter::modeSTA(const char *ssid, const char *pwd,
                         const std::function<void(bool)> &cb) {
    closeServer();
    WiFi.softAPdisconnect();
    delete ap;
    ap = nullptr;
    WiFi.begin(ssid, pwd);
    scheduler->addSchedule(new sche::DelaySchedulable( // 直到状态成功或失败时触发回调函数.
            0, 100, new sche::SchedulableFromLambda([&](const sche::mtime_t) {
                if (WiFiClass::status() == WL_CONNECTED) {
                    cb(true);
                    ap = new bool(true);
                    return false;
                }
                if (WiFiClass::status() == WL_CONNECT_FAILED) {
                    cb(false);
                    return false;
                }
                return true;
            }))
    );
}

NetAdapter::~NetAdapter() {
    alive = false;
    delete ap;
    delete server;
    clients.clear();
}

void NetAdapter::poll() {
    if (!alive) {
        return;
    }
    if (server == nullptr || !server->available()) {
        return;
    }
    if (server->poll()) {
        websockets::WebsocketsClient client = server->accept();
        client.onMessage(messageCallback_);
        clients.push_back(client);
    }
    for (int i = 0; i < clients.size(); ++i) {
        websockets::WebsocketsClient &client = clients.at(i);
        if (!client.available(true)) {
            client.close();
            clients.erase(clients.begin() + i);
        }
        // client.poll 会自动调用消息回调函数.
        client.poll();
    }
}

void NetAdapter::messageCallback(
    websockets::WebsocketsClient &client,
    const websockets::WebsocketsMessage &message) {
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, message.data());
    if (error) {
        Serial.println("Deserialing json failed.");
        return;
    }
    const String v = doc["type"];
    Serial.println(v);
    client.send("Ok");
}
