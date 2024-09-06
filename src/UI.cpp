//
// Created by azazo1 on 2024/9/5.
//

#include "UI.h"

#include <esp_wifi.h>
#include <Preferences.h>
#include <sche/DelaySchedulable.h>
#include <sche/SchedulableFromLambda.h>

UI::UI(view::Screen *screen): screen(screen) {
}

void UI::buildMain() {
    mainFrame.addChild(&mainTs);

    mainTs.addItem("Network");
    mainTs.addItem("Arm Adjusting");

    mainTs.setOnConfirmListener([this](const size_t idx) {
        if (idx == 0) {
            screen->pushRootView(&networkFrame);
        } else if (idx == 1) {
            screen->pushRootView(&armAdjustingFrame);
        }
    });
    screen->pushRootView(&mainFrame);
}

void UI::buildNetwork(NetAdapter &netAdapter) {
    Preferences pref0;
    pref0.begin(ARM_PREF_NAMESPACE);
    staPwd = pref0.getString("sta_pwd", String());
    staSsid = pref0.getString("sta_ssid", String());
    apPwd = pref0.getString("ap_pwd", String(DEFAULT_AP_PWD));
    apSsid = pref0.getString("ap_ssid", String(DEFAULT_AP_SSID));
    pref0.end();
    networkFrame.addChild(&networkTs);

    networkTs.addItem("IP");
    networkTs.addItem("Mode AP");
    networkTs.addItem("Mode STA");
    networkTs.addItem("Reset");

    networkTs.setOnConfirmListener([this, &netAdapter](const size_t idx) {
        switch (idx) {
            case 0: {
                const bool *ap = netAdapter.getMode();
                String prefix = "null/";
                if (ap != nullptr) {
                    if (*ap) {
                        prefix = "ap:/";
                    } else {
                        prefix = "sta/";
                    }
                }
                ipText.setText(prefix + netAdapter.ip());
                screen->pushRootView(&ipFrame);
            }
            break;
            case 1: {
                editingAP = true;
                Preferences pref;
                pref.begin(ARM_PREF_NAMESPACE);
                apSsid = pref.getString("ap_ssid", String(DEFAULT_AP_SSID));
                apPwd = pref.getString("ap_pwd", String(DEFAULT_AP_PWD));
                pref.end();
                screen->pushRootView(&modeAPFrame);
            }
            break;
            case 2: {
                editingAP = false;
                Preferences pref;
                pref.begin(ARM_PREF_NAMESPACE);
                staSsid = pref.getString("sta_ssid", String());
                staPwd = pref.getString("sta_pwd", String());
                pref.end();
                screen->pushRootView(&modeSTAFrame);
            }
            break;
            case 3: {
                // 恢复出厂设置.
                Preferences pref1;
                pref1.begin(ARM_PREF_NAMESPACE);
                pref1.clear();
                pref1.end();
                esp_wifi_restore();
                ESP.restart();
            }
            default: ;
        }
    });

    ipFrame.addChild(&ipText);

    ssidInput.setOnDoneListener([this](const String &ssid) {
        if (editingAP) {
            apSsid = ssid;
        } else {
            staSsid = ssid;
        }
    });
    pwdInput.setOnDoneListener([this](const String &pwd) {
        if (editingAP) {
            apPwd = pwd;
        } else {
            staPwd = pwd;
        }
    });

    modeAPFrame.addChild(&apTs);
    apTs.addItem("SSID");
    apTs.addItem("Pwd");
    apTs.addItem("Apply");
    apTs.setOnConfirmListener([this, &netAdapter](const size_t idx) {
        switch (idx) {
            case 0: {
                ssidInput.setText(apSsid);
                screen->pushRootView(&ssidInput);
            }
            break;
            case 1: {
                pwdInput.setText(apPwd);
                screen->pushRootView(&pwdInput);
            }
            break;
            case 2: {
                if (apSsid.isEmpty()) {
                    // 显示错误信息.
                    apTs.setItemAt(2, "Error at SSID",
                                   screen->getDisplay(), screen->getScheduler());
                    postDelay(3000, [this] {
                        apTs.setItemAt(2, "Apply", screen->getDisplay(), screen->getScheduler());
                    });
                    return;
                }
                if (apPwd.length() < 8) {
                    apTs.setItemAt(2, "Error at Pwd",
                                   screen->getDisplay(), screen->getScheduler());
                    postDelay(3000, [this] {
                        apTs.setItemAt(2, "Apply",
                                       screen->getDisplay(), screen->getScheduler());
                    });
                    return;
                }
                screen->popRootView();
                // 切换网络模式
                netAdapter.modeAP(apSsid.c_str(), apPwd.c_str());
            }
            break;
            default: ;
        }
    });

    modeSTAFrame.addChild(&staTs);
    staTs.addItem("SSID");
    staTs.addItem("Pwd");
    staTs.addItem("Apply");
    staTs.setOnConfirmListener([this, &netAdapter](const size_t idx) {
        switch (idx) {
            case 0: {
                ssidInput.setText(staSsid);
                screen->pushRootView(&ssidInput);
            }
            break;
            case 1: {
                pwdInput.setText(staPwd);
                screen->pushRootView(&pwdInput);
            }
            break;
            case 2: {
                if (staSsid.isEmpty()) {
                    // 显示错误信息.
                    staTs.setItemAt(2, "Error at SSID",
                                    screen->getDisplay(), screen->getScheduler());
                    postDelay(3000, [this] {
                        staTs.setItemAt(2, "Apply",
                                        screen->getDisplay(), screen->getScheduler());
                    });
                    return;
                }
                if (staPwd.length() < 8) {
                    staTs.setItemAt(2, "Error at Pwd",
                                    screen->getDisplay(), screen->getScheduler());
                    postDelay(3000, [this] {
                        staTs.setItemAt(2, "Apply",
                                        screen->getDisplay(), screen->getScheduler());
                    });
                    return;
                }
                screen->popRootView();
                // 切换网络模式
                netAdapter.modeSTA(staSsid.c_str(), staPwd.c_str());
            }
            break;
            default: ;
        }
    });
}

