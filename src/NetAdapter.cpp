//
// Created by azazo1 on 2024/9/5.
//

#include "NetAdapter.h"

#include <Preferences.h>
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
    broadcastServer = new WiFiUDP();
    broadcastServer->begin(BROADCAST_PORT);
    onServerStart();
}

void NetAdapter::closeServer() {
    if (broadcastServer != nullptr && broadcastServer->available()) {
        broadcastServer->stop();
    }
    delete broadcastServer;
    broadcastServer = nullptr;
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
        Preferences pref1;
        pref1.begin(ARM_PREF_NAMESPACE);
        pref1.putString("ap_ssid", ssid);
        pref1.putString("ap_pwd", pwd);
        pref1.end();
        restartServer();
        ap = new bool(true);
        return true;
    }
    return false;
}

void NetAdapter::modeSTA(const char *ssid, const char *pwd,
                         const std::function<void(bool)> &cb) {
    closeServer();
    if (ap != nullptr && *ap) {
        WiFi.softAPdisconnect(); // 不能凭空调用, 要保证处于 ap 状态才调用.
    }
    delete ap;
    ap = nullptr;
    WiFi.begin(ssid, pwd);
    // 防止字符串被销毁.
    String ssid_ = ssid;
    String pwd_ = pwd;
    scheduler->addSchedule(new sche::DelaySchedulable( // 直到状态成功或失败时触发回调函数.
            0, 100, new sche::SchedulableFromLambda([cb, this, ssid_, pwd_](const sche::mtime_t) {
                // 注意不能忘记捕获 cb.
                if (WiFiClass::status() == WL_CONNECTED) {
                    Preferences pref1;
                    pref1.begin(ARM_PREF_NAMESPACE);
                    pref1.putString("sta_ssid", ssid_);
                    pref1.putString("sta_pwd", pwd_);
                    pref1.end();
                    restartServer();
                    ap = new bool(false);
                    cb(true);
                    return false;
                }
                if (WiFiClass::status() == WL_CONNECT_FAILED) {
                    cb(true);
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
    if (broadcastServer->parsePacket()) {
        char buffer[255];
        const int len = broadcastServer->read(buffer, 255);
        if (len > 0) {
            buffer[len] = 0;
        }
        if (strcmp(buffer, "ping") == 0) {
            broadcastServer->beginPacket();
            String portStr = String() + port;
            if (portStr.length() < 5) {
                portStr = "0" + portStr;
            }
            const String respond = String("pong") + portStr;
            const auto respond_c_str = reinterpret_cast<const uint8_t *>(respond.c_str());
            broadcastServer->write(respond_c_str, respond.length());
            broadcastServer->endPacket();
            broadcastServer->flush();
        }
    }
    for (int i = 0; i < clients.size(); ++i) {
        websockets::WebsocketsClient &client = clients.at(i);
        if (!client.available()) {
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
    const String type = doc["type"];
    const JsonVariant args = doc["args"];
    // Serial.print("Get cmd type: ");
    // Serial.println(type);
    JsonDocument respondDoc;
    if (type == TYPE_LIFT_PEN) {
        arm->liftPen();
        respondDoc["code"] = RESPOND_CODE_OK;
    } else if (type == TYPE_DROP_PEN) {
        if (args.isNull() || args["strength"].isNull()) {
            respondDoc["code"] = RESPOND_CODE_ERROR;
        } else {
            arm->dropPen(args["strength"]);
            respondDoc["code"] = RESPOND_CODE_OK;
        }
    } else if (type == TYPE_MOVE_PEN) {
        if (args.isNull() || args["x"].isNull() || args["y"].isNull()) {
            respondDoc["code"] = RESPOND_CODE_ERROR;
        } else {
            arm->movePen(args["x"], args["y"]);
            respondDoc["code"] = RESPOND_CODE_OK;
        }
    } else if (type == TYPE_ACTION_SEQUENCE) {
        // actions 是个列表, 每项是个 action, action 有三个元素 x, y, pressure; pressure 为 0 时表示起笔.
        if (args.isNull() || args["actions"].isNull()) {
            respondDoc["code"] = RESPOND_CODE_ERROR;
        } else {
            JsonArray actions = args["actions"];
            std::vector<double> sequence;
            sequence.reserve(actions.size() * 3);
            for (JsonArray action: actions) {
                for (double v: action) {
                    sequence.push_back(v);
                }
            }
            arm->drawActionSequence(std::move(sequence), *scheduler);
            respondDoc["code"] = RESPOND_CODE_OK;
        }
    } else if (type == TYPE_AP_MODE) {
        if (args.isNull() || args["ssid"].isNull() || args["pwd"].isNull()) {
            respondDoc["code"] = RESPOND_CODE_ERROR;
        } else {
            // 这里的 String 不能换成 const char *, 因为后者无法被 lambda 捕获.
            String ssid = args["ssid"];
            String pwd = args["pwd"];
            scheduler->addSchedule(new sche::SchedulableFromLambda(
                [this, pwd, ssid](sche::mtime_t) {
                    modeAP(ssid.c_str(), pwd.c_str());
                    // 不能在这里进行网络的回复, 因为获取切换结果前网络状态就已经变化了.
                    return false;
                }));
            respondDoc["code"] = RESPOND_CODE_OK;
        }
    } else if (type == TYPE_STA_MODE) {
        if (args.isNull() || args["ssid"].isNull() || args["pwd"].isNull()) {
            respondDoc["code"] = RESPOND_CODE_ERROR;
        } else {
            String ssid = args["ssid"];
            String pwd = args["pwd"];
            scheduler->addSchedule(new sche::SchedulableFromLambda(
                [this, ssid, pwd](sche::mtime_t) {
                    modeSTA(ssid.c_str(), pwd.c_str());
                    return false;
                }));
            respondDoc["code"] = RESPOND_CODE_OK;
        }
    }
    String rst;
    serializeJson(respondDoc, rst);
    client.send(rst);
    // Serial.print("Respond: ");
    // Serial.println(rst);
}

String NetAdapter::ip() const {
    if (ap == nullptr) {
        return "";
    }
    if (*ap) {
        return WiFi.softAPIP().toString();
    }
    return WiFi.localIP().toString();
}

void NetAdapter::setOnServerStartCallback(std::function<void()> cb) {
    onServerStart = std::move(cb);
}

const bool *NetAdapter::getMode() const {
    return ap;
}
