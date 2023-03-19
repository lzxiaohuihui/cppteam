
#include <bits/stdc++.h>
#include "Robot.hpp"
#include "Workbench.hpp"

using namespace std;


class BuyAndSellService {

public :
    double losePriceSum = 0;

    void cancelBuy(Robot* robot) {

        robot->setCarry(0);
        robot->setTargetWorkBenchId(-1);
    }

    bool buy(Robot* robot, Workbench* workbench) {
        // 判断当前工作台是否有产品可以买
        if (workbench->getStatus() == 0) {
            return false;
        }

        robot->setCarry(workbench->getType());
        workbench->setStatus(0);
        robot->setTargetWorkBenchId(-1);
        workbench->setBuyLock(false);

        return true;
    }

    bool sell(Robot* robot, Workbench* workbench) {
        // 当前工作台不需要这个货
        if (!workbench->needRawMaterial(robot->getCarry())) {
            return false;
        }
        // 有这个材料了，等待生产消耗掉这个原材料
        if (hasMaterialNum(robot->getCarry(), workbench->getRawMaterialStatus())) {
            return false;
        }
//        losePriceSum += (1 - robot->getCollisionValue()) * originPrices[robot->getCarry()] * robot->getTimeValue();
//        System->err->printf("第%d次出售-------时间价值系数: %f -------- 碰撞价值系数: %f ------- 碰撞损失： %f\n"
//                ,++sellTimes, robot->getTimeValue(), robot->getCollisionValue(), losePriceSum);
        workbench->setSellLock(false, robot->getCarry());
        workbench->setRawMaterialStatus(Util::addWorkbenchStatus(robot->getCarry(), workbench));
        robot->setCarry(0);
        robot->setTargetWorkBenchId(-1);

        return true;
    }

    // 判断工作台status中是否有指定的原材料

    static bool hasMaterialNum(int num, int status) {
        return ((status >> (num)) & 1) == 1;
    }
};