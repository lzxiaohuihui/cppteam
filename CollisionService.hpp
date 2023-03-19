#include <bits/stdc++.h>
#include "util.hpp"
#include <vector>

#include "Robot.hpp"
#include "Workbench.hpp"

using namespace std;

class CollisionService {

public:
    vector<string> avoid(vector<Robot*> robots) {
        vector<string> orders;
        int num = robots.size();

        for (int i = 0; i < num; ++i) {
            auto robot1 = robots[i];

            for (int j = 0; j < num; ++j) {
                if (i == j) continue;
                auto robot2 = robots[j];
                if (Util::isCollision(*robot1, *robot2)) {
                    vector<string> res = fineTuning(*robot1, *robot2);
                    orders.insert(orders.end(), res.begin(), res.end());
                }
            }
        }

        return orders;
    }


    vector<string> fineTuning(Robot robot1, Robot robot2) {
        vector<string> orders;

        if (robot1.getCarry() == 0 && robot2.getCarry() == 0) return orders;

        // 对撞
        if (Util::getRobotDp(robot1, robot2) < -0.5) {
            double cp1 = Util::getCp(robot1, robot2);
            double cp2 = Util::getCp(robot2, robot1);

            if (cp1 * cp2 > 0) {
                orders.push_back("rotate " + to_string(robot1.getRobotId()) + " " + to_string(3.0 * -cp1) + "\n");
            } else {
                orders.push_back("rotate " + to_string(robot1.getRobotId()) + " " + to_string(3.0 * abs(cp1)) + "\n");
            }
        }

        return orders;
    }
};