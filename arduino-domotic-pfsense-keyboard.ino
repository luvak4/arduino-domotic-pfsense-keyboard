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

#include <VirtualWire.h>
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
char msgPushButton[13]  ="pulsPFSE0000";
//================================
// setup
//================================
void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinPushButton01, INPUT);
  pinMode(pinPushButton02, INPUT);
  pinMode(pinPushButton03, INPUT);
  pinMode(pinPushButton04, INPUT);
   // impostazione TX e RX
  vw_set_tx_pin(transmit_pin);
  vw_setup(2000);      
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
	if (digitalRead(pinPushButton01)){
	  txPulsantePremuto('1');
	}
	if (digitalRead(pinPushButton02)){
	  txPulsantePremuto('2');
	}
	if (digitalRead(pinPushButton03)){
	  txPulsantePremuto('3');
	}
	if (digitalRead(pinPushButton04)){
	  txPulsantePremuto('4');
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
      digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);
  msgPushButton[11]=nPushButton;
  vw_send((uint8_t *)msgPushButton,13);
  vw_wait_tx(); // Wait until the whole message is gone
  msgPushButton[11]='0';
}
