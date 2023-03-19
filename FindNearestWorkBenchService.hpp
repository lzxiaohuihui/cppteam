#include <bits/stdc++.h>
#include "util.hpp"

#include "Robot.hpp"
#include "Workbench.hpp"

using namespace std;

class FindNearestWorkBenchService {
public:
    void findWorkbenchSell(int robotId, vector<Robot*> robots, vector<Workbench*> workbenches) {
        auto robot = robots[robotId];

        // 找距离当前最近的工作台
        vector<Workbench*> workbenchList;
        for (const auto &workbench: workbenches){
            bool query = workbench->getType() > 3
                         && !workbench->getIsSellLock(robot->getCarry())
                         && workbench->needRawMaterial(robot->getCarry())
                         && !hasMaterialNum(robot->getCarry(), workbench->getRawMaterialStatus());
            if (!query) continue;
            workbenchList.push_back(workbench);
        }
        std::sort(workbenchList.begin(), workbenchList.end(), [robot](const Workbench* o1, const Workbench* o2){
            double d1 = Util::getDistance(o1->getX(), o1->getY(), robot->getX(), robot->getY());
            double d2 = Util::getDistance(o2->getX(), o2->getY(), robot->getX(), robot->getY());
            return d1 < d2;
        });


        if (workbenchList.empty()) {
            return;
        }
        for (auto& workbench: workbenchList) {

            if (robot->hasTarget()) {
                auto& originalTargetWorkbench = workbenches[robot->getTargetWorkBenchId()];
                double d1 = Util::getDistance(robot->getX(), robot->getY(), workbench->getX(), workbench->getY());
                double d2 = Util::getDistance(robot->getX(), robot->getY(), originalTargetWorkbench->getX(),
                                        originalTargetWorkbench->getY());
                if (d1 > d2) continue;
                originalTargetWorkbench->setSellLock(false, robot->getCarry());
            }

            int workbenchId = workbench->getWorkbenchId();
            robot->setTargetWorkBenchId(workbenchId);
            robot->pidClear();
            workbenches[workbenchId]->setSellLock(true, robot->getCarry());
            break;
        }

    }

    void findWorkbenchBuy(int robotId, vector<Robot*> robots, vector<Workbench*> workbenches) {
        auto robot = robots[robotId];
        // 找距离当前最近的工作台
        vector<Workbench*> workbenchList;
        for (const auto &workbench: workbenches){
            bool query = !workbench->getIsBuyLock() && workbench->getStatus()==1;
            if (!query) continue;
            workbenchList.push_back(workbench);
        }
        std::sort(workbenchList.begin(), workbenchList.end(), [robot](const Workbench* o1, const Workbench* o2){
            double d1 = Util::getDistance(o1->getX(), o1->getY(), robot->getX(), robot->getY());
            double d2 = Util::getDistance(o2->getX(), o2->getY(), robot->getX(), robot->getY());
            return d1 < d2;
        });


        if (workbenchList.empty()) {
            return;
        }

        for (auto& workbench: workbenchList) {

            int count = 0;
            for (const auto &w: workbenches){
                bool query1 = w->getType() > 3
                              && w->needRawMaterial(workbench->getType())
                              && !hasMaterialNum(workbench->getType(), w->getRawMaterialStatus())
                              && !w->getIsSellLock(workbench->getType());
                if (query1) count += 1;

            }

            int robotCarryNums = 0;
            for (const auto &r: robots){
                bool query2 = r->hasTarget()
                && r->getCarry()== 0
                && workbenches[r->getTargetWorkBenchId()]->getType()==workbench->getType();
                if (query2) robotCarryNums += 1;
            }

            if (count - robotCarryNums < 1) continue;

            int workbenchId = workbench->getWorkbenchId();

            if (robot->hasTarget()) {
                auto originalTargetWorkbench = workbenches[robot->getTargetWorkBenchId()];
                double d1 = Util::getDistance(robot->getX(), robot->getY(), workbench->getX(), workbench->getY());
                double d2 = Util::getDistance(robot->getX(), robot->getY(), originalTargetWorkbench->getX(),
                                        originalTargetWorkbench->getY());
                if (d1 > d2) continue;
                originalTargetWorkbench->setBuyLock(false);
            }

            robot->setTargetWorkBenchId(workbenchId);
            robot->pidClear();
            workbenches[workbenchId]->setBuyLock(true);
            break;
        }

    }
    bool hasMaterialNum(int num, int status) {
        return ((status >> (num)) & 1) == 1;
    }
};