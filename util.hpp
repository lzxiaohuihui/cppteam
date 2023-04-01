//
// Created by lzh on 18/03/23.
//

#ifndef CODECRAFTSDK_UTIL_HPP
#define CODECRAFTSDK_UTIL_HPP

#include <iostream>
#include <sstream>
#include <vector>
#include "Robot.hpp"
#include "Workbench.hpp"

using namespace std;

class Util {
public:
    static vector<string> split(string s, char delimiter) {
        vector<std::string> tokens;
        stringstream ss(s);
        string token;
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }


    static int addWorkbenchStatus(int num, Workbench* workbench) {
        if (pow(2, num) + workbench->getRawMaterialStatus() == workbench->getRaw() && workbench->getRaw() == -1) {
            return 0;
        }
        return (int) (pow(2, num) + workbench->getRawMaterialStatus());
    }

    static vector<double> getPosition(Robot robot,double t) {
        // 初始位置 (x, y, theta)
        double x0 = robot.getX();
        double y0 = robot.getY();
        double cosTheta = cos(robot.getOrientation());
        double sinTheta = sin(robot.getOrientation());

        double v = robot.getV();
        double w = robot.getW();

        v = v < 0 ? -v : v;

        double x = x0 + v * cosTheta * t - 0.5 * w * v * sinTheta * t * t;
        double y = y0 + v * sinTheta * t + 0.5 * w * v * cosTheta * t * t;

        return {x, y};
    }

    static bool isCollisionWall(Robot robot1) {
        double eps = 1.1;
        for (double t = 0.0; t < 1.0; t += 0.04) {
            if (getPosition(robot1, t)[0] < 0.5 || getPosition(robot1, t)[0] > 49.5) {
                return true;
            }
            if (getPosition(robot1, t)[1] < 0.5 || getPosition(robot1, t)[1] > 49.5) {
                return true;
            }
        }
        return false;
    }

    static bool isCollision(Robot robot1, Robot robot2) {
        double eps = 1.1;
        for (double t = 0.0; t < 1.0; t += 0.04) {
            if (getDistance(getPosition(robot1, t), getPosition(robot2, t)) < eps) {
                return true;
            }
        }
        return false;
//        if (getDistance(robot1, robot2) > 4){
//            return false;
//        }
//
//        double cp = getCp(robot1, robot2);
//
//        return cp < eps;
    }

    static double getDistance(double x1, double y1, double x2, double y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    }

    static double getDistance(Robot robot1, Robot robot2) {
        return getDistance({robot1.getX(), robot1.getY()}, {robot2.getX(), robot2.getY()});
    }

    static double getDistance(Robot robot, Workbench workbench) {
        return getDistance({robot.getX(), robot.getY()}, {workbench.getX(), workbench.getY()});
    }

    static double getDistance(vector<double> p1, vector<double> p2) {
        return sqrt(pow(p1[0] - p2[0], 2) + pow(p1[1] - p2[1], 2));
    }

    static double getCp(Robot robot1, Robot robot2) {
        double dx = robot2.getX() - robot1.getX();
        double dy = robot2.getY() - robot1.getY();
        double cosTheta = cos(robot1.getOrientation());
        double sinTheta = sin(robot1.getOrientation());
        return dy * cosTheta - dx * sinTheta;
    }

    static double getRobotCp(Robot robot1, Robot robot2) {
        double cosAlpha = cos(robot1.getOrientation());
        double sinAlpha = sin(robot1.getOrientation());
        double cosTheta = cos(robot2.getOrientation());
        double sinTheta = sin(robot2.getOrientation());
        return cosAlpha * sinTheta - sinAlpha * cosTheta;
    }

    static double getCp(Robot robot, double x2, double y2) {
        double dx = x2 - robot.getX();
        double dy = y2 - robot.getY();
        double cosTheta = cos(robot.getOrientation());
        double sinTheta = sin(robot.getOrientation());
        return dy * cosTheta - dx * sinTheta;
    }

    static double getDp(Robot robot1, Robot robot2) {
        double dx = robot2.getX() - robot1.getX();
        double dy = robot2.getY() - robot1.getY();
        double cosTheta = cos(robot1.getOrientation());
        double sinTheta = sin(robot1.getOrientation());
        return dx * cosTheta + dy * sinTheta;
    }

    static double getRobotDp(Robot robot1, Robot robot2) {
        double cosAlpha = cos(robot1.getOrientation());
        double sinAlpha = sin(robot1.getOrientation());
        double cosTheta = cos(robot2.getOrientation());
        double sinTheta = sin(robot2.getOrientation());
        return cosAlpha * cosTheta + sinAlpha * sinTheta;
    }



    static double getDpCos(Robot robot1, Robot robot2) {
        return getDp(robot1, robot2) / getDistance(robot1, robot2);
    }

};


#endif //CODECRAFTSDK_UTIL_HPP
