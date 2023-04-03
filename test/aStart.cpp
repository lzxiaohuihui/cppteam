#include "bits/stdc++.h"

using namespace std;

struct Node {
    int x, y;
    Node *parent;
    int g, h;

    Node(int _x, int _y, int _g, int _h, Node *_parent) : x(_x), y(_y), g(_g), h(_h), parent(_parent) {}

    int f() const { return g + h; }
};

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

vector<Node *> find_path(vector<vector<int>> &maze, double x1, double y1, double x2, double y2) {
    int start_x = (int) ((x1 - 0.25)*2);
    int start_y = (int) ((y1 - 0.25)*2);
    int end_x = (int) ((x2 - 0.25)*2);
    int end_y = (int) ((y2 - 0.25)*2);
    vector<Node *> path;
    int rows = maze.size(), cols = maze[0].size();
    priority_queue<Node *, vector<Node *>, function<bool(Node *, Node *)>> open(
            [](Node *a, Node *b) { return a->f() > b->f(); });
    vector<vector<bool>> closed(rows, vector<bool>(cols, false));
    vector<vector<Node *>> nodes(rows, vector<Node *>(cols, nullptr));
    Node *start_node = new Node(start_x, start_y, 0, heuristic(start_x, start_y, end_x, end_y), nullptr);
    nodes[start_x][start_y] = start_node;
    open.push(start_node);
    auto start_time = chrono::high_resolution_clock::now();
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
    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
    cout << "Time taken: " << duration.count() << " microseconds" << endl;
    return path;
}

int main() {

    vector<vector<int>> maze(100, vector<int>(100, 0));
    vector<vector<char>> grid(100, vector<char>(100, 32));

    string line;
    ifstream infile("/home/lzh/Downloads/huawei2023/LinuxRelease2/maps/4.txt"); // 打开文件

    int row = 99;
    while (getline(infile, line)) { // 逐行读取
        int col = 0;
        char c;
        istringstream ss(line); // 将一行数据转换为stringstream
        while (ss >> c) { // 逐个读取数据
            if (c == '#') {
                grid[col][row] = '#';
                maze[col][row] = 1;
            } else {
                grid[col][row] = ' ';
                maze[col][row] = 0;
            }
            col += 1;
        }
        row -= 1;
    }

    infile.close(); // 关闭文件

    vector<vector<int>> maze2(50, vector<int>(50, 0));

    for (int i = 0; i < 100; i += 2) {
        for (int j = 0; j < 100; j += 2) {
            int sum = 0;
            for (int x = i; x < i + 2; x++) {
                for (int y = j; y < j + 2; y++) {
                    sum += maze[x][y];
                }
            }
            maze2[i / 2][j / 2] = sum > 0 ? 1 : 0;
        }
    }


    double start_x = 24.25, start_y = 48.75, end_x = 43.25, end_y = 9.75;
//    double start_x = 28.25, start_y = 25.75, end_x = 10.25, end_y = 24.75;
    vector<Node *> path = find_path(maze, start_x, start_y, end_x, end_y);
    if (path.empty()) {
        cout << "No path found" << endl;
        return 0;
    }

    grid[start_x * 2][start_y * 2] = 'S';

    vector<vector<int>> offsets = {{1,0}, {0,1}, {-1,0}, {0,-1}};
    set<Node*> pathSet(path.begin(), path.end());
    for (int i = path.size() - 1; i >= 0; --i) {
        int cur_x = path[i]->x;
        int cur_y = path[i]->y;

        for (const auto &item: offsets){
            if (maze[cur_x+item[0]][cur_y+item[1]] == 1 && maze[cur_x-item[0]][cur_y-item[1]] != 1){
                cur_x -= item[0];
                cur_y -= item[1];
            }
        }
        int pre_x = -1;
        int pre_y = -1;
        int next_x = -1;
        int next_y = -1;
        if(i < path.size()-1) {
            pre_x=path[i + 1]->x;
            pre_y=path[i + 1]->y;
        }
        if(i > 1) {
            next_x=path[i - 1]->x;
            next_y=path[i - 1]->y;
        }
        int sum = 0;
        sum += abs(pre_x - cur_x) + abs(pre_y - cur_y);
        sum += abs(next_x - cur_x) + abs(next_y - cur_y);
        if (sum <= 2) {
            cout << sum << endl;
            continue;
        }

        cout << "(" << cur_x/2.0 + 0.25 << ", " << cur_y/2.0 + 0.25 << ")";
        if (i > 0) cout << " -> ";
        grid[cur_x][cur_y] = '*';
    }
    cout << endl;
    cout << path.size() << endl;

    ofstream outfile("data.txt");
    for (int i = grid.size() - 1; i >= 0; i--) {
        for (int j = 0; j < grid[i].size(); j++) {
            outfile << grid[j][i];  // 输出元素
        }
        outfile << endl;
    }
    outfile.close();


    return 0;
}