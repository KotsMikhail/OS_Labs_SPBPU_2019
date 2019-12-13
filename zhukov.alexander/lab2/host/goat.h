#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H
#include "message.h"
#include "connection_info_goat.h"

class goat {
public:
    static int start(const connection_info_goat &connectionInfo);
private:
    static Status status;
    static int GenerateRandNum(int max);
};


#endif //LAB2_GOAT_H
