#include<RH_ASK.h>
#include<morse.h>

//Modes
#define M_INIT			0
#define M_NORMAL		1
#define M_DEBUG			2

//Constants
#define L_MAX_SPECIAL 40
#define L_MAX_UPLINK  41
#define A_PAY			    1
#define C_PAY_VOICE   0
#define E_HANDSHAKE		6
#define P_RF_TX				11
#define P_RF_RX				12
#define P_FMT_SWITCH	5
#define P_FMT					3
#define P_RESET				13
#define P_KILL				4
#define T_FMWAIT      5000

//Uplink codes
#define U_KILL        0
#define U_RESET       1
#define U_HANDSHAKE   2
#define U_FORCE_INIT  3
#define U_SYS_TIME    4
#define U_NEXT_PASS   5
#define U_VOICE_STR   6
#define U_PIC_TYPE    7
#define U_SSTV_TIME   8

class beacon{
    PWMMorseSender morse_sender;
    RH_ASK official_transceiver;
    char special[L_MAX_SPECIAL];

public:
    beacon(){
        morse_sender = PWMMorseSender(P_FMT);
        morse_sender.setup();                   //Set words per minute
        official_sender.init();
    }
    void morse(int mode){
        pinMode(P_FMT_SWITCH, HIGH);
        if(mode==M_NORMAL){
            morse_sender.setMessage("VU3OH This is BholochanSat in normal mode. VU3OH");
        }
        else if(mode==M_INIT){
            morse_sender.setMessage("VU3OH This is BholochanSat in initial mode. VU3OH");
        }
        if (!morse_sender.continueSending())
	      {
		        morse_sender.startSending();
	      }
        pinMode(P_FMT_SWITCH, LOW);
    }
    void official(uint8_t *message, int length){
        official_transceiver.send(msg, length);
        official_transceiver.waitPacketSent();
    }
    void voice(){
      byte acknowledge;
      Wire.beginTransmission(A_PAY);
      Wire.write(C_PAY_VOICE);
      Wire.endTransmission();
      Wire.requestFrom(A_PAY, 1);
      while(Wire.available()){
        acknowledge = Wire.read();
      }
      if(acknowledge){
        pinMode(P_FMT_SWITCH, HIGH);
        Wire.beginTransmission(A_PAY);
        Wire.write(special);
        Wire.endTransmission();
        delay(T_FMWAIT);
        pinMode(P_FMT_SWITCH, LOW);
      }
    }
    void upl_chkrun(){
      uint8_t buffer[L_MAX_UPLINK];
      uint8_t bufflen = sizeof(buffer);

      if(official_transceiver.recv(buffer, &bufflen)){
        int i;

        if(buffer[0]==U_HANDSHAKE){
          EEPROM.write(E_HANDSHAKE, 1);
        }
        else{
          if(EEPROM.read(E_HANDSHAKE)==0){
            switch(buffer[0]){
              case U_KILL:
                pinMode(P_KILL, HIGH);
                break;
              case U_RESET:
                pinMode(P_RESET, HIGH);
                break;
              case U_FORCE_INIT:
                EEPROM.write(E_HANDSHAKE, 0);
                break;

              case U_SYS_TIME:
                uint16_t year = ((uint16_t)buffer[1]<<8) | ((uint16_t)buffer[2]);
                uint8_t month = buffer[3];
                uint8_t day = buffer[4];
                uint8_t hour = buffer[5];
                uint8_t minute = buffer[6];
                uint8_t second = buffer[7];

                rtc.adjust(DateTime(year, month, day, hour, minute, second));
                break;

              case U_NEXT_PASS:
                
            }
          }
        }
      }
    }
}
