//================================================================
// Halt, reboot, check status of a pfSense server through Arduino 
// and radio modules. 
// Project is divided in three parts: "keyboard", "servente" and "display".
//
// author: luvak4@gmail.com
//================================================================
// System hardware is divided into 3 parts:
// "servente": communicates via serial RS-232 with pfSense
// "keyboard": performs a remote control. It has 4 buttons. Give commands.
// "display": receive "servente" signals (and other devices, eventually) 
// and display its status on LCM or light.
//
// "servente": receive radio-commands from "keyboard" and transmit response to "display"
// "keyboard": transmit radio-commands to "servente"
// "display": receive radio-commands from "servente"
//
// !----------!               !----------!               !---------!
// ! keyboard !--> tx   rx >--! servente !--> tx   rx >--! display !
// !----------!               !          !               !---------!
//                            !          !
//                            !          !               !---------!
//                            !          !--<> rs232 <>--!         !
//                            !          !               ! pfSense !
//                            !----------!               !---------!
//
//////////////////////////////////////////
//// this is the "keyboard" schematic ////
//////////////////////////////////////////
//
//                     !--------------!
// pushbutton01 (2)>---!              !---> radio tx (12)
// pushbutton02 (3)>---!              !
// pushbutton03 (4)>---!              !
// pushbutton04 (5)>---!              !
//                     !--------------!
//
/////////////////////////////////////
//// this is the "keyboard" code ////
/////////////////////////////////////
#include <IRremote.h>
#include <VirtualWire.h>

const int MSG_LEN = 13;
const int POSIZIONE_CARATT = 11;
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
// Ir receiver pin etc
int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;
// 
int pfSenseInternalStep=1;
int seconds=0;
// for my timing
int dutyCycle = 0;
unsigned long int Pa;
unsigned long int Pb;
// radio modules
const int transmit_pin = 12; 
// pin pushbuttons
const int pinPushButton01 =2;
const int pinPushButton02 =3;
const int pinPushButton03 =4;
const int pinPushButton04 =5;
const int pinLED =13;
// prefix of command to transmit
char msgPushButton[MSG_LEN]  ="pulsPFSE0000";


void dump(decode_results *results) {
  //int count = results->rawlen;
  //if (results->decode_type == UNKNOWN) {
  //  Serial.println("Could not decode message");
  //} 
  
  //else {
  
    long gg =results->value;
    gg =gg - 16712445;
    //gg=gg-12445;
    long res[]={40800,24480,57120,8160,0,48960,56610,42330,36210,26010,38250,44370,11730,5610,30600,3570,13770,22440,16320,18360,20400};
    for (int r=0; r<21 ; r++){
      if (res[r]==gg){
        switch (r){
          case 0:
            txPulsantePremuto('1');
            break;
          case 1:
            txPulsantePremuto('2');
            break;  
          case 2:
            txPulsantePremuto('3');
            break;
          case 3:
            txPulsantePremuto('4');
            break;                                   
        }
      }
    }
  //}
  
  
}

//================================
// setup
//================================
void setup() {
  pinMode(pinLED, OUTPUT);
   // impostazione TX e RX
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000); 
  //
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

void txPulsantePremuto(char nPushButton){
  digitalWrite(pinLED, HIGH);
  msgPushButton[POSIZIONE_CARATT]=nPushButton;
  vw_send((uint8_t *)msgPushButton,MSG_LEN);
  vw_wait_tx(); // Wait until the whole message is gone
  msgPushButton[POSIZIONE_CARATT]='0';
   digitalWrite(pinLED, LOW);
}
