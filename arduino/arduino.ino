#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>  
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
#define MAX_BUFFER 4
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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN, NEO_GRB + NEO_KHZ800);

int bluetoothTx = 2;  // TX-O pin of bluetooth mate, Arduino D2
int bluetoothRx = 3;  // RX-I pin of bluetooth mate, Arduino D3
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

// constants won't change. They're used here to set pin numbers:
const int triggerPin = 5;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin
const int statusPin = 4;
const int incPin = 6;
const int decPin = 7;
const int infraredPin = 5;
const int RED = 1;
//const int GREEN = 2;
const int BLUE = 3;

// variables will change:
int lidState = LOW;
int incState = LOW;
int decState = LOW;
int prevLidState = LOW;
int prevStatusState = LOW;
int prevIncState = LOW;
int prevDecState = LOW;
int statusState = LOW;
int infraredValue = 0;
//int intervalCigarettes = 5;
int smoked = 0;
//unsigned long lastCigarTime = 0;
int limit = 8;
char* buffer;
boolean receiving = false;
int pos;


int ledPins[] = {13, 12, 11, 10, 9, 8, 7, 6, 5, 4};
int leds[] = {BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE, BLUE};
int ledSignals[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
int LEDS_SIZE = 8;
int LEDS_SIGNALS_SIZE = 10;
int PERCENTAGE_DIVISION = 100 / LEDS_SIZE;

void setup() {
  // initialize the LED pin as an output:
  int i;
  for (i = 0; i < LEDS_SIGNALS_SIZE; i++) {
    pinMode(ledPins[i], OUTPUT); 
  }
  // initialize the pushbutton pin as an input:
  pinMode(triggerPin, INPUT);
  pinMode(statusPin, INPUT);
  pinMode(incPin, INPUT);
  pinMode(decPin, INPUT);
  Serial.begin(9600);
  strip.begin();
  strip.show();
  strip.setBrightness(5);
  
  //BLUETOOTH
  //Serial.begin(9600);  // Begin the serial monitor at 9600bps
  bluetooth.begin(115200);  // The Bluetooth Mate defaults to 115200bps
  bluetooth.print("$");  // Print three times individually
  bluetooth.print("$");
  bluetooth.print("$");  // Enter command mode
  delay(100);  // Short delay, wait for the Mate to send back CMD
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  // 115200 can be too fast at times for NewSoftSerial to relay the data reliably
  bluetooth.begin(9600);  // Start bluetooth serial at 9600
  buffer = new char[MAX_BUFFER];
  resetData();
}

void resetLeds() {
  int i;
  for (i = 0; i < LEDS_SIZE; i++) {
    leds[i] = BLUE;
  }
  /*for (i = 0; i < LEDS_SIGNALS_SIZE; i++) {
    ledSignals[i] = LOW;
  }*/
}

void convertPercent2Colors() {
  int percentage = (int) (smoked * 100 / limit);
  //Serial.print("Percentage: ");
  //Serial.print(percentage);
  resetLeds();
  /*0-20 0/5 reds (all blues)
  20-40 1/5 reds
  40.60 2/5 reds
  60-80 3/5 reds
  80-100 4/5 reds
  100 - all reds*/
  int maxSize = percentage / PERCENTAGE_DIVISION;
  int i;
  if (maxSize > LEDS_SIZE) { maxSize = LEDS_SIZE; }
  for (i = 0; i < maxSize; i++) {
    leds[i] = RED;
  }
}

void mapLeds() {
  int i;
  //Serial.print(" LEDS: ");
  for (i = 0; i < LEDS_SIZE; i++) {
    if (leds[i] == BLUE) {
      //Serial.print("B"); 
      ledSignals[i*2] = HIGH;
      ledSignals[i*2+1] = LOW;
    } else {
      //Serial.print("R");
      ledSignals[i*2] = LOW;
      ledSignals[i*2+1] = HIGH;
    }
  }
}

/*void showStatus() {
  //Serial.print(" LEDS: ");
  delay(100);
  for (int i = 0; i < LEDS_SIGNALS_SIZE; i++) {
    //Serial.print(ledSignals[i]);
    digitalWrite(ledPins[i], ledSignals[i]);
  }
  delay(2000);
  for (int i = 0; i < LEDS_SIGNALS_SIZE; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}*/

void showStatus(int interval) {
  uint32_t red = strip.Color(255, 0, 0);
  uint32_t blue = strip.Color(0, 0, 255);
  uint16_t i;
  for(i=0; i < strip.numPixels(); i++) {
    if (leds[i] == BLUE) {
      strip.setPixelColor(i, blue);
    } else {
      strip.setPixelColor(i, red);
    }
  }
  strip.show();
  delay(interval);
  turnOff();
}

void turnOff() {
  uint32_t off = strip.Color(0, 0, 0);
  for(uint16_t i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, off);
  }
  strip.show();
}


void blink() {
  for(int i = 0; i < 2; i++) {
    showStatus(300);
    delay(200);
  } 
}

void colorWipe(uint8_t wait) {
  uint32_t c = strip.Color(255, 0, 0);
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    //delay(wait);
  }
}

void loop() {
  lidState = digitalRead(triggerPin);
  statusState = digitalRead(statusPin);
  incState = digitalRead(incPin);
  decState = digitalRead(decPin);
  infraredValue = analogRead(infraredPin);
  //Serial.print("Analog inc: ");
  //Serial.print(incState);
  
  if (lidState == LOW && prevLidState == HIGH) {
    //increment number cigarettes;
    smoked = smoked + 1;
    convertPercent2Colors();
    //Serial.print("\nSmoked:");
    //Serial.print(smoked);
    //lastCigarTime = millis();
  }
  else if (statusState == LOW && prevStatusState == HIGH) {
    //mapLeds();
    showStatus(800);
    //Serial.print("\nSTATUS");
    //colorWipe(50);
  }
  //Serial.print("\nincState = ");
  //Serial.print(incState);
  else if (incState == LOW && prevIncState == HIGH) {
    limit = limit + 1;
    convertPercent2Colors();
    //Serial.print("\nIncremented limit\nNew limit: ");
    //Serial.print(limit);
  }
  else if (decState == LOW && prevDecState == HIGH) {
    if (limit > 1) {
      limit = limit - 1;
      convertPercent2Colors();
      //Serial.print("\nIncremented limit\nNew limit: ");
      //Serial.print(limit);
    } else {
      //Serial.print("\nReached bottom limit 1");
    }
  }
  
  prevLidState = lidState;
  prevStatusState = statusState;
  prevIncState = incState;
  prevDecState = decState;

  //Infrared
  //Serial.print("InfraRed value: ");
  //Serial.println(infraredValue);

  //BLUETOOTH
  if(bluetooth.available())  // If the bluetooth sent any characters
  {
    // Send any characters the bluetooth prints to the serial monitor
    char data = (char)bluetooth.read();
    //Serial.println(data);
    switch(data) {
            //3: End of transmission
            case 3:  receiving = false;
                    limit = buffer2int(buffer);
                    Serial.println(limit);
                    convertPercent2Colors();
                    blink();
                      break; //end message
            default: if (receiving == false) resetData();
                    buffer[pos] = data;
                    pos++;
                    receiving = true;       
          }
  }
  if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }
  // and loop forever and ever!
}

void resetData(){
   for (int i=0; i<=pos; i++) buffer[i] = 0; 
   pos = 0;
}
    
int buffer2int(char* buffer){
  int i;
  sscanf(buffer, "%d", &i);
  return i;
}
