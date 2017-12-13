#include<SPI.h>
#include<SD.h> //also include any other library required
const byte interruptPin = 2;
void setup() 
{
  Serial.begin(9600);//required only during testing and prototyping
  if(!SD.begin(4))
  {
    Serial.println("card failure!");//use only for debugging
    transmit.beacon(0);//0 signifies "callsign+This is b-sat reset+callsign" other modes latter 
    reset();//pls try if this restarts system or not...if not pls change it to some logic function to restart  
  }
  if(check.sat())//takes the reading sets the orientation and sets flag data in variables and file returns 1 if there is an error
  {
   transmit.beacon(1);//1 signifies "callsign+This is b-sat attitude error+callsign"
   reset();
  }
  softserial camera=new softserial(campina,campinb);//needs to install sofserial library and get camera on pins a and b
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin),uplink, CHANGE);//create uplink function
}

void loop() {
  check.getflags();//get flags in variable....these are current states as indicated by output class sensors
  transmit.beacon(2);//2 signifies "callsign+This is b-sat +flag 1 +flag 2 +flag 3+callsign"
  while(!locate())//locate function checks if we are at location or if the capture uplink is fired and ORs them
  {
   beacon(2);
   delay(waittime); 
  }
  if(uplinkch=5)
  {
   transmit.beacon(4);//this sends a static image via Lora
  }
  else
  {cam.capture();
  transmit.beacon(3);// this transmits image via rf downlink
  }
 }
