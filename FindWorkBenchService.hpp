//
// Created by lzh on 03/04/23.
//

#ifndef CODECRAFTSDK_FINDWORKBENCHSERVICE_HPP
#define CODECRAFTSDK_FINDWORKBENCHSERVICE_HPP

class FindWorkBenchService {
public:
    virtual void findWorkbenchSell(int robotId, int frameId){}
    virtual void findWorkbenchBuy(int robotId, int frameId){}

    static bool hasMaterialNum(int num, int status) {
        return ((status >> (num)) & 1) == 1;
    }

};

#endif //CODECRAFTSDK_FINDWORKBENCHSERVICE_HPP
