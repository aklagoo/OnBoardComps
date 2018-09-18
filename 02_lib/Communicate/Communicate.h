#ifndef Communicate_h
#define Communicate_h

#include<morse.h>

class Communicate{
private:
    PWMMorseSender _morse_sender;
    int *_sys_mode;
public:
    Communicate(int *sys_mode);
    void morse();
}

#endif
