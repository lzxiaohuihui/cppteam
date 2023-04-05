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
#include "FindBestWorkbenchService.hpp"


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
    map<pair<int, int>, vector<vector<double>>> originCarryPaths;
    map<pair<int, int>, vector<vector<double>>> optimCarryPaths;
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
        auto start_time = chrono::high_resolution_clock::now();
        // workbench --> w
        for (const auto &workbench: workbenches) {
            for (const auto &w: workbenches) {
                if (workbench==w) continue;
                vector<vector<double>> res1;
                vector<vector<double>> res2;
                bool succeed1 = pathPlanning->find_path(workbench->getX(), workbench->getY(), w->getX(), w->getY(),
                                                       res1, res2, true);
                if (succeed1) {
                    pair<int,int> p(workbench->getWorkbenchId(), w->getWorkbenchId());
                    optimCarryPaths.insert(make_pair(p, res1));
                    originCarryPaths.insert(make_pair(p, res2));
                }
            }
        }
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        fprintf(stderr, "[OK][%ld milliseconds]\n", duration.count());

        fprintf(stderr, "init workbench can buy", duration.count());
        start_time = chrono::high_resolution_clock::now();
        // 标记买了这个工作台，能不能卖出去
        for (int i = 1; i < 4; ++i) {
//            workbench -> w
            for (auto &workbench: typeWorkbenches[i]){
                bool b = false;
                for (const auto &j: sellTargets[i]){
                    for (const auto &w: typeWorkbenches[j]){
                        pair<int,int> p(workbench->getWorkbenchId(), w->getWorkbenchId());
                        if (!originCarryPaths[p].empty()){
                            workbench->setCanBuy(true);
                            b = true;
                            break;
                        }
                    }
                    if (b) break;
                }
                if (!b) fprintf(stderr, "工作台%d不能购买. ", workbench->getWorkbenchId());
            }
        }

        end_time = chrono::high_resolution_clock::now();
        duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
        fprintf(stderr, "[OK][%ld milliseconds]\n", duration.count());

        fprintf(stderr, "init collisionService.");
        collisionService = new CollisionWallService(maze);
        fprintf(stderr, "[OK]\n");

        fprintf(stderr, "init findWorkBenchService.");
        findWorkBenchService = new FindNearestWorkBenchService(robots, workbenches, sellTargets, buyTargets,
                                                               typeWorkbenches, originCarryPaths,optimCarryPaths, pathPlanning);
//        findWorkBenchService = new FindBestWorkbenchService(robots, workbenches, sellTargets, buyTargets,
//                                                               typeWorkbenches, originCarryPaths,optimCarryPaths, pathPlanning);
        fprintf(stderr, "[OK]\n");

    }

    vector<string> schedulingRobot(int frameId) {
        vector<string> orders;

        for (const auto &robot: robots) {
            if (robot->getRobotId() == 3) continue;
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

//            bool succeed = pathPlanning->find_path(start_x, start_y, end_x, end_y, path,true);
//            if (!succeed) return orders;

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
                workbenches.push_back(new Workbench(countWorkbench++, c - '0', i / 2.0 + 0.25, (99 - num) / 2.0 + 0.25));
            }

            if (c == 'A') {
                robots.push_back(new Robot(countRobot++, i / 2.0 + 0.25, (99 - num) / 2.0 + 0.25));
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
