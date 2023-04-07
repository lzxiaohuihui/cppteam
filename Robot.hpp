//
// Created by lzh on 18/03/23.
//

#ifndef ROBOT_HPP_
#define ROBOT_HPP_

#include "util.hpp"
#include <string>
#include <vector>
#include <math.h>

#define PI 3.1415925

using namespace std;

class Robot {

private:
    /**
     * 机器人id
     */
    int robotId;

    /**
     * 要前往的工作台id, -1代表没有目标
     */
    int targetWorkBenchId = -1;

    /**
     * 所处工作台ID
     * -1:表示当前没有处于任何工作台附近
     * [0,工作台总数-1] :表示某工作台的下
     * 标,从 0 开始,按输入顺序定。当前机
     * 器人的所有购买、出售行为均针对该工
     * 作台进行。
     */
    int workbenchId;

    /**
     * 携带物品类型
     * 范围[0,7]。
     * 0 表示未携带物品。
     * 1-7 表示对应物品。
     */
    int carry;

    /**
     * 时间价值系数
     * 携带物品时为[0.8, 1]的浮点数
     * 不携带物品时为 0
     */
    double timeValue;


    /**
     * 碰撞价值系数
     * 携带物品时为[0.8, 1]的浮点数
     * 不携带物品时为 0。
     */
    double collisionValue;
    /**
     * 角速度
     * 单位:弧度/秒。
     * 正数:表示逆时针。
     * 负数:表示顺时针
     */
    double w;

    /**
     * 线速度 x
     */
    double vx;
    /**
     * 线速度 y
     */

    double vy;
    /**
     * 朝向
     * 弧度,范围[-π,π]。方向示例:
     * 0:表示右方向。
     * π/2:表示上方向。
     * -π/2:表示下方向。
     */

    double orientation;
    /**
     * 坐标 x
     */

    double x;
    /**
     * 坐标 y
     */
    double y;

    vector<vector<double>> path;

    int path_step;

    double linearKp = 6.0;
    double linearKi = 0.00;
    double linearKd = 0.02;
    double angularKp = 10.0;
    double angularKi = 0.00;
    double angularKd = 0.15;
    double linearIntegral = 0.0, linearLastError;
    double angularIntegral = 0.0, angularLastError;

public:

    void pidClear() {
        linearIntegral = 0.0;
        linearLastError = 0.0;
        angularIntegral = 0.0;
        angularLastError = 0.0;
        path_step = 0;
    }

    void updateRobot(std::string line) {
        vector<string> parts = split(line, ' ');
        setWorkbenchId(stoi(parts[0]));
        setCarry(stoi(parts[1]));
        setTimeValue(stod((parts[2])));
        setCollisionValue(stod(parts[3]));
        setW(stod(parts[4]));
        setVx(stod(parts[5]));
        setVy(stod(parts[6]));
        setOrientation(stod(parts[7]));
        setX(stod(parts[8]));
        setY(stod(parts[9]));
    }

    vector<string> split(string s, char delimiter) {
        vector<std::string> tokens;
        stringstream ss(s);
        string token;
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }


    Robot(int id, double x, double y) : robotId(id), x(x), y(y) {

    }


    std::vector<std::string> pidMove(double targetX, double targetY) {
        double distance = sqrt(pow(targetX - x, 2) + pow(targetY - y, 2));
        if (distance < 0.4) return {};
        double cosTheta = cos(orientation);
        double sinTheta = sin(orientation);
        double dp = (targetX - x) * cosTheta + (targetY - y) * sinTheta;
        // cp > 0 目标点在当前的逆时针方向
        double cp = (targetY - y) * cosTheta - (targetX - x) * sinTheta;
        cp = cp == 0 ? 0 : cp / abs(cp);
        double angle = acos(dp / sqrt(pow(targetX - x, 2) + pow(targetY - y, 2) + 0.001));

        double linearError = distance * cos(angle);
        linearIntegral += linearError;
        angularIntegral += angle;
        double linearDerivative = linearError - linearLastError;
        double angularDerivative = angle - angularLastError;
        linearLastError = linearError;
        angularLastError = angle;
        double linearVelocity = linearKp * linearError + linearKi * linearIntegral + linearKd * linearDerivative;
        double angularVelocity = angularKp * angle + angularKi * angularIntegral + angularKd * angularDerivative;
        // limit linear velocity to [-2, 6]
//        linearVelocity = max(2.0, min(6.0, linearVelocity));
        // limit angular velocity to [-π, π]
//        angularVelocity = max(-PI, min(PI, angularVelocity));

        vector<string> res;
        res.push_back("forward " + to_string(robotId) + " " + to_string(linearVelocity) + "\n");
        res.push_back("rotate " + to_string(robotId) + " " + to_string(cp * angularVelocity) + "\n");
        return res;
    }

