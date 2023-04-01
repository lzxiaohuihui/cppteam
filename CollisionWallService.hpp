//
// Created by lzh on 01/04/23.
//

#ifndef CODECRAFTSDK_COLLISIONWALLSERVICE_HPP
#define CODECRAFTSDK_COLLISIONWALLSERVICE_HPP
#include <bits/stdc++.h>
#include "util.hpp"

#include "Robot.hpp"
#include "Workbench.hpp"

class CollisionWallService {
private:
    vector<vector<int>> data;

public:


    CollisionWallService(vector<vector<int>> _data): data(_data){}

    vector<string> avoid(vector<Robot*>& robots, vector<Workbench*>& workbenches) {
        vector<string> orders;
        int num = robots.size();

        for (int i = 0; i < num; ++i) {
            auto robot1 = robots[i];
            if (Util::isCollisionWall(*robot1, data)){
                vector<double> pos = robot1->getCurTarget();
                double cp = Util::getCp(*robot1, pos[0], pos[1]);

//                orders.push_back("forward " + to_string(robot1->getRobotId()) + " " + to_string(robot1->getV()/2) + "\n");
//                orders.push_back("rotate " + to_string(robot1->getRobotId()) + " " + to_string(1.0 * cp) + "\n");

            }
        }

        return orders;
    }


    vector<string> fineTuning(Robot& robot1, Robot& robot2, Workbench& workbench) {
        vector<string> orders;



        return orders;
    }
};

#endif //CODECRAFTSDK_COLLISIONWALLSERVICE_HPP
