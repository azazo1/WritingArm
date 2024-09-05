//
// Created by azazo1 on 2024/9/5.
//

#include "UI.h"

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
    netAdapter.setOnServerStartCallback();
    networkFrame.addChild(&networkTs);
    // todo 构建网络设置.
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
