#ifndef MESSAGE_H
#define MESSAGE_H

const int MIN_TEMP = -30;
const int MAX_TEMP = 30;

class message_t {
    public:
        unsigned int day;
        unsigned int month;
        unsigned int year;
        int temperature;

        message_t(unsigned int day = 0, unsigned int month = 0, unsigned int year = 0)
            : day(day), month(month), year(year), temperature(0) 
        {}
};

#endif