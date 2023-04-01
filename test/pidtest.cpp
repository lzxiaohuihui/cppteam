#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// 机器人属性
struct Robot {
    double x; // x 坐标
    double y; // y 坐标
    double theta; // 朝向，弧度制
    double v; // 速度
    double w; // 角速度
};

// PID 控制器
class PIDController {
public:
    PIDController(double kp, double ki, double kd) {
        _kp = kp;
        _ki = ki;
        _kd = kd;
        _lastError = 0;
        _integral = 0;
    }

    double update(double error) {
        double derivative = error - _lastError;
        _integral += error;
        double output = _kp * error + _ki * _integral + _kd * derivative;
        _lastError = error;
        return output;
    }

private:
    double _kp;
    double _ki;
    double _kd;
    double _lastError;
    double _integral;
};

// A* 算法生成的路径
vector<pair<double, double>> path = {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};

int main() {
    Robot robot = {0, 0, 0, 1, 0}; // 初始位置和朝向，速度为 1，角速度为 0
    PIDController vController(1, 0.1, 0.01); // 速度 PID 控制器
    PIDController wController(2, 0.2, 0.02); // 角速度 PID 控制器

    for (int i = 0; i < path.size(); i++) {
        double targetX = path[i].first;
        double targetY = path[i].second;
        double targetTheta = atan2(targetY - robot.y, targetX - robot.x); // 计算目标朝向

        while (abs(robot.theta - targetTheta) > 0.1) { // 当机器人的朝向与目标朝向差距大于 0.1 弧度时
            double error = targetTheta - robot.theta;
            double wOutput = wController.update(error); // 计算角速度控制量
            robot.w = wOutput; // 更新机器人角速度
            robot.theta += robot.w; // 更新机器人朝向
            cout << "Robot position: (" << robot.x << ", " << robot.y << "), orientation: " << robot.theta << endl; // 输出机器人位置和朝向
        }

        while (sqrt(pow(robot.x - targetX, 2) + pow(robot.y - targetY, 2)) > 0.1) { // 当机器人与目标点的距离大于 0.1 时
            double error = sqrt(pow(robot.x - targetX, 2) + pow(robot.y - targetY, 2));
            double vOutput = vController.update(error); // 计算速度控制量
            robot.v = vOutput; // 更新机器人速度
            robot.x += robot.v * cos(robot.theta); // 更新机器人 x 坐标
            robot.y += robot.v * sin(robot.theta); // 更新机器人 y 坐标
            cout << "Robot position: (" << robot.x << ", " << robot.y << "), orientation: " << robot.theta << endl; // 输出机器人位置和朝向
        }
    }

    cout << "Robot reached destination!" << endl;

    return 0;
}