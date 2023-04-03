//
// Created by lzh on 02/04/23.
//
#include "bits/stdc++.h"

using namespace std;

int main(){

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

    ofstream outfile("map4.txt");
    for (int i = maze2.size() - 1; i >= 0; i--) {
        for (int j = 0; j < maze2[i].size(); j++) {
            if (maze2[j][i] == 1) outfile << '#';  // 输出元素
            else outfile << ' ';
        }
        outfile << endl;
    }
    outfile.close();

}