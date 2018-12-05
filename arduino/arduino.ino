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
const int infraredPin = 5;
const int emitterPin = 4;
const int RED = 1;
//const int GREEN = 2;
const int BLUE = 3;

// variables will change:
int lidState = LOW;
int prevLidState = LOW;
int prevStatusState = LOW;
int statusState = LOW;

int prevInfraredValue;
int infraredValue;
int emitterValue;

unsigned long initTime = millis();
int smoked = 0;
//unsigned long lastCigarTime = 0;
int limit = 10;
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

  //Calibracao
  infraredValue = analogRead(infraredPin);
  prevInfraredValue = infraredValue;
}

void resetLeds() {
  int i;
  for (i = 0; i < LEDS_SIZE; i++) {
    leds[i] = BLUE;
  }
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

void showStatus(int interval) {
  convertPercent2Colors();
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
  //lidState = digitalRead(triggerPin);
  statusState = digitalRead(statusPin);
  infraredValue = analogRead(infraredPin);
  emitterValue = analogRead(emitterPin);
  

  //if (prevInfraredValue - infraredValue > 5) initTime = millis();
  // && (millis() - initTime > 800) && (millis() - initTime < 3000)
  if (prevInfraredValue - infraredValue < -7) {
    smoked = smoked + 1;
    Serial.println("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
  }
  Serial.print("emitter: ");
  Serial.print(emitterValue);
  Serial.print(" | receiver: ");
  Serial.println(infraredValue);
  
  //if(prevInfraredValue - infraredValue < -7) {Serial.println("REEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE");}
  //delay(500);
  prevInfraredValue = infraredValue;

  /*Disabled button*/
  /*if (lidState == LOW && prevLidState == HIGH) {
    //increment number cigarettes;
    smoked = smoked + 1;
    convertPercent2Colors();
    //Serial.print("\nSmoked:");
    //Serial.print(smoked);
    //lastCigarTime = millis();
  }*/
  
  if (statusState == LOW && prevStatusState == HIGH) showStatus(800);
  
  prevLidState = lidState;
  prevStatusState = statusState;
  
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
  /*if(Serial.available())  // If stuff was typed in the serial monitor
  {
    // Send any characters the Serial monitor prints to the bluetooth
    bluetooth.print((char)Serial.read());
  }*/
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
