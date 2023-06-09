//
// Created by lzh on 01/04/23.
//

#ifndef CODECRAFTSDK_ASTAR_HPP
#define CODECRAFTSDK_ASTAR_HPP

#include "bits/stdc++.h"

using namespace std;

class aStar {
private:
    struct Node {
        int x, y;
        int g, h;
        Node *parent;

        Node(int _x, int _y, int _g, int _h, Node *_parent) : x(_x), y(_y), g(_g), h(_h), parent(_parent) {}

        int f() const { return g + h; }
    };

    vector<vector<int>> maze;
    vector<vector<int>> originMaze;

public:

    aStar(vector<vector<int>> _maze) : originMaze(_maze) {
//        maze = vector(50, vector<int>(50, 0));
//        for (int i = 0; i < 100; i += 2) {
//            for (int j = 0; j < 100; j += 2) {
//                int sum = 0;
//                for (int x = i; x < i + 2; x++) {
//                    for (int y = j; y < j + 2; y++) {
//                        sum += _maze[x][y];
//                    }
//                }
//                maze[i / 2][j / 2] = sum>0?1:0;
//            }
//        }
        maze = originMaze;

    }

    bool find_path(double x1, double y1, double x2, double y2, vector<vector<double>> &optimPath,
                   vector<vector<double>> &originPath, bool isCarry) {
        int start_x = (int) ((x1 - 0.25) * 2);
        int start_y = (int) ((y1 - 0.25) * 2);
        int end_x = (int) ((x2 - 0.25) * 2);
        int end_y = (int) ((y2 - 0.25) * 2);
        vector<Node *> path;
        int rows = maze.size(), cols = maze[0].size();
        priority_queue<Node *, vector<Node *>, function<bool(Node *, Node *)>> open(
                [](Node *a, Node *b) { return a->f() > b->f(); });
        vector<vector<bool>> closed(rows, vector<bool>(cols, false));
        vector<vector<Node *>> nodes(rows, vector<Node *>(cols, nullptr));
        Node *start_node = new Node(start_x, start_y, 0, heuristic(start_x, start_y, end_x, end_y), nullptr);
        nodes[start_x][start_y] = start_node;
        open.push(start_node);
        while (!open.empty()) {
            Node *current = open.top();
            open.pop();
            if (current->x == end_x && current->y == end_y) {
                while (current) {
                    path.push_back(current);
                    current = current->parent;
                }
                break;
            }
            closed[current->x][current->y] = true;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (i + j != 1 && i + j != -1) continue;
                    if (i == 0 && j == 0) continue;
                    int x = current->x + i, y = current->y + j;
                    // 是否携带物品
                    if (isCarry) {
                        bool flag = false;
                        for (int m = -1; m <= 1; ++m) {
                            for (int n = -1; n <= 1; ++n) {
                                if (m + n != 1 && m + n != -1) continue;
                                if (x == 1 || x == 98) flag = true;
                                if (y == 1 || y == 98) flag = true;
                                if (maze[x + m][y + n] == 1) flag = true;
                            }
                        }
                        if (flag) {
                            continue;
                        }
                    }

                    if (x < 0 || x >= rows || y < 0 || y >= cols || maze[x][y] == 1 || closed[x][y]) continue;
                    Node *neighbor;
                    int g = current->g + (i * j == 0 ? 1 : 2);
                    if (!nodes[x][y]) {
                        neighbor = new Node(x, y, g, heuristic(x, y, end_x, end_y), current);
                        nodes[x][y] = neighbor;
                    } else if (g < nodes[x][y]->g) {
                        neighbor = nodes[x][y];
                        neighbor->g = g;
                        neighbor->parent = current;
                    } else continue;
                    open.push(neighbor);
                }
            }
        }

        if (path.empty()) return false;
        vector<vector<int>> offsets = {{1,  0},
                                       {0,  1},
                                       {-1, 0},
                                       {0,  -1},
                                       {1,  1},
                                       {1,  -1},
                                       {-1, 1},
                                       {-1, -1}};
        int n = path.size();
        int pre_x = path[n - 1]->x;
        int pre_y = path[n - 1]->y;
        optimPath.push_back({pre_x / 2.0 + 0.25, pre_y / 2.0 + 0.25});
        for (int i = path.size() - 1; i >= 0; --i) {
            int &cur_x = path[i]->x;
            int &cur_y = path[i]->y;

//            if (cur_x > 0 && cur_x < 99 && cur_y > 0 && cur_y < 99) {
//                for (const auto &item: offsets) {
//                    if ((maze[cur_x + item[0]][cur_y + item[1]] == 1) && maze[cur_x - item[0]][cur_y - item[1]] != 1) {
//                        cur_x -= item[0];
//                        cur_y -= item[1];
//                    }
//                }
//            }
            originPath.push_back({cur_x / 2.0 + 0.25, cur_y / 2.0 + 0.25});

            bool flag = hasObstacle(pre_x, pre_y, cur_x, cur_y);

            if (!flag) {
                continue;
            }
            pre_x = path[i]->x;
            pre_y = path[i]->y;

            if (i < n - 2) {
                int px = path[i + 2]->x, py = path[i + 2]->y;
                double target_x = px, target_y = py;
                if (px > 0 && px < 99 && py > 0 && py < 99) {
                    for (const auto &item: offsets) {
                        if ((maze[px + item[0]][py + item[1]] == 1) && maze[px - item[0]][py - item[1]] != 1) {
                            target_x -= item[0]/2.0;
                            target_y -= item[1]/2.0;
                        }
                    }
                }
                optimPath.push_back({target_x / 2.0 + 0.25, target_y / 2.0 + 0.25});
            }
//            else if (i < n - 1) optimPath.push_back({path[i + 1]->x / 2.0 + 0.25, path[i + 1]->y / 2.0 + 0.25});
//            else optimPath.push_back({path[i]->x / 2.0 + 0.25, path[i]->y / 2.0 + 0.25});
        }
        optimPath.push_back({path[0]->x / 2.0 + 0.25, path[0]->y / 2.0 + 0.25});

        return true;

    }

    int heuristic(int x1, int y1, int x2, int y2) {
        return abs(x1 - x2) + abs(y1 - y2);
    }

    bool hasObstacle(int x1, int y1, int x2, int y2) {
        int ROBOT_RADIUS = 0;
        double OBSTACLE_RADIUS = 2.0;
        // 判断AB线段是否经过障碍
        int dx = abs(x1 - x2);
        int dy = abs(y1 - y2);
        int x = x1, y = y1;
        int xstep = (x2 > x1) ? 1 : -1;
        int ystep = (y2 > y1) ? 1 : -1;
        int error = dx - dy;
        while (x != x2 || y != y2) {
            // 判断机器人是否在障碍物的半径范围内
            for (int i = x - ROBOT_RADIUS - 1; i <= x + ROBOT_RADIUS + 1; i++) {
                for (int j = y - ROBOT_RADIUS - 1; j <= y + ROBOT_RADIUS + 1; j++) {
                    if (i >= 0 && i < 100 && j >= 0 && j < 100) {
                        if (maze[i][j] == 1 &&
                            sqrt((i - x) * (i - x) + (j - y) * (j - y)) <= OBSTACLE_RADIUS + ROBOT_RADIUS) {
                            return true;
                        }
                    }
                }
            }
            int e2 = 2 * error;
            if (e2 > -2 * dy) { // 将误差范围放大一些
                error -= dy;
                x += xstep;
            }
            if (e2 < 2 * dx) { // 将误差范围放大一些
                error += dx;
                y += ystep;
            }
        }
        return false;
    }

};

#endif //CODECRAFTSDK_ASTAR_HPP