void UI::buildArmAdjusting() {
    armAdjustingFrame.addChild(&armAdjustingTs);

    armAdjustingTs.addItem("xyz");
    armAdjustingTs.addItem("tab");
    armAdjustingTs.addItem("trz");

    armAdjustingTs.setOnConfirmListener([this](const size_t idx) {
        if (idx == 0) {
            screen->pushRootView(&xyzFrame);
        } else if (idx == 1) {
            screen->pushRootView(&tabFrame);
        } else if (idx == 2) {
            screen->pushRootView(&trzFrame);
        }
    });
}

void UI::buildXYZ(WritingArm &arm) {
    xyzFrame.addChild(&xyzTs);

    xyzTs.addItem("x");
    xyzTs.addItem("y");
    xyzTs.addItem("z");

    xyzTs.setOnConfirmListener([this, &arm](const size_t idx) {
        if (idx == 0) {
            const auto cur = static_cast<int16_t>(arm.getX());
            xFrame.setTitle(String("X ") + cur);
            xSb.setCurrent(cur);
            screen->pushRootView(&xFrame);
        } else if (idx == 1) {
            const auto cur = static_cast<int16_t>(arm.getY());
            yFrame.setTitle(String("Y ") + cur);
            ySb.setCurrent(cur);
            screen->pushRootView(&yFrame);
        } else if (idx == 2) {
            const auto cur = static_cast<int16_t>(arm.getZ());
            zSb1.setCurrent(cur);
            zFrame1.setTitle(String("Z ") + cur);
            screen->pushRootView(&zFrame1);
        }
    });

    xFrame.addChild(&xSb);
    yFrame.addChild(&ySb);
    zFrame1.addChild(&zSb1);

    xSb.setMin(0);
    xSb.setMax(148);
    xSb.setStep(1);
    ySb.setMin(0);
    ySb.setMax(210);
    ySb.setStep(1);
    zSb1.setMin(-15);
    zSb1.setMax(20);
    zSb1.setStep(1);

    xSb.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveTo(val, NAN, NAN);
        xFrame.setTitle(String("X ") + val);
    });
    xSb.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    ySb.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveTo(NAN, val, NAN);
        yFrame.setTitle(String("Y ") + val);
    });
    ySb.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    zSb1.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveTo(NAN, NAN, val);
        zFrame1.setTitle(String("Z ") + val);
    });
    zSb1.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
}

