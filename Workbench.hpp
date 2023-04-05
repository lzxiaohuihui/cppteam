//
// Created by lzh on 18/03/23.
//
#ifndef WORKBENCH_HPP_
#define WORKBENCH_HPP_

#include <bits/stdc++.h>
#include "util.hpp"

using namespace std;

class Workbench {
    
private:
    /**
     * 工作台id
     */
    int workbenchId;

    /**
     * 有机器人正在来这里买货
     */
    bool isBuyLock;

    /**
     * 有机器人正在来这里卖货
     */
    bool isSellLock[8];

    /**
     * 工作台类型
     * 范围[1, 9]
     */
    int type;

    /**
     * 坐标 x
     */
    double x;

    /**
     * 坐标 y
     */
    double y;

    /**
     * 剩余生产时间(帧数)
     * -1:表示没有生产。
     * 0:表示生产因输出格满而阻塞。
     * >=0:表示剩余生产帧数。
     */
    int rpt;

    /**
     * 原材料格状态
     * 二进制位表描述,例如 48(110000)
     * 表示拥有物品 4 和 5。
     */
    int rawMaterialStatus;

    /**
     * 可以收购的原材料
     * 二进制位表描述,例如 48(110000)
     * 表示需要物品 4 和 5。
     */
    int raw;

    /**
     * 产品格状态
     * 0:表示无。
     * 1:表示有。
     */
    int status;

    /**
     * 当前需要这个类型物品的个数
     */
    int needCount;

    bool canBuy = false;

public:
    
    void updateWorkbench(const string& line) {
        vector<string> parts = split(line, ' ');
        setType(stoi(parts[0]));
        setX(stod(parts[1]));
        setY(stod(parts[2]));
        setRpt(stoi(parts[3]));
        setRawMaterialStatus(stoi(parts[4]));
        setStatus(stoi(parts[5]));
    }

    static vector<string> split(string s, char delimiter) {
        vector<std::string> tokens;
        istringstream iss(s);
        string token;
        while (getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    Workbench(int id, int type, double x, double y):workbenchId(id),type(type), x(x),y(y) {
        int r = 0;
        if (type == 4) r = 2 | 4;
        if (type == 5) r = 2 | 8;
        if (type == 6) r = 4 | 8;
        if (type == 7) r = 16 | 32 | 64;
        if (type == 8) r = 128;
        if (type == 9) r = 2 | 4 | 8 | 16 | 32 | 64 | 128;
        this->raw = r;
    }

    int getType() const {
        return type;
    }

    void setType(int t) {
        this->type = t;
    }

    double getX() const {
        return x;
    }

    void setX(double a) {
        this->x = a;
    }

    double getY() const {
        return y;
    }

    void setY(double a) {
        this->y = a;
    }

    int getRpt() {
        return rpt;
    }

    void setRpt(int r) {
        this->rpt = r;
    }

    int getRawMaterialStatus() {
        return rawMaterialStatus;
    }

    void setRawMaterialStatus(int r) {
        this->rawMaterialStatus = r;
    }

    int getStatus() {
        return status;
    }

    void setStatus(int s) {
        this->status = s;
    }

    int getWorkbenchId() const{
        return workbenchId;
    }

    void setWorkbenchId(int w) {
        workbenchId = w;
    }

    bool getIsBuyLock() const {
        return isBuyLock;
    }

    void setBuyLock(bool buyLock) {
        isBuyLock = buyLock;
    }

    bool getIsSellLock(int num) {
        return isSellLock[num];
    }

    void setSellLock(bool sellLock, int num) {
        isSellLock[num] = sellLock;
    }

    bool needRawMaterial(int num) {
        return ((this->raw >> (num)) & 1) == 1;
    }

    int getRaw() {
        return raw;
    }

    void setNeedCount(int n) {
        this->needCount = n;
    }

    int getNeedCount() {
        return needCount;
    }

    bool isCanBuy() const{
        return canBuy;
    }

    void setCanBuy(bool b){
        canBuy = b;
    }
};


#endif
