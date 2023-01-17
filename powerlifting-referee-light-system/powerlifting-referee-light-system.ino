/* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*  Software by:
 *   _______ _____  _  ____      __   _____ _____        _____ _______       _____ _    _  _____
 *  |__   __/ ____|| |/ /\ \    / /  / ____|  __ \ /\   |  __ \__   __|/\   / ____| |  | |/ ____|
 *     | | | (___  | \' /  \ \  / /  | (___ | |__) /  \  | |__) | | |  /  \ | |    | |  | | (___
 *     | |  \___ \ |  <    \ \/ /    \___ \|  ___/ /\ \ |  _  /  | | / /\ \| |    | |  | |\___ \
 *     | |_ ____) || . \ _  \  /     ____) | |  / ____ \| | \ \  | |/ ____ \ |____| |__| |____) |
 *     |_(_)_____(_)_|\_(_)  \(_)   |_____/|_| /_/    \_\_|  \_\ |_/_/    \_\_____|\____/|_____/
 *
 *     Tilburg, The Netherlands.
 *     https://tskvspartacus.nl/powerlifting
 *     zeus [[[at]]]] tskvspartacus.nl
 */

// Bounce2 V2.71 library from: https://github.com/thomasfredericks/Bounce2
#include <Bounce2.h>

// Declare pins
const int refereeonewhitePin = 2; // Right referee
const int refereeoneredPin = 3;
const int refereeheadwhitePin = 5;
const int refereeheadredPin = 6;
const int refereethreewhitePin = 8; // Left referee
const int refereethreeredPin = 9;
const bool buttonsPressedState = LOW;
const int lightonewhitePin = A4;
const int lightoneredPin = A5;
const int lightheadwhitePin = A0;
const int lightheadredPin = A1;
const int lightthreewhitePin = A2;
const int lightthreeredPin = A3;
const int buzzerPin = 7;

// Configuration
const int buzzerLength = 700; // length of the buzzer in ms, the last referee is unable to declare its judgement during this time if buzzimmediately = 1:
const int buzzimmediately = 0; // 1 = yes, as per IPF rules; 0 = only after all referees made a judgment
long interval = 15000; // time after when to reset in ms
bool debug = true;

//
// DON'T CHANGE ANYTHING BELOW
//

// variables that will change:
int refereeonewhiteState = 0;
int refereeoneredState = 0;
int refereeheadwhiteState = 0;
int refereeheadredState = 0;
int refereethreewhiteState = 0;
int refereethreeredState = 0;
int numrefchoices = 0;
int numredlights = 0;
int buzzerwent = 0;
int refereeone = 0;
int refereehead = 0;
int refereethree = 0;
unsigned long lastMillis = 0; // last time update
int serialmsgnumber = 1000;

// setup debounce objects
Bounce2::Button debouncer1 = Bounce2::Button();
Bounce2::Button debouncer2 = Bounce2::Button();
Bounce2::Button debouncer3 = Bounce2::Button();
Bounce2::Button debouncer4 = Bounce2::Button();
Bounce2::Button debouncer5 = Bounce2::Button();
Bounce2::Button debouncer6 = Bounce2::Button();

void setup() {
  // initialize
  pinMode(refereeonewhitePin, INPUT);
  debouncer1.attach(refereeonewhitePin);
  debouncer1.interval(20); // interval in ms
  debouncer1.setPressedState(buttonsPressedState);
  pinMode(refereeoneredPin, INPUT);
  debouncer2.attach(refereeoneredPin);
  debouncer2.interval(20); // interval in ms
  debouncer2.setPressedState(buttonsPressedState);

  pinMode(refereeheadwhitePin, INPUT);
  debouncer3.attach(refereeheadwhitePin);
  debouncer3.interval(20); // interval in ms
  debouncer3.setPressedState(buttonsPressedState);
  pinMode(refereeheadredPin, INPUT);
  debouncer4.attach(refereeheadredPin);
  debouncer4.interval(20); // interval in ms
  debouncer4.setPressedState(buttonsPressedState);

  pinMode(refereethreewhitePin, INPUT);
  debouncer5.attach(refereethreewhitePin);
  debouncer5.interval(20); // interval in ms
  debouncer5.setPressedState(buttonsPressedState);
  pinMode(refereethreeredPin, INPUT);
  debouncer6.attach(refereethreeredPin);
  debouncer6.interval(20); // interval in ms
  debouncer6.setPressedState(buttonsPressedState);

  pinMode(buzzerPin, OUTPUT);
  pinMode(lightonewhitePin, OUTPUT);
  pinMode(lightoneredPin, OUTPUT);
  pinMode(lightheadwhitePin, OUTPUT);
  pinMode(lightheadredPin, OUTPUT);
  pinMode(lightthreewhitePin, OUTPUT);
  pinMode(lightthreeredPin, OUTPUT);

  // test all lights
  lights(HIGH,HIGH,HIGH,HIGH,HIGH,HIGH);
  delay(2000);

  // turn all lights off
  reset();

  // start serial decision reporting
  Serial.begin(9600); // set up Serial library at 9600 bps
  if (debug) { Serial.println("debug: System online!"); }

}

