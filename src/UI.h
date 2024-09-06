//
// Created by azazo1 on 2024/9/5.
//

#ifndef UI_H
#define UI_H
#include <NetAdapter.h>
#include <view/LabeledFrame.h>
#include <view/Screen.h>
#include <view/Seekbar.h>
#include <view/TextInput.h>
#include <view/TextSelector.h>
#include <view/TextView.h>

#define DEFAULT_AP_SSID "ESP32-WritingArm"
#define DEFAULT_AP_PWD "eesspp3322"

class UI {
    bool built = false;
    /// 用于输入框分辨当前正在编辑 ap 模式的 ssid/pwd 还是 sta 模式的 ssid/pwd.
    bool editingAP = false;
    // 在确认启动 ap 模式或者 sta 模式时会保存对应的值.
    /// ap 模式下的 WiFi 名称.
    ///
    /// build 时会读取单片机中的保存值, 如果没有保存值, 那么使用默认值.
    String apSsid = "";
    /// ap 模式下的 WiFi 密码.
    ///
    /// build 时会读取单片机的保存值, 如果没有保存值, 那么使用默认值.
    String apPwd = "";
    /// sta 模式下的 WiFi 名称.
    ///
    /// build 时会读取单片机的保存值, 如果没有保存值, 那么将保持为空.
    String staSsid = "";
    /// sta 模式下的 WiFi 密码.
    ///
    /// build 时会读取单片机的保存值, 如果没有保存值, 那么将保持为空.
    String staPwd = "";

    view::Screen *screen;
    view::LabeledFrame mainFrame = view::LabeledFrame("Writing Arm");
    view::TextSelector mainTs = view::TextSelector();

    view::LabeledFrame networkFrame = view::LabeledFrame("Network");
    view::TextSelector networkTs = view::TextSelector();

    view::LabeledFrame ipFrame = view::LabeledFrame("IP Address");
    view::TextView ipText = view::TextView("null");

    view::LabeledFrame modeAPFrame = view::LabeledFrame("Mode AP");
    view::TextSelector apTs = view::TextSelector();

    view::LabeledFrame modeSTAFrame = view::LabeledFrame("Mode STA");
    view::TextSelector staTs = view::TextSelector();

    view::TextInput ssidInput = view::TextInput();
    view::TextInput pwdInput = view::TextInput();

    view::LabeledFrame armAdjustingFrame = view::LabeledFrame("Arm Adjusting");
    view::TextSelector armAdjustingTs = view::TextSelector();

    view::LabeledFrame xyzFrame = view::LabeledFrame("XYZ");
    view::TextSelector xyzTs = view::TextSelector();

    view::LabeledFrame tabFrame = view::LabeledFrame("Theta Alpha Beta");
    view::TextSelector tabTs = view::TextSelector();

    view::LabeledFrame trzFrame = view::LabeledFrame("Theta Radius Z");
    view::TextSelector trzTs = view::TextSelector();

    view::LabeledFrame xFrame = view::LabeledFrame("X");
    view::Seekbar xSb = view::Seekbar();
    view::LabeledFrame yFrame = view::LabeledFrame("Y");
    view::Seekbar ySb = view::Seekbar();
    view::LabeledFrame zFrame1 = view::LabeledFrame("Z");
    view::Seekbar zSb1 = view::Seekbar();

    view::LabeledFrame tFrame1 = view::LabeledFrame("Theta");
    view::Seekbar tSb1 = view::Seekbar();
    view::LabeledFrame aFrame = view::LabeledFrame("Alpha");
    view::Seekbar aSb = view::Seekbar();
    view::LabeledFrame bFrame = view::LabeledFrame("Beta");
    view::Seekbar bSb = view::Seekbar();

    view::LabeledFrame tFrame2 = view::LabeledFrame("Theta");
    view::Seekbar tSb2 = view::Seekbar();
    view::LabeledFrame rFrame = view::LabeledFrame("Radius");
    view::Seekbar rSb = view::Seekbar();
    view::LabeledFrame zFrame2 = view::LabeledFrame("Z");
    view::Seekbar zSb2 = view::Seekbar();

    void buildMain();

    void buildNetwork(NetAdapter &netAdapter);

    void buildArmAdjusting();

    void buildXYZ(WritingArm &arm);

    void buildTAB(WritingArm &arm);

    void buildTRZ(WritingArm &arm);

    /// 把一个任务通过 scheduler 推迟到一定时间后运行.
    void postDelay(sche::mtime_t delay, const std::function<void()>& action) const;

public:
    /// 创建 UI 对象.
    /// \param screen 屏幕对象, 将会把 UI 部署到此对象上, 不管理其生命周期.
    explicit UI(view::Screen *screen);

    /// 把 UI 构建到 Screen 上.
    /// \param arm 机械臂, 用于读取机械臂的数值.
    /// \param netAdapter 用于注册服务器启动时 IP 更改的事件.
    void build(WritingArm &arm, NetAdapter &netAdapter);
};


#endif //UI_H
