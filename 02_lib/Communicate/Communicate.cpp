#define PIN_FMT 3
#define PIN_FMT_SWITCH 5

#include "Arduino.h"
#include <morse.h>
#include "Communicate.h"

Communicate::Communicate(int *sys_mode){
    _sys_mode = sys_mode;
    _morse_sender = PWMMorseSender(PIN_FMT);
    _morse_sender.setup();
}

void Communicate::Morse(){
    digitalWrite(PIN_FMT_SWITCH, HIGH); /* Switch the FMT on. */
    _morse_sender.setMessage("testing ");
    _morse_sender.send_blocking();
}