    vector<string> pathMove() {
        vector<string> res;
//        int n = path.size();
//        if (path_step == n-1){
//            return pidMove(path[n-1][0], path[n-1][1]);
//        }
        for (; path_step < path.size(); ++path_step) {
            double targetX = path[path_step][0];
            double targetY = path[path_step][1];
//            fprintf(stderr, "(x:%lf, y%lf)->(target_x: %lf, target_y: %lf)\n", x, y, targetX, targetY);
//            if (robotId == 0) fprintf(stderr, "%d robot step: %d\n", robotId, path_step);
            double distance = sqrt(pow(targetX - x, 2) + pow(targetY - y, 2));
            if (distance <= 0.45 && path_step < path.size()-1) continue;
            if (distance < 0.4 && path_step == path.size()-1) return {};
            double cosTheta = cos(orientation);
            double sinTheta = sin(orientation);
            double dp = (targetX - x) * cosTheta + (targetY - y) * sinTheta;
            // cp > 0 目标点在当前的逆时针方向
            double cp = (targetY - y) * cosTheta - (targetX - x) * sinTheta;
            cp = cp == 0 ? 0 : cp / abs(cp);
            double angle = acos(dp / sqrt(pow(targetX - x, 2) + pow(targetY - y, 2) + 0.001));

            double linearError = distance * tan(PI/2 - angle);
//            double linearError = distance * cos(angle) / 2;
            linearIntegral += linearError;
            angularIntegral += angle;
            double linearDerivative = linearError - linearLastError;
            double angularDerivative = angle - angularLastError;
            linearLastError = linearError;
            angularLastError = angle;
            double linearVelocity = linearKp * linearError + linearKi * linearIntegral + linearKd * linearDerivative;
            double angularVelocity = angularKp * angle + angularKi * angularIntegral + angularKd * angularDerivative;
            // limit linear velocity to [-2, 6]
            linearVelocity = max(-2.0, min(6.0, linearVelocity));

            // limit angular velocity to [-π, π]
            angularVelocity = max(-PI, min(PI, angularVelocity));
//            if (path_step < path.size()-2) linearVelocity = max(4.0, linearVelocity);
            if (abs(angularVelocity) > PI/4) linearVelocity /= 2.0;
//            if (path_step < n - 2 && distance < 1.0) linearVelocity /= 2.0;
            res.push_back("forward " + to_string(robotId) + " " + to_string(linearVelocity) + "\n");
            res.push_back("rotate " + to_string(robotId) + " " + to_string(cp * angularVelocity) + "\n");
            return res;
        }
        return res;

    }

    int getWorkbenchId() const {
        return workbenchId;
    }

    void setWorkbenchId(int a) {
        workbenchId = a;
    }

    int getCarry() {
        return carry;
    }


    void setCarry(int c) {
        carry = c;
    }


    double getTimeValue() {
        return timeValue;
    }


    void setTimeValue(double t) {
        timeValue = t;
    }


    double getCollisionValue() {
        return collisionValue;
    }


    void setCollisionValue(double c) {
        collisionValue = c;
    }


    double getW() {
        return w;
    }


    void setW(double w_) {
        w = w_;
    }


    double getVx() {
        return vx;
    }


    void setVx(double vx_) {
        vx = vx_;
    }


    double getVy() {
        return vy;
    }


    void setVy(double vy_) {
        vy = vy_;
    }


    double getOrientation() {
        return orientation;
    }


    void setOrientation(double o) {
        orientation = o;
    }


    void setX(double x_) {
        x = x_;
    }


    void setY(double y_) {
        y = y_;
    }


    int getTargetWorkBenchId() {
        return targetWorkBenchId;
    }


    void setTargetWorkBenchId(int t) {
        targetWorkBenchId = t;
    }


    bool hasTarget() {
        return targetWorkBenchId >= 0;
    }


    int getRobotId() {
        return robotId;
    }


    void setRobotId(int x_) {
        robotId = x_;
    }


    double getV() {
        double bias = 1;
        if (getVx() * getVy() < 0) bias = -1;
        return bias * sqrt(pow(getVx(), 2) + pow(getVy(), 2));
    }


    double getX() {
        return x;
    }


    double getY() {
        return y;
    }

    void setPath(const vector<vector<double>> &_path) {
        path = _path;
        path_step = 0;
    }
    bool hasPath(){
        return !path.empty();
    }

    vector<double> getCurTarget(){
        return {path[path_step][0], path[path_step][1]};
    }
};


#endif
