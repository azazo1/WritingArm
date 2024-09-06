//
// Created by azazo1 on 2024/9/5.
//

#ifndef ARMCONTROLLER_H
#define ARMCONTROLLER_H
#include <WritingArm.h>


/// 负责机械臂的高级行为.
/// todo 根据动作序列(类似gcode)来控制机械臂.
/// todo 设置基本坐标, 然后通过小偏移量来实现在指定位置书写文字.
class ArmController {
    WritingArm *const arm = nullptr;

public:
    /// 创建机械臂控制器.
    /// \param arm 机械臂驱动实例, 不控制生命周期.
    explicit ArmController(WritingArm *arm);

    /// 落笔, 可以反复调用此方法来控制笔的强度.
    /// \param strength 落笔强度, 范围为 0~1, 超过范围的将自动取边界.
    void dropPen(double strength) const;

    /// 提笔.
    void liftPen() const;

    /// 把笔移动到纸面 xy 坐标.
    void movePen(double x, double y) const;
};


#endif //ARMCONTROLLER_H
