//Directives
#include <EEPROM.h>
#include <RH_ASK.h>
#include <SPI.h>

#define DEPLOY_OUT  2
#define KILL_SWITCH 3
#define FTM_SWITCH  4

#define MT_RX_PIN_1 5
#define MT_RX_PIN_2 6
#define MT_TX_OUT   7
#define MT_AMP      8
#define ADC_SWITCH  9
#define ADC_INTR    10
#define ADC_ADDR    1
#define PAY_SWITCH  11
#define PAY_INTR    12
#define PAY_ADDR    2
#define RESET_ACTIVE 13
#define MORSE_TX_OUT 0
#define BATT_READ_IN 1
#define EEPROM_ADDR 0

#define MAX_BATT_VOTLAGE 5
#define SAFE_VOLTAGE 3.5
#define CUT_TIME 300000

#define DETUMBLE 0
#define MONITOR 1
#define ADC_DATA_LENGTH 10      //Change it later according to the actual sensors

#define BUFF_MAX 20

//Global variables
RH_ASK radio;
int start_count;
int state=0;
byte instr;
char adc_sensors[ADC_DATA_LENGTH];

//Functions
void init_pins(){
  //Outputs - MT_RX_PIN_1 and MT_RX_PIN_2::NOT INCLUDED
  pinMode(DEPLOY_OUT, OUTPUT);
  pinMode(KILL_SWITCH, OUTPUT);
  pinMode(FTM_SWITCH, OUTPUT);
  pinMode(MT_TX_OUT, OUTPUT);
  pinMode(MT_AMP, OUTPUT);
  pinMode(ADC_SWITCH, OUTPUT);
  pinMode(ADC_INTR, OUTPUT);
  pinMode(PAY_SWITCH, OUTPUT);
  pinMode(PAY_INTR, OUTPUT);
}
void init_I2C(){
  Wire.begin();
}


//DEPLOY______________________
void deploy_handler(){
  //Giving a high signal for 1s
  digitalWrite(DEPLOY_OUT, HIGH);
  delay(CUT_TIME);
  digitalWrite(DEPLOY_OUT, LOW);
}

//I2C_________________________
void I2C_send(int address, byte instr_data[]){
  Wire.beginTransmission(address);
  Wire.transmit(instr_data);
  Wire.endTransmission();
}

//ADC_DATA____________________
void ADC_fetch_data(){
  int i=0;
  Wire.requestFrom(ADC_ADDR, ADC_DATA_LENGTH);
  while(Wire.available()){
    data[i++] = Wire.read();
  }
}

//UPLINK READ_________________
void read_uplink(){
  uint8_t buff[BUFF_MAX];
  uint8_t bufflen = sizeof(buff);
  if(driver.recv(buff, &bufflen)){
    int i;

    if((char)buff[0]=='1'){

    }
  }
}

//Main==============================================================
void setup(){
  start_count = EEPROM.read(EEPROM_ADDR)+1;
  EEPROM.write(EEPROM_ADDR, start_count);
  init_pins();
  while(check_power()<SAFE_VOLTAGE){
    sleep(300000);
  }
  init_I2C();
  for(int x=0; x<5; x++){
    Beacon.official();
    delay(180000);
  }
  if(start_count==1){
    while(check_power()<SAFE_VOLTAGE){
      sleep(300000);
    }
    deploy_handler();
    for(int x=0; x<5; x++){
      Beacon.morse("BSAT has started");
      delay(180000);
    }
    while(check_power()<SAFE_VOLTAGE){
      sleep(300000);
    }

    byte instr[] = {DETUMBLE};
    I2C_send(ADC_ADDR, instr);
    
    for(int x=0; x<5; x++){
      Beacon.morse("BSAT has detumbled");
      delay(180000);
    }
  }
  while(check_power()<SAFE_VOLTAGE){
    sleep(300000);
  }
}
void loop(){
  //ADC fetch data
  ADC_fetch_data();
  Beacon.morse("This is BhulochanSat working A okay.");
  read_uplink();
  watchdog();
}