void loop() {

  if ( refereeone == 0 ) {
    debouncer1.update();
    debouncer2.update();
    refereeonewhiteState = debouncer1.rose();
    refereeoneredState = debouncer2.rose();
    if (refereeonewhiteState == HIGH || refereeoneredState == HIGH) {
      if (debug) { Serial.println("debug: Referee one (right) pressed"); }
      if (refereeoneredState == HIGH) { ++numredlights; }
      ++numrefchoices;
      refereeone = 1;
      lastMillis = millis();
    }
  }
  if ( refereehead == 0 ) {
    debouncer3.update();
    debouncer4.update();
    refereeheadwhiteState = debouncer3.rose();
    refereeheadredState = debouncer4.rose();
    if (refereeheadwhiteState == HIGH || refereeheadredState == HIGH) {
      if (debug) { Serial.println("debug: Referee head pressed"); }
      if (refereeheadredState == HIGH) { ++numredlights; }
      ++numrefchoices;
      refereehead = 1;
      lastMillis = millis();
    }
  }
  if ( refereethree == 0 ) {
    debouncer5.update();
    debouncer6.update();
    refereethreewhiteState = debouncer5.rose();
    refereethreeredState = debouncer6.rose();
    if (refereethreewhiteState == HIGH || refereethreeredState == HIGH) {
      if (debug) { Serial.println("debug: Referee three (left) pressed"); }
      if (refereethreeredState == HIGH) { ++numredlights; }
      ++numrefchoices;
      refereethree = 1;
      lastMillis = millis();
    }
  }

  if (buzzimmediately == 1 && numredlights > 1) {
    if ( buzzerwent == 0 ) {
      digitalWrite(buzzerPin, HIGH);
      delay(buzzerLength);
      digitalWrite(buzzerPin, LOW);
      buzzerwent = 1;
    }
  }

  if (numrefchoices == 3) {
    lights(refereeonewhiteState,refereeoneredState,refereeheadwhiteState,refereeheadredState,refereethreewhiteState,refereethreeredState);
    serialmessage(refereeonewhiteState,refereeoneredState,refereeheadwhiteState,refereeheadredState,refereethreewhiteState,refereethreeredState);

    if (buzzimmediately == 0 && numredlights > 1) {
	    digitalWrite(buzzerPin, HIGH);
	    delay(buzzerLength);
	    digitalWrite(buzzerPin, LOW);
	  }

    delay(10000);
    reset();
  }

  // auto reset
  if (lastMillis > 0 ) {
    unsigned long currentMillis = millis();
    if(currentMillis - lastMillis > interval) {
	   if (buzzerwent == 1) {
		   // output serial message with the red lights from the referees who did declare judgment
	     // this is to make up for edge case when the 3rd referee made a choice during the buzzer
	     serialmessage(refereeonewhiteState,refereeoneredState,refereeheadwhiteState,refereeheadredState,refereethreewhiteState,refereethreeredState);
	   }
     if (debug) { Serial.println("debug: Did not receive all judgements in 15 seconds. Resetting.."); }
     reset();
    }
  }

}

void lights(char onewhite, char onered, char headwhite, char headred, char threewhite, char threered) { // 1 is on; 0 is off
  digitalWrite(lightonewhitePin, lightmode(onewhite));
  digitalWrite(lightoneredPin, lightmode(onered));
  digitalWrite(lightheadwhitePin, lightmode(headwhite));
  digitalWrite(lightheadredPin, lightmode(headred));
  digitalWrite(lightthreewhitePin, lightmode(threewhite));
  digitalWrite(lightthreeredPin, lightmode(threered));
}

char lightmode(char state) { // 1 is on; 0 is off
  //if (state == 1) { return LOW; } // LOW is on for our relays
  //if (state == 0) { return HIGH; }
  if (state == HIGH) { return LOW; }
  //if (state == NULL) { return HIGH; } // if variable not set
  else { return HIGH; }
}

void serialmessage(char onewhite, char onered, char headwhite, char headred, char threewhite, char threered) {
  serialmsgnumber = serialmsgnumber + 1;
  Serial.print(serialmsgnumber);
  Serial.print(";");

  // if no choice was made, will be be printed
  // (edge case for buzzer on 2 red lights and the 3rd referee made its choice during the buzzer)
  if (threewhite == HIGH && threered == LOW) { // three is left referee
    Serial.print("L:W;");
  }
  if (threewhite == LOW && threered == HIGH) {
    Serial.print("L:R;");
  }
  if (headwhite == HIGH && headred == LOW) {
    Serial.print("H:W;");
  }
  if (headwhite == LOW && headred == HIGH) {
    Serial.print("H:R;");
  }
  if (onewhite == HIGH && onered == LOW) { // one is right referee
    Serial.print("R:W;");
  }
  if (onewhite == LOW && onered == HIGH) {
    Serial.print("R:R;");
  }
  Serial.println();
}

void reset() {
  lights(LOW,LOW,LOW,LOW,LOW,LOW);

  numrefchoices = 0;
  numredlights = 0;
  buzzerwent = 0;

  refereeone = 0;
  refereehead = 0;
  refereethree = 0;

  refereeonewhiteState = LOW;
  refereeoneredState = LOW;
  refereeheadwhiteState = LOW;
  refereeheadredState = LOW;
  refereethreewhiteState = LOW;
  refereethreeredState = LOW;

  lastMillis = 0;
}

