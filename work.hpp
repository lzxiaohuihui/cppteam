//
// Created by lzh on 18/03/23.
//

#ifndef CODECRAFTSDK_WORK_HPP
#define CODECRAFTSDK_WORK_HPP

#include <bits/stdc++.h>
#include "util.hpp"

#include "Robot.hpp"
#include "Workbench.hpp"
#include "FindNearestWorkBenchService.hpp"
#include "CollisionService.hpp"
#include "BuyAndSellService.hpp"
#include "aStar.hpp"
#include "CollisionWallService.hpp"


class Work {
private:
    int money;

    vector<vector<int>> maze;

    vector<Robot *> robots;
    vector<Workbench *> workbenches;

    int countRobot = 0;
    int countWorkbench = 0;

    BuyAndSellService *buyAndSellService = new BuyAndSellService;
    FindNearestWorkBenchService *findNearestWorkBenchService = new FindNearestWorkBenchService();
    CollisionWallService *collisionService;
    aStar *pathPlanning;


public:

    Work() {
        maze = vector<vector<int>>(100, vector<int>(100, 0));
    }

    vector<string> schedulingRobot(int frameId) {
        vector<string> orders;

        for (const auto &robot: robots) {
            // 有没有目标都更新目标
            if (!robot->hasTarget() || false) {
                // 没有货
                if (robot->getCarry() == 0) {
                    findNearestWorkBenchService->findWorkbenchBuy(robot->getRobotId(), robots, workbenches);
                }
                    // 有货
                else {
                    findNearestWorkBenchService->findWorkbenchSell(robot->getRobotId(), robots, workbenches);
                }
            }

            // 到达目的地
            if (robot->hasTarget() && robot->getWorkbenchId() == robot->getTargetWorkBenchId()) {
                // 机器人当前所处的工作台
                int wId = robot->getWorkbenchId();
                // 买货
                if (robot->getCarry() == 0) {
                    bool isBuy = buyAndSellService->buy(robot, workbenches[wId]);
                    if (isBuy) {
                        // 买了货，机器人去新的目的地卖货
                        findNearestWorkBenchService->findWorkbenchSell(robot->getRobotId(), robots, workbenches);
                        // 判断够不够时间，然后再去买
                        if (robot->hasTarget() &&
                            isEnoughTime(*robot, *workbenches[robot->getTargetWorkBenchId()], frameId)) {
                            orders.push_back("buy " + to_string(robot->getRobotId()) + "\n");
                        } else {
                            buyAndSellService->cancelBuy(robot);
                        }
                    }

                }
                    // 卖货
                else {
                    bool isSell = buyAndSellService->sell(robot, workbenches[wId]);
                    if (isSell) {
                        robot->pidClear();
                        orders.push_back("sell " + to_string(robot->getRobotId()) + "\n");
                        // 卖了货，机器人去新的地方买货
                        findNearestWorkBenchService->findWorkbenchBuy(robot->getRobotId(), robots, workbenches);
                    }

                }
            }

            // 根据目标前往目的地
            if (robot->hasTarget()) {
                double workbenchX = workbenches[robot->getTargetWorkBenchId()]->getX();
                double workbenchY = workbenches[robot->getTargetWorkBenchId()]->getY();
                // 返回该机器人到目的地需要怎么走
                // orders->addAll(moveRobotService->pidMove(robot, 10, 10));
                vector<string> res = robot->pidMove(workbenchX, workbenchY);
                orders.insert(orders.end(), res.begin(), res.end());
            }

        }

        // 避免碰撞
        vector<string> a = collisionService->avoid(robots, workbenches);
        orders.insert(orders.end(), a.begin(), a.end());

        return orders;
    }


    vector<string> schedulingTargetWorkbench(int frameId){
        vector<string> orders;

        if (!robots[0]->hasPath()){
            double start_x = robots[0]->getX();
            double start_y = robots[0]->getY();
            double end_x = workbenches[20]->getX();
            double end_y = workbenches[20]->getY();
            vector<vector<double>> path;

            bool succeed = pathPlanning->find_path(start_x, start_y, end_x, end_y, path);
            if (!succeed) return orders;

            robots[0]->setPath(path);
        }

        vector<string> res = robots[0]->pathMove();
        orders.insert(orders.end(), res.begin(), res.end());

        return orders;

    }

    void init() {
        pathPlanning = new aStar(maze);
        collisionService = new CollisionWallService(maze);
    }

    static bool isEnoughTime(Robot& robot, Workbench workbench, int frameId){
        return Util::getDistance(robot, workbench) / 5.5 < (9000 - frameId) / 50.0;
    }

    void setDataLine(int num, const string& line){
        for(int i = 0; i < 100; ++i){
            char c = line[i];
            // 数字1-9 的 ASCII
            // 添加工作台信息

            if (c == '#'){
                maze[i][99-num] = 1;
            }

            if (c >= 49 && c <= 57){
                workbenches.push_back(new Workbench(countWorkbench++, c-'0', i/2.0, (99-num)/2.0));
            }

            if (c == 'A'){
                robots.push_back(new Robot(countRobot++, i/2.0, (99-num)/2.0));
            }

        }
    }

    void updateStatus(int num, const string& line) {
//        fprintf(stderr, "%s", line.data());
        auto n = workbenches.size();
        if(num < n){
            workbenches[num]->updateWorkbench(line);
        }
        else{
            robots[num - n]->updateRobot(line);
        }
    }

    vector<Robot*> getRobots(){
        return robots;
    }
    vector<Workbench*> getWorkbenches(){
        return workbenches;
    }
};


#endif //CODECRAFTSDK_WORK_HPP
