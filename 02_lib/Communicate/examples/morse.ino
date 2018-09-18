#include<Communicate.h>

int mode;
Communicate gCom;

void setup() {
    gCom = Communicate(&mode);
    gCom.Morse();
}
void loop() {
}
