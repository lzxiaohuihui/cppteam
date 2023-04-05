//
// Created by lzh on 05/04/23.
//

#ifndef CODECRAFTSDK_FINDBESTWORKBENCH_HPP
#define CODECRAFTSDK_FINDBESTWORKBENCH_HPP

#include "Robot.hpp"
#include "Workbench.hpp"
#include "FindWorkBenchService.hpp"
#include "aStar.hpp"

using namespace std;

class FindBestWorkbenchService : public FindWorkBenchService {
private:
    vector<Robot *> robots;
    vector<Workbench *> workbenches;
    map<pair<int, int>, vector<vector<double>>> originCarryPaths;
    map<pair<int, int>, vector<vector<double>>> optimCarryPaths;
    unordered_map<int, vector<int>> sellTargets;
    unordered_map<int, vector<int>> buyTargets;
    vector<vector<Workbench *>> typeWorkbenches;

public:
    aStar *pathPlanning;

    FindBestWorkbenchService(vector<Robot *> _robots, vector<Workbench *> _workbenches,
                             unordered_map<int, vector<int>> _sellTargets,
                             unordered_map<int, vector<int>> _buyTargets,
                             vector<vector<Workbench *>> _typeWorkbenches,
                             const map<pair<int, int>, vector<vector<double>>> &_originCarryPaths,
                             const map<pair<int, int>, vector<vector<double>>> &_optimCarryPaths,
                             aStar *_pathPlanning) :
            robots(std::move(_robots)),
            workbenches(std::move(_workbenches)),
            sellTargets(std::move(_sellTargets)),
            buyTargets(std::move(_buyTargets)),
            typeWorkbenches(std::move(_typeWorkbenches)),
            pathPlanning(_pathPlanning) {
        originCarryPaths = _originCarryPaths;
        optimCarryPaths = _optimCarryPaths;
    }
    void findWorkbenchSell(int robotId, int frameId) override{

    }

    void findWorkbenchBuy(int robotId, int frameId) override {

    }

};


#endif //CODECRAFTSDK_FINDBESTWORKBENCH_HPP