void UI::buildTAB(WritingArm &arm) {
    tabFrame.addChild(&tabTs);

    tabTs.addItem("theta");
    tabTs.addItem("alpha");
    tabTs.addItem("beta");

    tabTs.setOnConfirmListener([this, &arm](const size_t idx) {
        if (idx == 0) {
            const auto cur = static_cast<int16_t>(arm.getDegree(0));
            tSb1.setCurrent(cur);
            tFrame1.setTitle(String("Theta ") + cur);
            screen->pushRootView(&tFrame1);
        } else if (idx == 1) {
            const auto cur = static_cast<int16_t>(arm.getDegree(1));
            aSb.setCurrent(cur);
            aFrame.setTitle(String("Alpha ") + cur);
            screen->pushRootView(&aFrame);
        } else if (idx == 2) {
            const auto cur = static_cast<int16_t>(arm.getDegree(2));
            bSb.setCurrent(cur);
            bFrame.setTitle(String("Beta ") + cur);
            screen->pushRootView(&bFrame);
        }
    });

    tFrame1.addChild(&tSb1);
    aFrame.addChild(&aSb);
    bFrame.addChild(&bSb);

    tSb1.setMin(0);
    tSb1.setMax(180);
    tSb1.setStep(1);
    aSb.setMin(0);
    aSb.setMax(180);
    aSb.setStep(1);
    bSb.setMin(0);
    bSb.setMax(180);
    bSb.setStep(1);

    tSb1.setOnChangeListener([this, &arm](const int16_t val) {
        arm.setDegree(val, NAN, NAN);
        tFrame1.setTitle(String("Theta ") + val);
    });
    tSb1.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    aSb.setOnChangeListener([this, &arm](const int16_t val) {
        arm.setDegree(NAN, val, NAN);
        aFrame.setTitle(String("Alpha ") + val);
    });
    aSb.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    bSb.setOnChangeListener([this, &arm](const int16_t val) {
        arm.setDegree(NAN, NAN, val);
        bFrame.setTitle(String("Beta ") + val);
    });
    bSb.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
}

void UI::buildTRZ(WritingArm &arm) {
    trzFrame.addChild(&trzTs);

    trzTs.addItem("theta");
    trzTs.addItem("radius");
    trzTs.addItem("z");

    trzTs.setOnConfirmListener([this, &arm](const size_t idx) {
        if (idx == 0) {
            const auto cur = static_cast<int16_t>(arm.getDegree(0));
            tSb2.setCurrent(cur);
            tFrame2.setTitle(String("Theta ") + cur);
            screen->pushRootView(&tFrame2);
        } else if (idx == 1) {
            const auto cur = static_cast<int16_t>(arm.getR());
            rSb.setCurrent(cur);
            rFrame.setTitle(String("Radius ") + cur);
            screen->pushRootView(&rFrame);
        } else if (idx == 2) {
            const auto cur = static_cast<int16_t>(arm.getZ());
            zSb2.setCurrent(cur);
            zFrame2.setTitle(String("Z ") + cur);
            screen->pushRootView(&zFrame2);
        }
    });

    tFrame2.addChild(&tSb2);
    rFrame.addChild(&rSb);
    zFrame2.addChild(&zSb2);

    tSb2.setMin(0);
    tSb2.setMax(180);
    tSb2.setStep(1);
    rSb.setMin(0);
    rSb.setMax(202);
    rSb.setStep(1);
    zSb2.setMin(-15);
    zSb2.setMax(20);
    zSb2.setStep(1);

    tSb2.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveToPolar(val, NAN, NAN);
        tFrame2.setTitle(String("Theta ") + val);
    });
    tSb2.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    rSb.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveToPolar(NAN, val, NAN);
        rFrame.setTitle(String("Radius ") + val);
    });
    rSb.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
    zSb2.setOnChangeListener([this, &arm](const int16_t val) {
        arm.moveToPolar(NAN,NAN, val);
        zFrame2.setTitle(String("Z ") + val);
    });
    zSb2.setOnCancelListener([this](int16_t) {
        screen->popRootView();
    });
}

void UI::build(WritingArm &arm, NetAdapter &netAdapter) {
    if (built) {
        return;
    }
    buildMain();
    buildArmAdjusting();
    buildNetwork(netAdapter);
    buildXYZ(arm);
    buildTAB(arm);
    buildTRZ(arm);
    built = true;
}

void UI::postDelay(const sche::mtime_t delay, const std::function<void()> &action) const {
    screen->getScheduler().addSchedule(new sche::DelaySchedulable(
        delay, 0, new sche::SchedulableFromLambda([action](sche::mtime_t) {
            action();
            return false;
        })));
}
