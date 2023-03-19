#include <iostream>
#include "work.hpp"
using namespace std;

Work* work = new Work();


bool readMapOK() {
    char line[1024];
    int num = 0;
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something
        work->setDataLine(num, line);
        num += 1;
    }
    return false;
}

bool readFrameOK() {
    char line[1024];
    int num = 0;
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        //do something
        string str(line);
//        fprintf(stderr, "%s", str.data());
//        fprintf(stderr, "%s\n", to_string(work->getWorkbenches().size()).c_str());
        work->updateStatus(num, str);
        num += 1;
    }
    return false;
}

int main() {

    readMapOK();
    puts("OK");
    fflush(stdout);
    int frameID, k, money;
    while (scanf("%d", &frameID) != EOF) {
        scanf("%d", &money);
        scanf("%d ", &k);

        readFrameOK();
        printf("%d\n", frameID);

        vector<string> orders = work->schedulingRobot(frameID);

        for (const auto &item: orders){
            printf("%s", item.data());
        }

        printf("OK\n");
        fflush(stdout);
    }
    return 0;
}
