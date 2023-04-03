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
#include "FindWorkBenchService.hpp"


class Work {
private:
    int money;

    vector<vector<int>> maze;

    vector<Robot *> robots;
    vector<Workbench *> workbenches;

    int countRobot = 0;
    int countWorkbench = 0;

    BuyAndSellService *buyAndSellService = new BuyAndSellService;
//    FindNearestWorkBenchService *findNearestWorkBenchService = new FindNearestWorkBenchService();
    FindWorkBenchService *findWorkBenchService;
    CollisionWallService *collisionService;
    aStar *pathPlanning;
    map<pair<int, int>, vector<vector<double>>> paths;
    unordered_map<int, vector<int>> sellTargets;
    unordered_map<int, vector<int>> buyTargets;
    vector<vector<Workbench *>> typeWorkbenches;


public:

    Work() {
        maze = vector<vector<int>>(100, vector<int>(100, 0));
    }

    void init() {
        fprintf(stderr, "init pathPlanning.");
        pathPlanning = new aStar(maze);
        fprintf(stderr, "[OK]\n");

        fprintf(stderr, "init sellTargets.");
        sellTargets.insert(make_pair<int, vector<int>>(1, {4, 5, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(2, {4, 6, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(3, {5, 6, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(4, {7, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(5, {7, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(6, {7, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(7, {8, 9}));
        sellTargets.insert(make_pair<int, vector<int>>(8, {}));
        sellTargets.insert(make_pair<int, vector<int>>(9, {}));
        fprintf(stderr, "[OK]\n");


        fprintf(stderr, "init buyTargets.");
        buyTargets.insert(make_pair<int, vector<int>>(1, {}));
        buyTargets.insert(make_pair<int, vector<int>>(2, {}));
        buyTargets.insert(make_pair<int, vector<int>>(3, {}));
        buyTargets.insert(make_pair<int, vector<int>>(4, {1, 2}));
        buyTargets.insert(make_pair<int, vector<int>>(5, {1, 3}));
        buyTargets.insert(make_pair<int, vector<int>>(6, {2, 3}));
        buyTargets.insert(make_pair<int, vector<int>>(7, {4, 5, 6}));
        buyTargets.insert(make_pair<int, vector<int>>(8, {7}));
        buyTargets.insert(make_pair<int, vector<int>>(9, {1, 2, 3, 4, 5, 6, 7}));
        fprintf(stderr, "[OK]\n");


        fprintf(stderr, "init typeWorkbenches.");
        for(int i = 0; i <= 10; i++) typeWorkbenches.emplace_back();
        for (const auto &item: workbenches) {
            typeWorkbenches[item->getType()].push_back(item);
        }
        fprintf(stderr, "[OK]\n");


        fprintf(stderr, "init paths.");
        // workbench --> w
        for (const auto &workbench: workbenches) {
            for (const auto &w: workbenches) {
                if (workbench==w) continue;
                vector<vector<double>> res;
                bool succeed = pathPlanning->find_path(workbench->getX(), workbench->getY(), w->getX(), w->getY(),
                                                       res);
                if (succeed) {
                    pair<int,int> p(workbench->getWorkbenchId(), w->getWorkbenchId());
                    paths.insert(make_pair(p, res));
                }
            }
        }
        fprintf(stderr, "[OK]\n");

        fprintf(stderr, "init collisionService.");
        collisionService = new CollisionWallService(maze);
        fprintf(stderr, "[OK]\n");

        fprintf(stderr, "init findWorkBenchService.");
        findWorkBenchService = new FindNearestWorkBenchService(robots, workbenches, sellTargets, buyTargets,
                                                               typeWorkbenches, paths, pathPlanning);
        fprintf(stderr, "[OK]\n");

    }

    vector<string> schedulingRobot(int frameId) {
        vector<string> orders;

        for (const auto &robot: robots) {
            // 有没有目标都更新目标
            if (!robot->hasTarget() || false) {
                // 没有货
                if (robot->getCarry() == 0) {
                    findWorkBenchService->findWorkbenchBuy(robot->getRobotId(), frameId);
                }
                    // 有货
                else {
                    findWorkBenchService->findWorkbenchSell(robot->getRobotId(), frameId);
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
                        robot->pidClear();
                        orders.push_back("buy " + to_string(robot->getRobotId()) + "\n");
                        findWorkBenchService->findWorkbenchSell(robot->getRobotId(), frameId);
//                        // 买了货，机器人去新的目的地卖货
//                        findWorkBenchService->findWorkbenchSell(robot->getRobotId(), frameId);
//                        // 判断够不够时间，然后再去买
//                        if (robot->hasTarget() &&
//                            isEnoughTime(*robot, *workbenches[robot->getTargetWorkBenchId()], frameId)) {
//                            orders.push_back("buy " + to_string(robot->getRobotId()) + "\n");
//                        } else {
//                            buyAndSellService->cancelBuy(robot);
//                        }

                    }

                }
                    // 卖货
                else {
                    bool isSell = buyAndSellService->sell(robot, workbenches[wId]);
                    if (isSell) {
                        robot->pidClear();
                        orders.push_back("sell " + to_string(robot->getRobotId()) + "\n");
                        // 卖了货，机器人去新的地方买货
                        findWorkBenchService->findWorkbenchBuy(robot->getRobotId(), frameId);
                    }

                }
            }

            // 根据目标前往目的地
            if (robot->hasTarget()) {
//                double workbenchX = workbenches[robot->getTargetWorkBenchId()]->getX();
//                double workbenchY = workbenches[robot->getTargetWorkBenchId()]->getY();
                // 返回该机器人到目的地需要怎么走
                // orders->addAll(moveRobotService->pidMove(robot, 10, 10));
//                vector<string> res = robot->pidMove(workbenchX, workbenchY);
                vector<string> res = robot->pathMove();
                orders.insert(orders.end(), res.begin(), res.end());
            }

        }

        // 避免碰撞
//        vector<string> a = collisionService->avoid(robots, workbenches);
//        orders.insert(orders.end(), a.begin(), a.end());

        return orders;
    }


    vector<string> schedulingTargetWorkbench(int frameId) {
        vector<string> orders;

        if (!robots[0]->hasPath()) {
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


    static bool isEnoughTime(Robot &robot, Workbench workbench, int frameId) {
        return Util::getDistance(robot, workbench) / 5.5 < (15000 - frameId) / 50.0;
    }

    void setDataLine(int num, const string &line) {
        for (int i = 0; i < 100; ++i) {
            char c = line[i];
            // 数字1-9 的 ASCII
            // 添加工作台信息

            if (c == '#') {
                maze[i][99 - num] = 1;
            }

            if (c >= 49 && c <= 57) {
                workbenches.push_back(new Workbench(countWorkbench++, c - '0', i / 2.0, (99 - num) / 2.0));
            }

            if (c == 'A') {
                robots.push_back(new Robot(countRobot++, i / 2.0, (99 - num) / 2.0));
            }

        }
    }

    void updateStatus(int num, const string &line) {
//        fprintf(stderr, "%s", line.data());
        auto n = workbenches.size();
        if (num < n) {
            workbenches[num]->updateWorkbench(line);
        } else {
            robots[num - n]->updateRobot(line);
        }
    }

    vector<Robot *> getRobots() {
        return robots;
    }

    vector<Workbench *> getWorkbenches() {
        return workbenches;
    }
};


#endif //CODECRAFTSDK_WORK_HPP
