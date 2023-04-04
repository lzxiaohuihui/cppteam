#include <bits/stdc++.h>

#include <utility>
#include "util.hpp"

#include "Robot.hpp"
#include "Workbench.hpp"
#include "FindWorkBenchService.hpp"
#include "aStar.hpp"

using namespace std;

class FindNearestWorkBenchService : public FindWorkBenchService {
private:
    vector<Robot *> robots;
    vector<Workbench *> workbenches;
    map<pair<int, int>, vector<vector<double>>> paths;
    unordered_map<int, vector<int>> sellTargets;
    unordered_map<int, vector<int>> buyTargets;
    vector<vector<Workbench *>> typeWorkbenches;

public:
    aStar *pathPlanning;

    FindNearestWorkBenchService(vector<Robot *> _robots, vector<Workbench *> _workbenches,
                                unordered_map<int, vector<int>> _sellTargets,
                                unordered_map<int, vector<int>> _buyTargets,
                                vector<vector<Workbench *>> _typeWorkbenches,
                                const map<pair<int, int>, vector<vector<double>>> &_paths, aStar *_pathPlanning) :
            robots(std::move(_robots)),
            workbenches(std::move(_workbenches)),
            sellTargets(std::move(_sellTargets)),
            buyTargets(std::move(_buyTargets)),
            typeWorkbenches(std::move(_typeWorkbenches)),
            pathPlanning(_pathPlanning) {
        paths = _paths;
    }

    void findWorkbenchSell(int robotId, int frameId) override {
        auto start_time = chrono::high_resolution_clock::now();
        fprintf(stderr, "%d robot find workbench to sell. ", robotId);

        auto robot = robots[robotId];

        // 找距离当前最近的工作台
        vector<Workbench *> workbenchList;
        for (const auto &workbench: workbenches) {
            bool query = workbench->getType() > 3
                         && !workbench->getIsSellLock(robot->getCarry())
                         && workbench->needRawMaterial(robot->getCarry())
                         && !hasMaterialNum(robot->getCarry(), workbench->getRawMaterialStatus());
            if (!query) continue;
            workbenchList.push_back(workbench);
        }
//        std::sort(workbenchList.begin(), workbenchList.end(), cmpSell(robot, pathPlanning, paths));


        if (workbenchList.empty()) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            fprintf(stderr, "\tfailed.[%ld microseconds]\n", duration.count());
            return;
        }

        vector<pair<Workbench *, int>> ws;
        for (const auto &item: workbenchList) {
            int d = 0;
            vector<vector<double>> path;
            bool succeed = findPath(*robot, *item, path);
            if (succeed) {
                d = path.size();
                ws.push_back(make_pair(item, d));
            }
        }

        sort(ws.begin(), ws.end(), [](const pair<Workbench *, int> o1, const pair<Workbench *, int> o2) {
            return o1.second < o2.second;
        });


        for (auto &item: ws) {
            auto &workbench = item.first;
            vector<vector<double>> path;
            bool succeed = findPath(*robot, *workbench, path);
            if (succeed) {
                int workbenchId = workbench->getWorkbenchId();
                robot->setPath(path);
                robot->setTargetWorkBenchId(workbenchId);
                robot->pidClear();
                if (workbench->getType() != 9) workbench->setSellLock(true, robot->getCarry());
                auto end_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
                fprintf(stderr, "\tsucceed.[%ld microseconds]，workbench id is %d, type is %d, path size is %d\n",
                        duration.count(),
                        workbench->getWorkbenchId(), workbench->getType(), path.size());
                break;
            }
        }
        if (!robot->hasTarget()) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            fprintf(stderr, "\tfailed.[%ld microseconds]\n", duration.count());
        }
    }

    void findWorkbenchBuy(int robotId, int frameId) override {
        auto start_time = chrono::high_resolution_clock::now();
        fprintf(stderr, "%d robot find workbench to buy. ", robotId);
        auto robot = robots[robotId];
        // 找距离当前最近的工作台
        vector<Workbench *> workbenchList;
        for (const auto &workbench: workbenches) {
            bool query = !workbench->getIsBuyLock() && (workbench->getStatus() == 1 || workbench->getType() <= 3);
            if (!query) continue;
            workbenchList.push_back(workbench);
        }
//        std::sort(workbenchList.begin(), workbenchList.end(), cmpBuy(robot, pathPlanning, paths));

        if (workbenchList.empty()) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            fprintf(stderr, "\tfailed.[%ld microseconds]\n", duration.count());
            return;
        }

        vector<pair<Workbench *, int>> ws;
        for (const auto &item: workbenchList) {
            int d = 0;
            vector<vector<double>> path;
            bool succeed = findPath(*robot, *item, path);
            if (succeed) {
                d = path.size();
                ws.push_back(make_pair(item, d));
            }
        }

        sort(ws.begin(), ws.end(), [](const pair<Workbench *, int> o1, const pair<Workbench *, int> o2) {
            return o1.second < o2.second;
        });

        for (auto &item: ws) {
            auto &workbench = item.first;

            int count = 0;
            for (const auto &w: workbenches) {
                bool query1 = w->getType() > 3
                              && w->needRawMaterial(workbench->getType())
                              && !hasMaterialNum(workbench->getType(), w->getRawMaterialStatus())
                              && !w->getIsSellLock(workbench->getType());
                if (query1) count += 1;

            }

            int robotCarryNums = 0;
            for (const auto &r: robots) {
                bool query2 = r->hasTarget()
                              && r->getCarry() == 0
                              && workbenches[r->getTargetWorkBenchId()]->getType() == workbench->getType();
                if (query2) robotCarryNums += 1;
            }

            if (count - robotCarryNums < 1) continue;

            int workbenchId = workbench->getWorkbenchId();

            vector<vector<double>> path;
            bool succeed = findPath(*robot, *workbench, path);
            if (succeed) {
                robot->setPath(path);
                robot->setTargetWorkBenchId(workbenchId);
                robot->pidClear();
                if (workbench->getType() > 3) workbench->setBuyLock(true);
                auto end_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
                fprintf(stderr, "\tsucceed.[%ld microseconds]，workbench id is %d, type is %d, path size is %d\n", duration.count(),
                        workbench->getWorkbenchId(), workbench->getType(), path.size());
                break;
            }
        }
        if (!robot->hasTarget()) {
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            fprintf(stderr, "\tfailed.[%ld microseconds]\n", duration.count());
        }

    }

    bool findPath(Robot &robot, Workbench &workbench, vector<vector<double>> &path) {
        bool succeed = true;
        if (robot.getWorkbenchId() == -1) {
            succeed = pathPlanning->find_path(robot.getX(), robot.getY(), workbench.getX(), workbench.getY(), path,
                                              true);
//            fprintf(stderr, "\tget path from pathPlanning.");
        } else {
            path = paths[make_pair(robot.getWorkbenchId(), workbench.getWorkbenchId())];
//            fprintf(stderr, "\tget path from cache.");
        }
        return succeed && !path.empty();
    }

};