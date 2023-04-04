#include <climits>
#include <functional>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

// 定义地图大小和障碍物
const int ROW = 10;
const int COL = 10;
const vector<pair<int, int>> OBSTACLES = {{2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2}, {2, 7}, {3, 7}, {4, 7}, {5, 7}, {6, 7}, {7, 7}, {8, 7}};

// 定义单个代理人的状态
struct State {
    int x, y;  // 代理人的位置
    int g, h;  // g为从起点到当前状态的代价，h为从当前状态到终点的估计代价
    int f() const { return g + h; }  // f为总代价
    bool operator==(const State& other) const { return x == other.x && y == other.y; }
};

// 定义哈希函数用于unordered_map
namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State& s) const {
            return hash<int>()(s.x) ^ hash<int>()(s.y);
        }
    };
}

// 定义A*算法
vector<State> astar(const State& start, const State& goal) {
    // 定义open和closed列表
    priority_queue<State, vector<State>, function<bool(const State&, const State&)>> open([](const State& a, const State& b) { return a.f() > b.f(); });
    unordered_map<State, State> came_from;
    unordered_map<State, int> cost_so_far;

    // 将起点加入open列表
    open.push(start);
    came_from[start] = start;
    cost_so_far[start] = 0;

    while (!open.empty()) {
        // 取出f值最小的状态
        State current = open.top();
        open.pop();

        // 如果当前状态为终点，则返回路径
        if (current == goal) {
            vector<State> path;
            while (current != start) {
                path.push_back(current);
                current = came_from[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return path;
        }

        // 遍历当前状态的邻居
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                State next = {current.x + dx, current.y + dy, 0, 0};
                if (next.x < 0 || next.x >= ROW || next.y < 0 || next.y >= COL) continue;
                if (find(OBSTACLES.begin(), OBSTACLES.end(), make_pair(next.x, next.y)) != OBSTACLES.end()) continue;
                int new_cost = cost_so_far[current] + 1;
                if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next]) {
                    cost_so_far[next] = new_cost;
                    next.g = new_cost;
                    next.h = abs(next.x - goal.x) + abs(next.y - goal.y);
                    open.push(next);
                    came_from[next] = current;
                }
            }
        }
    }

    // 如果无法到达终点，则返回空路径
    return vector<State>();
}

// 定义CBS算法
vector<vector<State>> cbs(const vector<State>& starts, const vector<State>& goals) {
    // 定义open和closed列表
    priority_queue<vector<State>, vector<vector<State>>, function<bool(const vector<State>&, const vector<State>&)>> open([](const vector<State>& a, const vector<State>& b) { return a.size() > b.size(); });
    unordered_map<vector<State>, int> cost_so_far;

    // 将初始状态加入open列表
    open.push(starts);
    cost_so_far[starts] = 0;

    while (!open.empty()) {
        // 取出f值最小的状态
        vector<State> current = open.top();
        open.pop();

        // 检查当前状态是否为目标状态
        bool goal_reached = true;
        for (int i = 0; i < current.size(); i++) {
            if (current[i] != goals[i]) {
                goal_reached = false;
                break;
            }
        }
        if (goal_reached) {
            return current;
        }

        // 遍历所有代理人的冲突
        vector<pair<int, int>> conflicts;
        for (int i = 0; i < current.size(); i++) {
            for (int j = i + 1; j < current.size(); j++) {
                if (current[i] == current[j]) {
                    conflicts.push_back({i, j});
                }
            }
        }

        // 如果没有冲突，则遍历所有代理人的邻居
        if (conflicts.empty()) {
            for (int i = 0; i < current.size(); i++) {
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        State next = {current[i].x + dx, current[i].y + dy, 0, 0};
                        if (next.x < 0 || next.x >= ROW || next.y < 0 || next.y >= COL) continue;
                        if (find(OBSTACLES.begin(), OBSTACLES.end(), make_pair(next.x, next.y)) != OBSTACLES.end()) continue;
                        vector<State> next_state = current;
                        next_state[i] = next;
                        int new_cost = cost_so_far[current] + 1;
                        if (cost_so_far.find(next_state) == cost_so_far.end() || new_cost < cost_so_far[next_state]) {
                            cost_so_far[next_state] = new_cost;
                            open.push(next_state);
                        }
                    }
                }
            }
        }
        // 如果有冲突，则分裂状态
        else {
            // 找到最紧急的冲突
            int min_time = INT_MAX;
            pair<int, int> min_conflict;
            for (const auto& conflict : conflicts) {
                int time = max(cost_so_far[current], max(current[conflict.first].g, current[conflict.second].g));
                if (time < min_time) {
                    min_time = time;
                    min_conflict = conflict;
                }
            }

            // 分裂状态
            for (int i = 0; i < 2; i++) {
                vector<State> next_state = current;
                int agent = i == 0 ? min_conflict.first : min_conflict.second;
                State start = current[agent];
                State goal = i == 0 ? current[min_conflict.second] : current[min_conflict.first];
                vector<State> path = astar(start, goal);
                if (path.empty()) continue;
                int new_cost = cost_so_far[current] + path.size() - 1;
                next_state[agent] = path.back();
                if (cost_so_far.find(next_state) == cost_so_far.end() || new_cost < cost_so_far[next_state]) {
                    cost_so_far[next_state] = new_cost;
                    open.push(next_state);
                }
            }
        }
    }

    // 如果无法找到路径，则返回空路径
    return vector<vector<State>>();
}

int main() {
    // 定义起点和终点
    vector<State> starts = {{0, 0, 0, 0}, {9, 9, 0, 0}};
    vector<State> goals = {{9, 9, 0, 0}, {0, 0, 0, 0}};

    // 运行CBS算法
    vector<vector<State>> paths = cbs(starts, goals);

    // 输出路径
    for (int i = 0; i < paths[0].size(); i++) {
        for (int j = 0; j < paths.size(); j++) {
            if (i < paths[j].size()) {
                cout << "(" << paths[j][i].x << "," << paths[j][i].y << ") ";
            }
            else {
                cout << "      ";
            }
        }
        cout << endl;
    }

    return 0;
}