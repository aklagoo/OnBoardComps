//===========================SECTION 1===========================
//Includes
#include<Wire.h>
#include<beacon.h>		//https://github.com/markfickett/arduinomorse
#include<sleep.h>			//Arduino safe_sleep - Either use interrupts or library
#include<EEPROM.h>		//For reading EEPROM values
#include<RTClib.h>		//https://github.com/adafruit/RTClib/blob/master/examples/ds3231/ds3231.ino

//Defines

//EEPROM flags
#define E_START				0
#define E_DEPLOYED		2
#define E_DETUMBLED		4
#define E_HANDSHAKE		6
#define E_TIME				8

//Pins
#define P_DEPLOY			2
#define P_FMT					3
#define P_KILL				4
#define P_FMT_SWITCH	5
#define P_ADC_SWITCH	7
#define P_PAY_SWITCH	8
#define P_RF_TX				11
#define P_RF_RX				12
#define P_RESET				13
#define P_BAT_LEVEL		A2
#define P_TEMP_LM35		A3

//Modes
#define M_INIT			0
#define M_NORMAL		1
#define M_DEBUG			2

//safe_sleep timings
#define S_HANDSHAKE_DEL	100
#define S_LOW_SLEEP		10000
#define S_OP_SLEEP		5000
#define S_BEAC_SLEEP	5000
#define S_DEPLOY		10000

//I2C addresses
#define A_ADC			0
#define A_PAY			1

//Battery constants
#define B_VOLTAGE		10
#define B_SLP_LEVEL		40
#define B_OP_LEVEL		60

//Temperature constants
#define T_OP_HIGH		65

//Commands
#define C_ADC_DETUMBLE	0

//Lengths
#define L_BEACON_VALUES	10

//Global variables
beacon Beacon(P_FMT, P_RF_TX);
RTC_DS3231 rtc;

//===========================SECTION 2===========================
void setup(){
	init();								//Initialize pins and I2C
	watchdog();							//safe_sleep if conditions are unsuitable
	start_incr();						//Increment the start counter
	if(EEPROM.read(E_DEPLOYED)==0){
		deploy();
	}
	watchdog();
	while(EEPROM.read(E_DETUMBLED)==0){
		detumble();
	}
}
void loop(){
	while(!EEPROM.read(E_HANDSHAKE)){	//Check for a handshake
		Beacon.voice(M_INIT);
		safe_sleep(S_BEAC_SLEEP);
		Beacon.morse(M_INIT);
		safe_sleep(S_BEAC_SLEEP);
		watchdog();
		upl_chkrun();
		safe_sleep(S_HANDSHAKE_DEL);
	}
	if(homeposition()){
		Beacon.official(getBeaconValues());    //Add all parts here
	}
	Beacon.upl_chkrun();
	watchdog();
	safe_sleep(S_BEAC_SLEEP);
	Beacon.voice(M_NORMAL);
	safe_sleep(S_BEAC_SLEEP);
	Beacon.morse(M_NORMAL);
	watchdog();
}

//===========================SECTION 3===========================
void init(){
	//Initialize pins
	pinMode(P_DEPLOY, OUTPUT);
	pinMode(P_FMT, OUTPUT);
	pinMode(P_KILL, OUTPUT);
	pinMode(P_ADC_SWITCH, OUTPUT);
	pinMode(P_PAY_SWITCH, OUTPUT);
	pinMode(P_RF_TX, OUTPUT);
	pinMode(P_RF_RX, OUTPUT);
	pinMode(P_RESET, OUTPUT);

	//Init I2C and RTC
	Wire.begin();
	rtc.begin();
}

void watchdog(){
	int batt_level = (analogRead(P_BAT_LEVEL)*(5.0/1023.0)*100)/B_VOLTAGE;
	int temp = (analogRead(P_TEMP_LM35)*(5.0/1024.0))*100;

	//Check battery levels
	if(batt_level<B_SLP_LEVEL||temp>T_OP_HIGH){
		safe_sleep(S_LOW_SLEEP);
	}
	else{
		if(batt_level>B_SLP_LEVEL&&batt_level<B_OP_LEVEL){
			safe_sleep(S_OP_SLEEP);
		}
	}
}

void start_incr(){
	EEPROM.write(E_START,(EEPROM.read(E_START)+1));
}

void deploy(){
	digitalWrite(P_DEPLOY, HIGH);
	delay(S_DEPLOY);
	digitalWrite(P_DEPLOY, LOW);
	EEPROM.write(E_DEPLOYED, 1);
}

void detumble(){
	//Send command to ADC
	Wire.beginTransmission(A_ADC);
	Wire.write(C_ADC_DETUMBLE);
	Wire.endTransmission();

	//Request acknowledgement
	Wire.requestFrom(A_ADC, 1);
	while(Wire.available()){
		EEPROM.write(Wire.read());
	}
}

uint8_t *getBeaconValues(){
	uint8_t values[L_BEACON_VALUES];
	values[0] = (uint8_t)((analogRead(P_TEMP_LM35)*(5.0/1024.0))*100);
	values[1] = (uint8_t)((analogRead(P_BAT_LEVEL)*(5.0/1023.0)*100)/B_VOLTAGE);
	values[2] = (uint8_t)(EEPROM.read(E_START)-1);
	values[3] = (uint8_t)(EEPROM.read(E_TIME));
	values[4] = (uint8_t)(EEPROM.read(E_TIME)+1);
	values[5] = (uint8_t)(EEPROM.read(E_TIME)+2);
	values[6] = (uint8_t)(EEPROM.read(E_TIME)+3);
	values[7] = (uint8_t)(EEPROM.read(E_TIME)+4);
	values[8] = (uint8_t)(EEPROM.read(E_TIME)+5);
	values[9] = (uint8_t)(EEPROM.read(E_TIME)+6);
	return values;
}
/*                                  BYTES
TEMP                                1
BATT_PERC                           1
RESTARTS                            1
LAST COMMAND TIME AND LAST COMMAND  2+1+*5=7, 1
MEM_VALUE
TOTAL = 10/11 Bytes
*/
