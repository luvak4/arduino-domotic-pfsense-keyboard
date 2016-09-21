//================================================================
// Halt, reboot, check status of a pfSense server through Arduino 
// and radio modules. 
// Project is divided in three parts: "keyboard", "servente" and "display".
//
// author: luvak4@gmail.com
//================================================================
//
/////////////////////////////////////
//// this is the "KEYBOARD" code ////
/////////////////////////////////////
#include <IRremote.h>
#include <VirtualWire.h>
// max lenght of my message
const int MSG_LEN = 13;
// position of character to change
const int POSIZIONE_CARATT = 11;
//
const int pinLED =13;
// timing loop
int dutyCycle = 0;
unsigned long int Pa;
unsigned long int Pb;
// radio modules
const int transmit_pin = 12;
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
// IR receiver
int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
// 
int pfSenseInternalStep=1;
int seconds=0;
// prefix to transmit
char msgPushButton[MSG_LEN]  ="pulsPFSE0000";
// ir-keyboard constant
const long irK = 16712445;
// ir-keyboard codes (reduce by irK)
const long res[]={40800,24480,57120,8160,0,48960,56610,42330,\
 	    36210,26010,38250,44370,11730,5610,30600,\
	    3570,13770,22440,16320,18360,20400};

//================================
// IR dump
//================================
void dump(decode_results *results) {
    long gg =results->value;
    // const relate to ir-control keyboard
    gg =gg - irK;
    // find if in array
    for (int r=0; r<21 ; r++){
      if (res[r]==gg){
        switch (r){
          case 0:
	    // HALT
            txPulsantePremuto('1');
            break;
          case 1:
	    // REBOOT
            txPulsantePremuto('2');
            break;  
          case 2:
	    // SYNC
            txPulsantePremuto('3');
            break;
          case 3:
	    // PING
            txPulsantePremuto('4');
            break;                                   
        }
      }
    }
}
//================================
// setup
//================================
void setup() {
  // led
  pinMode(pinLED, OUTPUT);
  // radio tx
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000); // speed
  // IR receiver enabled
  irrecv.enableIRIn();     
}
//================================
// loop
//================================
void loop() {
  //--------------------------------
  // time subdivision
  //--------------------------------
  unsigned long int Qa;
  unsigned long int Qb;
  int DIFFa;
  int DIFFb;
  int Xa;
  int Xb;
  //
  dutyCycle += 1;
  if (dutyCycle > 9){
    dutyCycle = 0;
  }
  if (dutyCycle > 0){
    Qa=millis();
    if (Qa >= Pa){
      DIFFa=Qa-Pa;
      Xa = DIFFa - 25;
      if (Xa >= 0){
	Pa = Qa;
	//--------------------------------
	// every 0.025 Sec
	//--------------------------------
      }
    } else {
      Pa = Qa - Xa;
    }
  } else {  
    Qb=millis();
    if (Qb >= Pb){
      DIFFb=Qb-Pb;
      Xb = DIFFb - 1000;
      if (Xb >= 0){
	Pb = Qb - Xb;
	//--------------------------------
	// BEGIN every second
	//--------------------------------
	if (irrecv.decode(&results)) {
	  // receive
	  dump(&results);
	  irrecv.resume();
	}
	//--------------------------------
	// END every second
	//--------------------------------
      }
    } else {
      Pb = Qb;
    }      
  }
}
//================================
// send pushButton via radio tx
//================================
void txPulsantePremuto(char nPushButton){
  digitalWrite(pinLED, HIGH);
  msgPushButton[POSIZIONE_CARATT]=nPushButton;
  vw_send((uint8_t *)msgPushButton,MSG_LEN);
  vw_wait_tx(); // Wait until the whole message is gone
  msgPushButton[POSIZIONE_CARATT]='0';
  digitalWrite(pinLED, LOW);
}
