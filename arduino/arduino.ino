/*
   The circuit:
  - LED attached from pin 13 to ground
  - pushbutton attached to pin 2 from +5V
  - 10K resistor attached to pin 2 from ground

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.

  created 2005
  by DojoDave <http://www.0j0.org>
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Button
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int statusPin = 7;

// variables will change:
int lidState = 0;
int prevLidState = 0;
int prevStatusState = 0;
int statusState = 0;
int intervalCigarettes = 5;
int smoked = 0;
unsigned long lastCigarTime = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(statusPin, INPUT);
  Serial.begin(9600);
}

void signalUnlocked() {
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
}

void signalLocked() {
  delay(100);
  for (int i = 0; i < 2; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(150);
  }
  digitalWrite(ledPin, HIGH);
  delay(300);
  digitalWrite(ledPin, LOW);
}

void loop() {
  lidState = digitalRead(buttonPin);
  statusState = digitalRead(statusPin);

  if (lidState == LOW && prevLidState == HIGH) {
    //increment number cigarettes;
    smoked = smoked + 1;
    Serial.print("Smoked:");
    Serial.print(smoked);
    lastCigarTime = millis();
  }
  if (statusState == LOW && prevStatusState == HIGH) {
    long now = millis();
    Serial.print(now + '>' + (lastCigarTime + intervalCigarettes * 1000));
    if (now > (lastCigarTime + intervalCigarettes * 1000)) {
      Serial.print("Unlocked\n");
      signalUnlocked();
    } else {
      Serial.print("Locked\n");
      signalLocked();
    }
  }
  prevLidState = lidState;
  prevStatusState = statusState;
}
